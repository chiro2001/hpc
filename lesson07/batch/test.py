import json
import traceback
import copy
import time
import os

template = '''HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
test.out     output file name (if any)
6            device out (6=stdout,7=stderr,file)
1            # of problems sizes (N)
{Ns}         Ns
1            # of NBs
{NBs}        NBs
1            PMAP process mapping (0=Row-,1=Column-major)
1            # of process grids (P x Q)
{Ps}         Ps
{Qs}         Qs
16.0         threshold
3            # of panel fact
0 1 2        PFACTs (0=left, 1=Crout, 2=Right)
2            # of recursive stopping criterium
2 4          NBMINs (>= 1)
1            # of panels in recursion
2            NDIVs
3            # of recursive panel fact.
0 1 2        RFACTs (0=left, 1=Crout, 2=Right)
1            # of broadcast
0            BCASTs (0=1rg,1=1rM,2=2rg,3=2rM,4=Lng,5=LnM)
1            # of lookahead depth
0            DEPTHs (>=0)
2            SWAP (0=bin-exch,1=long,2=mix)
64           swapping threshold
0            L1 in (0=transposed,1=no-transposed) form
0            U  in (0=transposed,1=no-transposed) form
1            Equilibration (0=no,1=yes)
8            memory alignment in double (> 0)
'''

results = {}


def update_dat_file(run_args: dict, template_: str = template, target: str = 'openblas', local: bool = True) -> bool:
    data = template_.format(**run_args)
    try:
        if local:
            with open('HPL.dat', 'w', encoding='utf8') as f:
                f.write(data)
        else:
            with open(os.path.join(os.path.join('build', target), 'HPL.dat'), 'w', encoding='utf8') as f:
                f.write(data)
    except Exception:
        traceback.print_exc()
        return False
    return True


def run(run_args: dict, target: str = 'openblas') -> float:
    update_dat_file(run_args=run_args, target=target, local=True)
    os.system("rm nohup.out")
    os.system(f"nohup mpirun -n 28 ./build/{target}/xhpl &")
    time.sleep(0.2)
    pid = int(os.popen(f'ps aux | grep "mpirun.*xhpl"').readline().split()[1])
    print('pid', pid)
    lines_count = 10
    unit, flops = None, None
    while True:
        if len(os.popen(f'ps aux | grep {pid}').readline()) == 0:
            break
        try:
            lines = os.popen(f"tail -n {lines_count} nohup.out").readlines()
            if len(lines) == lines_count and lines[0].startswith('=' * 10) and lines[1].startswith("T/V") and len(lines[3].split()) == 7:
                unit = lines[1].split()[-1]
                flops = float(lines[3].split()[-1])
                break
        except Exception:
            traceback.print_exc()
    # os.system(f"kill {pid}")
    os.system(f"killall mpirun")
    print(f"{flops} {unit} {run_args}")
    results[f"P{run_args['Ps']}_Q{run_args['Qs']}_N{run_args['Ns']}"] = flops
    with open("result.json", "w", encoding="utf8") as f:
        json.dump(results, f, indent=2, sort_keys=True)
    if unit == 'Tflops':
        flops *= 1024
    return flops


def generate_args(run_args_range: dict, cores: int = 16) -> dict:
    tails = {}
    now = {}
    for key in run_args_range:
        if not isinstance(run_args_range[key], list):
            continue
        tails[key] = 0
        now[key] = run_args_range[key][0]
    print("keys:", list(run_args_range.keys()))

    while True:
        for key in run_args_range:
            if not isinstance(run_args_range[key], list):
                continue
            now[key] = run_args_range[key][tails[key]]
        if now['Qs'] * now['Ps'] != cores:
            continue
        now_data = copy.deepcopy(now)
        yield now_data
        p = 0
        over: bool = False
        while True:
            key = str(list(run_args_range.keys())[p])
            tails[key] += 1
            if tails[key] >= len(run_args_range[key]):
                print(f"tails[{key}] {tails[key]} => 0, p {p} => {p + 1}")
                tails[key] = 0
            else:
                break
            p += 1
            if p >= len(run_args_range.keys()):
                over = True
                break

        if over:
            break


# def test_all(run_args_range: dict):
#     for run_args in generate_args(run_args_range):
#         print(run_args)
#         run(run_args)

def run_args_list(run_args_li: list):
    for run_args in run_args_li:
        if isinstance(run_args, list):
            run_args_list(run_args)
        else:
            run(run_args=run_args)


if __name__ == '__main__':
    # test_all({
    #     "Ns": [10000, ],
    #     "NBs": [256, ],
    #     "Ps": [2, ],
    #     "Qs": [8, ]
    # })
    # run_args_list([
    #     {
    #         "Ns": 10000,
    #         "NBs": 256,
    #         "Ps": 2,
    #         "Qs": 8
    #     },
    # ])

    # run_args_list([[{"Ns": Ns, "NBs": NBs, "Ps": 2, "Qs": 8} for Ns in range(8000, 20000, 4000)] for NBs in [64, 128, 256]])
    # run_args_list([[{"Ns": Ns, "NBs": 128, "Ps": d[0], "Qs": d[1]} for Ns in range(8000, 20000, 4000)] for d in [(1, 16), (2, 8), (4, 4)]])
    run_args_list([{"Ns": Ns, "NBs": 128, "Ps": 2, "Qs": 7} for Ns in range(10000, 30000, 10000)])
    print(results)
    with open("result.json", "w", encoding="utf8") as f:
        json.dump(results, f, indent=2, sort_keys=True)
    
