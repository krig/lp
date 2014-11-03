#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int a = 0;

int foo(int x, int y) {
	return x + y;
}

int main(int argc, char** argv)
{
	int i;
	for (i = 0; i < argc; ++i) {
		printf("%s\n", argv[i]);
	}
	int d = 3;
	int b = 6;
	printf("%d + %d = %d\n", d, b, foo(d, b));
	a = foo(d, b);
	return 0;
}


