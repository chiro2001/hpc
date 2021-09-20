build_path:=build_mkfile
OpenBLAS_INCLUDE_DIRS:=/usr/share/openblas/include
OpenBLAS_LIBRARIES:=/usr/share/openblas/lib

build:
# 神创造了世界，神的福音润泽世间。
	$(shell if [ ! -d ${build_path} ]; then mkdir ${build_path}; fi)

# 首先是犯下懒惰之罪的 mat.c，
# 仗着自己有一些矩阵运算函数，
# 就希望别的文件来静态链接它。
# 然后是犯下傲慢之罪的 mat_mul_test.c，
# 自己只实现了一个函数就敢教贵为 OpenBLAS 的 cblas_dgemm 做事，
# 没有任何一点自知之明。
# 最后是犯下暴食之罪的 mat_mul_test，
# 它疯狂吞噬了 mat.o 和 mat_mul_test.o 不算，
# 还残忍吞入了 libopenblas.so，实在是欲望滔天！
	gcc src/mat.c src/mat_mul_test.c src/utils.c \
		-o ${build_path}/mat_mul_test \
		-lm \
		-L${OpenBLAS_LIBRARIES} \
		-I${OpenBLAS_INCLUDE_DIRS} \
		-lopenblas \
		-static \
		-lpthread \
		-O3 \
		-mavx

run: build
# 所以，神降下了祂的神罚，惩罚作为世界载体的 CPU 高负荷执行一段无用的计算
# 然后将此记录于石碑之上。
	./build_mkfile/mat_mul_test 8

clean:
# 仁慈的父，我已坠入，
# 看不见罪的国度。
# 请原谅我，我的自负，
# 盖着一道孤独。
	rm -rf build_mkfile/
