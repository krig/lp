test.ll: test.c
	clang -S -emit-llvm $<

all: test.ll
