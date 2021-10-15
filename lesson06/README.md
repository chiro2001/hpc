# MPI

## 文档

[PDF](./MPI_实验.pdf)，[Markdown](./docs/MPI%20%E5%AE%9E%E9%AA%8C%20bda7c64d295e4f2d93795c57df6375ad.md)，[Online](https://www.chiro.work/MPI-daf5a27893d948b98f722b7233a4d9a6)。

## 代码说明

```text
.
├── CMakeLists.txt
├── MPI_实验.pdf
├── Makefile
├── README.md
├── data
│   ├── mem_plot_s10_m12_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s2_m6_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s5_m11_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s7_m9_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s8_m10_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s9_m11_r1_linear_sl4_t-10_d0.png
│   ├── mem_plot_s9_m11_r1_linear_sl4_t8_d0.png
│   ├── mpi_mem_test_s1_m11_r1_linear_sl4_t8_d0.png
│   ├── mpi_mem_test_s1_m6_r1_linear_sl4_t0.png
│   ├── mpi_mem_test_s4_m6_r1_linear_sl4_t0.png
│   ├── mpi_mem_test_s7_m10_r1_linear_sl4_t8.png
│   ├── mpi_mem_test_s8_m10_r1_linear_sl4_t-9_d0.png
│   ├── mpi_mem_test_s8_m10_r1_linear_sl4_t0_d0.png
│   ├── mpi_mem_test_s9_m11_r1_linear_sl4_t-8_d0.png
│   ├── mpi_mem_test_s9_m11_r1_linear_sl4_t0.png
│   ├── mpi_mem_test_s9_m11_r1_linear_sl4_t9.png
│   ├── mpi_mem_test_s9_m12_r1_linear_sl4_t-10_d0.png
│   ├── mpi_mem_test_s9_m12_r1_linear_sl4_t-8_d0.png
│   ├── mpi_mem_test_s9_m12_r1_linear_sl4_t8_d0.png
│   ├── mpi_server_s1_m7_r5_cubic_sl32_t0.png
│   ├── mpi_server_s1_m7_r5_linear_sl32_t0.png
│   ├── mpi_server_s7_m10_r1_linear_sl32_t4.png
│   ├── mpi_server_s7_m11_r1_linear_sl32_t0.png
│   ├── mpi_server_s9_m12_r1_linear_sl32_t4.png
│   ├── mpi_test_s1_m7_r1_linear.png
│   ├── mpi_wsl_s1_m7_r5_cubic_sl4.png
│   ├── mpi_wsl_s7_m10_r1_linear_sl4_t4.png
│   ├── mpi_wsl_s7_m11_r1_cubic_sl4.png
│   ├── mpi_wsl_s7_m11_r1_linear_sl4.png
│   └── mpi_wsl_s9_m12_r1_linear_sl4_t4.png
├── docs
│   ├── MPI 实验 bda7c64d295e4f2d93795c57df6375ad
│   │   ├── Untitled 1.png
│   │   ├── Untitled 2.png
│   │   ├── Untitled 3.png
│   │   ├── Untitled 4.png
│   │   └── Untitled.png
│   └── MPI 实验 bda7c64d295e4f2d93795c57df6375ad.md
├── mpi_demo.c
├── mul_test_all.py
├── requirements.txt
└── src
    ├── mat.c
    ├── mat.h
    ├── mat_mul_test.c
    ├── task.c
    ├── task.h
    ├── utils.c
    └── utils.h

5 directories, 80 files
```

## 运行方式

直接调用 Python 脚本编译并且运行，自动记录数据

```shell
python3 -m pip install -r requirements.txt
python3 mul_test_all.py --help
python3 mul_test_all.py -m 11 -s 9 -r 1 -o mem_plot -f linear -e 0.01 -t 8
```

Python 脚本的帮助信息：

``` shell
➜  lesson05 git:(master) ✗ python3 mul_test_all.py --help
usage: mul_test_all.py [-h] [-m MAX_N] [-n SLOT_N] [-s START_N] [-t TASK_START] [-d TASK_END] [-r REPEATE] [-b {make,cmake}] [-f {cubic,linear}] [-e RECORD_DELTA] [-o OUT]

optional arguments:
  -h, --help            show this help message and exit
  -m MAX_N, --max-n MAX_N
                        设置最大 N，N = 2^k，请输入 k。
  -n SLOT_N, --slot-n SLOT_N
                        设置 MPI 使用的 slot 数量，0 表示所有。
  -s START_N, --start-n START_N
                        设置开始 N，N = 2^k，请输入 k。
  -t TASK_START, --task-start TASK_START
                        设置开始任务 task_id，默认全部 task。
  -d TASK_END, --task-end TASK_END
                        设置结束任务 task_id，默认全部 task。
  -r REPEATE, --repeate REPEATE
                        设置每个 N 重复多少次取平均值。
  -b {make,cmake}, --build-method {make,cmake}
                        设置编译方式……暂时没写 Makefile。
  -f {cubic,linear}, --fitting {cubic,linear}
                        设置曲线拟合方式: 二次拟合或线性。
  -e RECORD_DELTA, --record-delta RECORD_DELTA
                        设置内存记录时间间隔。
  -o OUT, --out OUT     设置图片保存基础文件名(不含扩展名)。
```


