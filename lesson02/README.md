# GCC 的使用和矩阵乘法性能比较

**文档**

[PDF](./gcc的使用和矩阵乘法性能比较.pdf)，[Markdown](./docs/gcc 使用和矩阵乘法性能比较 b9e3e2ef8c954cd0b07ce8df4d225a37.md)，[Online](https://www.chiro.work/gcc-b9e3e2ef8c954cd0b07ce8df4d225a37)。

**代码说明**

```
.
├── CMakeLists.txt
├── Makefile
├── README.md
├── build/*				# CMake 编译目录
├── build_mkfile/*		# Make 编译目录
├── data				# 实验结果数据
│   └── plot.png
├── docs				# 实验文档
├── gcc的使用和矩阵乘法性能比较.pdf
├── mul_test_all.py		# 实验的主程序
├── requirements.txt	# Python 依赖
└── src					# 实验源文件
    ├── mat.c
    ├── mat.h
    └── mat_mul_test.c
```

**运行方式**

1. 编译运行

   1. `make`编译：`make run && make clean`：运行单次性能比较。

   2. `cmake`编译：

      ```shell
      mkdir build
      cd build
      cmake ..
      make
      ```

2. 运行实验

   ```shell
   python3 -m pip install -r requirements.txt
   python3 mul_test_all.py -h
   python3 mul_test_all.py
   ```

   

具体运行方式请查看`docs/*.md`。