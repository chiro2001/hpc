import os
import time
import argparse
import psutil
import platform
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d


def change_path(target: str = 'build_mkfile'):
    if not os.path.exists(target):
        os.mkdir(target)
    os.chdir(target)


def build_it(build_method: str = 'make'):
    if build_method == 'make':
        print("Building excutable file by Make: mat_mul_test...")
        if os.path.exists("build"):
            os.system("rm -rf build/")
        try:
            os.system("make")
        except Exception as e:
            print(f"Meets error: {e}, check it.")
            return
        change_path(target='build_mkfile')
    else:
        change_path(target='build')
        if not os.path.exists("mat_mul_test"):
            print("Building excutable file by CMake: mat_mul_test...")
            try:
                os.system("cmake .. && make")
            except Exception as e:
                print(f"Meets error: {e}, check it.")
                return
        else:
            try:
                os.system("make mat_mul_test")
            except Exception as e:
                print(f"Meets error: {e}, check it.")
                return


def test_all(start_n=4,
             max_n: int = 8,
             out: str = 'plot.png',
             repeate: int = 1,
             fitting: str = 'cubic',
             task_start: int = 0,
             slot_n: int = 0,
             record_delta: float = 0.1, **kwargs):
    # plt.rcParams['font.sans-serif'] = ['FSGB2312']
    build_it(**kwargs)

    # if os.path.exists("../data"):
    #     os.system("rm -rf ../data")
    # os.mkdir("../data")
    if not os.path.exists("../data"):
        os.mkdir("../data")

    if slot_n == 0:
        slot_n = (os.system("mpirun mat_mul_test -1") & 0xFF00) >> 8
        print(f"Running on {slot_n} slot(s)")

    label_names: list = None
    memories_record = {}

    results = {}
    # for k in [(2 ** (max_n + 1)) // (2 ** i) for i in range((max_n + 1), 0, -1)]:
    for k in [int(2 ** (x / 2)) for x in range(start_n * 2, max_n * 2)]:
        # for k in range(0, 2 ** max_n, 10):
        for _ in range(repeate):
            # os.system(f"./mat_mul_test {k} > /dev/null")
            # 后台启动进程，记录 pid
            os.system(
                f"mpirun {'' if slot_n == 0 else f'-n {slot_n}'} mat_mul_test {k} {task_start}")
            # pid = (os.system('''ps -ef | grep "mat_mul_test" | grep -v grep | awk '{print $2}'''') & 0xFF00) >> 8
            pid = int(os.popen(
                '''ps -ef | grep "mat_mul_test" | grep -v grep | awk '{print $2}' ''').readlines()[0])
            memories = []
            time_start = time.time()
            # 持续记录内存大小
            # 等待跑完
            while len(os.popen('''ps -ef | grep "mat_mul_test" | grep -v grep | awk '{print $2}' ''').readlines()) > 0:
                try:
                    mem_bytes = int(os.popen('''ps aux | grep "python3" | grep -v grep | awk '{print $6}' ''').readlines()[0])
                except IndexError:
                    break
                memories.append((time.time() - time_start, mem_bytes))
                time.sleep(record_delta)
            memories_record[k] = memories
            with open("results.txt", 'r') as f:
                # results_now = np.array([float(x) for x in f.readlines()])
                lines = f.readlines()
                results_now = np.array(
                    [abs(float(x.split(': ')[1])) for x in lines])
                if label_names is None:
                    label_names = [x.split(': ')[0] for x in lines]
                    # print(label_names)
                # lines_data = {x.split(': ')[0]: x.split(': ')[1] for x in lines}
                if k not in results:
                    results[k] = results_now
                else:
                    results[k] = results[k] + results_now
        results[k] = results[k] / repeate
        print(f'N = {k:4}', ', '.join(
            [f"{label_names[i]} = {results[k][i]:.3}" for i in range(len(label_names))]))

    # 画出图像
    # 绘制平滑曲线
    def interp1d_data(X_data, Y_data):
        # print(X_data, Y_data)
        cubic_interploation_model = interp1d(X_data, Y_data, kind=fitting)
        xs = np.linspace(np.min(X_data), np.max(X_data), 500)
        ys = cubic_interploation_model(xs)
        return ys
    X = list(results.keys())
    X_linear = np.linspace(np.min(X), np.max(X), 500)
    Y = [interp1d_data(X, [results[k][z] for k in X])
         for z in range(len(label_names))]
    # plt.title(f'Running Time for {", ".join(label_names[:-1])} and {label_names[-1]}')
    fig1, (ax1, ax2) = plt.subplots(nrows=2, ncols=1, sharex=True)
    ax1.set_title(f'On {platform.platform()}\n{psutil.cpu_count()} Core(s) {(psutil.cpu_freq().current / 1000):.3}GHz, {slot_n} Slot(s)' +
              '$N' r'\in' '[2^{' + str(start_n) + '}, 2^{' + str(max_n) + '}]$, ' + f'R={repeate}, {len(label_names)} items, {fitting} fitting.')
    ax1.set_xlabel("N")
    ax1.set_ylabel("Time(s)")
    [ax1.plot(X_linear, Y[i]) for i in range(len(Y))]
    ax1.legend(label_names, loc='upper left')

    ax2.set_title(f"Memory usage")
    ax2.set_xlable('Time')
    ax2.set_ylable('Memory/MB')
    k_names = [f"N = {k}" for k in memories_record]
    for k in memories_record:
        ax2.plot(*(np.array(memories_record[k]).T))
    ax2.legend(k_names, loc='upper left')
    if not out.endswith('.png'):
        out = out + '.png'
    save_path = os.path.join(
        "../data/", '.'.join(out.split('.')[:-1]) + f"_s{start_n}_m{max_n}_r{repeate}_{fitting}_sl{slot_n}_t{task_start}")
    plt.savefig(save_path)
    print(f'Saved image file: {save_path}')
    plt.clf()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--max-n', type=int,
                        default=7, help='设置最大 N，N = 2^k，请输入 k。')
    parser.add_argument('-n', '--slot-n', type=int,
                        default=0, help='设置 MPI 使用的 slot 数量，0 表示所有。')
    parser.add_argument('-s', '--start-n', type=int,
                        default=3, help='设置开始 N，N = 2^k，请输入 k。')
    parser.add_argument('-t', '--task-start', type=int,
                        default=0, help='设置开始任务 task_id，默认全部 task。')
    parser.add_argument('-r', '--repeate', type=int,
                        default=1, help='设置每个 N 重复多少次取平均值。')
    parser.add_argument('-b', '--build-method', type=str, default='cmake',
                        help='设置编译方式……暂时没写 Makefile。', choices=['make', 'cmake'])
    parser.add_argument('-f', '--fitting', type=str, default='cubic',
                        help='设置曲线拟合方式: 二次拟合或线性。', choices=['cubic', 'linear'])
    parser.add_argument('-e', '--record-delta', type=float, default=0.1,
                        help='设置内存记录时间间隔。')
    parser.add_argument('-o', '--out', type=str, default='plot.png',
                        help='设置图片保存基础文件名(不含扩展名)。')

    args = parser.parse_args()
    test_all(**args.__dict__)
