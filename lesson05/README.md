# OpenMP 尝试

## 文档

[PDF](./OpenMP_初试.pdf)，[Markdown](./docs/OpenMP%20%E5%88%9D%E8%AF%95%20da6bdc1cb8b449ebbb22aaf57028f5c6.md)，[Online](https://www.chiro.work/OpenMP-d1b7981c902f4c589b5a97bd8c72c2af)。

## 代码说明

```text
.
├── CMakeLists.txt            # CMake 编译工程
├── Makefile                  # Makefile 编译工程
├── OpenMP_初试.pdf           # 实验文档
├── README.md                 # 实验说明
├── build_mkfile/              # Makefile 编译后文件夹
├── data                      # 实验数据记录
│   ├── openmp_server_s1_m7_r50_cubic.png
│   ├── openmp_server_s7_m11_r2_linear.png
│   ├── openmp_wsl_s1_m7_r50_cubic.png
│   ├── openmp_wsl_s7_m11_r2_linear.png
│   └── openmp_wsl_s7_m9_r2_linear.png
├── data_old/                 # 旧实验数据
├── docs/                     # 文档
├── mul_test_all.py           # 编译、运行、测试、绘图用脚本
├── openmp_demo.cpp           # 小 Demo
├── requirements.txt          # Python 依赖
├── results.txt               # 临时文件
├── src                      # 源代码目录
│   ├── mat.c                # 存放矩阵计算相关代码
│   ├── mat.h
│   ├── mat_mul_test.c       # 进行单次实验并且计时
│   ├── utils.c              # 工具函数
│   └── utils.h
└── test_openmp.sh            # 测试 OpenMP 是否运行的脚本

6 directories, 46 files
```

## 运行方式

直接调用 Python 脚本编译并且运行，自动记录数据

```shell
python3 -m pip install -r requirements.txt
python3 mul_test_all.py --help
python3 mul_test_all.py --max-n 9 --start-n 7 --repeate 2 --fitting linear --out openmp_wsl
```

Python 脚本的帮助信息：

``` shell
➜  lesson05 git:(master) ✗ python3 mul_test_all.py --help
usage: mul_test_all.py [-h] [-m MAX_N] [-s START_N] [-r REPEATE] [-b {make,cmake}] [-f {cubic,linear}] [-o OUT]

optional arguments:
  -h, --help            show this help message and exit
  -m MAX_N, --max-n MAX_N
                        设置最大 N，N = 2^k，请输入 k。
  -s START_N, --start-n START_N
                        设置开始 N，N = 2^k，请输入 k。
  -r REPEATE, --repeate REPEATE
                        设置每个 N 重复多少次取平均值。
  -b {make,cmake}, --build-method {make,cmake}
                        设置编译方式，两者都可以。
  -f {cubic,linear}, --fitting {cubic,linear}
                        设置曲线拟合方式: 二次拟合或线性。
  -o OUT, --out OUT     设置图片保存基础文件名(不含扩展名)。
```


