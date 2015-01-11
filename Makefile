test.ll: test.cpp
	clang -O2 -S -emit-llvm $<

all: test.ll
