import os
import argparse
from posixpath import join
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


def test_all(show_log: bool = False, max_n: int = 8, **kwargs):
    build_it(**kwargs)

    if os.path.exists("../data"):
        os.system("rm -rf ../data")
    os.mkdir("../data")

    results = {}
    for k in [(2 ** (max_n + 1)) // (2 ** i) for i in range((max_n + 1), 0, -1)]:
        os.system(f"./mat_mul_test {k} > /dev/null")
        with open("results.txt", 'r') as f:
            results_now = [float(x) for x in f.readlines()]
            results[k] = results_now
            print(f"N = {k:4}, time_native = {results[k][0]:9}, time_OpenBLAS = {results[k][1]:9}")
    # print(results)
    # 画出图像

    # 绘制平滑曲线
    def interp1d_data(X_data, Y_data):
        cubic_interploation_model = interp1d(X_data, Y_data, kind="cubic")
        xs = np.linspace(np.min(X_data), np.max(X_data), 500)
        ys = cubic_interploation_model(xs)
        return ys
    X = list(results.keys())
    X_linear = np.linspace(np.min(X), np.max(X), 500)
    Y1, Y2 = [interp1d_data(X, [results[k][z] for k in results]) for z in range(2)]
    plt.title('Running Time for Native mul and OpenBLAS')
    plt.xlabel("N")
    plt.ylabel("Time")
    plt.plot(X_linear, Y1, X_linear, Y2)
    plt.legend(('Native', 'OpenBLAS'), loc='upper right')
    plt.savefig("../data/plot.png")
    print('Saved image file: ../data/plot.png')
    plt.clf()

    # 画一画 log 看看
    if show_log:
        Y1_log, Y2_log = [interp1d_data(X, [np.log(results[k][z]) for k in results]) for z in range(2)]
        plt.title('Running Time (log) for Native mul and OpenBLAS')
        plt.xlabel("N")
        plt.ylabel("Time (log)")
        plt.plot(X_linear, Y1_log, X_linear, Y2_log)
        plt.legend(('Native', 'OpenBLAS'), loc='upper right')
        plt.savefig("../data/plot_log.png")
        print('Saved image file: ../data/plot_log.png')
        plt.clf()
    


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', '--show-log', default=False, help='绘制 log 图像', action='store_const', const=True)
    parser.add_argument('-m', '--max-n', type=int, default=8, help='设置最大 N，N = 2^k，请输入 k。')
    parser.add_argument('-b', '--build-method', type=str, default='make', help='设置编译方式，两者都可以。', choices=['make', 'cmake'])

    args = parser.parse_args()
    test_all(**args.__dict__)

