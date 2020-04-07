#include "MemTest.h"
#include <stdio.h>

void* Alloc(int size)
{
	puts("from globle func");
	return new char[size];
}
