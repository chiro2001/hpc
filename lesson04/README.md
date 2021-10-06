# Linux 环境多线程编程

## 文档

[PDF](./Linux_环境多线程编程.pdf)，[Markdown](./docs/Linux%20%E7%8E%AF%E5%A2%83%E5%A4%9A%E7%BA%BF%E7%A8%8B%E7%BC%96%E7%A8%8B%206d25302e391241f5ba4734720fc1d3e9.md)，[Online](https://www.chiro.work/Linux-0b58eba7e9154235a5b8ea7fb88d2d39)。

## 代码说明

```text
.
├── data
├── src_http                     # 多线程 HTTP 服务器代码
│   ├── run.sh                   # 辅助 client
│   ├── socket_server_threads.c     # 主要测试代码
│   ├── thread_demo_single_var.c    # 多线程 demo
│   └── thread_demo_single_var_s1.c # 多线程 demo
└── src_mat                      # 多线程矩阵乘法
    ├── CMakeLists.txt           # CMake 编译工程
    ├── Linux_环境多线程编程.pdf   # 实验文档
    ├── Makefile                 # Makefile 编译工程
    ├── README.md                # 实验说明
    ├── build_mkfile             # Makefile 编译后文件夹
    │   ├── mat_mul_test         # 多线程矩阵乘法可执行文件
    │   └── results.txt          # 临时储存数据用文件
    ├── data/                    # Python 绘制图像结果
    ├── docs/                    # 文档
    ├── mul_test_all.py          # 编译、运行、测试、绘图用脚本
    ├── requirements.txt         # Python 依赖
    ├── results.txt              # 临时文件
    ├── src                      # 源代码目录
    │   ├── mat.c                # 存放矩阵计算相关代码
    │   ├── mat.h
    │   ├── mat_mul_test.c       # 进行单次实验并且计时
    │   ├── utils.c              # 工具函数
    │   └── utils.h
    └── sse_test.c               # 测试 SIMD 部分指令集是否可用

8 directories, 30 files
```

## 运行方式

1. 多线程 HTTP 服务器
   1. 编译运行

      ```shell
      gcc -o socket_server_threads socket_server_threads.c -lpthread
      ./socket_server_threads 8088 1024
      # 在 8088 端口运行服务器，并且启动 1024 个 wget 进程对服务器进行请求，并且计算整体时间。
      ```

2. 多线程矩阵乘法
   1. 直接调用 Python 脚本编译并且运行，自动记录数据

      ```shell
      python3 -m pip install -r requirements.txt
      python3 mul_test_all.py --help
      python3 mul_test_all.py --start-n 1 --max-n 10 --build-method make --out plot.png 
      ```

      Python 脚本的帮助信息：

      ``` shell
      ➜  src_mat git:(master) ✗ python3 mul_test_all.py --help
      usage: mul_test_all.py [-h] [-m MAX_N] [-s START_N] [-b {make,cmake}] [-o OUT]

      optional arguments:
      -h, --help            show this help message and exit
      -m MAX_N, --max-n MAX_N
                              设置最大 N，N = 2^k，请输入 k。
      -s START_N, --start-n START_N
                              设置开始 N，N = 2^k，请输入 k。
      -b {make,cmake}, --build-method {make,cmake}
                              设置编译方式，两者都可以。
      -o OUT, --out OUT     设置图片保存文件名。
      ```


