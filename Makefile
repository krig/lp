test.ll: test.c
	clang -O2 -S -emit-llvm $<

all: test.ll
