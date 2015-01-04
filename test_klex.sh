#!/bin/sh
clang -O2 -S -emit-llvm klex.c
gcc -o klex -g klex.c
./klex <<EOF
# this is test klex input!+12-3
return a + b12;
"this is a string"
r"this is a \raw \n \string"
"this is \not a raw string"
10
120.f
0x10
0b10_01
run run shaw
foo+barn -- -+= -=+++=a12-+>> <<><>=<====a!!!=!b/if/else/elif/for/return()[and|||&&is&in]
EOF
