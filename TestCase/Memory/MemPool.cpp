#include <stdio.h>
#include "../../XPublic.h"

int main()
{
	MemPool pool;
	const int ARRCOUNT = 2;
	pool.Init();
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	char* parr[ARRCOUNT] = {};
	for(int i=0;i<ARRCOUNT;i++)
		parr[i] = (char*)pool.Alloc(32);//Alloc
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	strcpy(parr[1],"helloworld");
	strcpy(parr[0], "god like!hello worldhello world");
	printf("%s\n",parr[1]);
	pool.Free(parr[1]);//Free
	puts("==========================");
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	puts("start realloc");
	char* p=(char*)pool.Realloc(parr[0], 64);//Realloc
	if (NULL != p) {
		strcat(p, "fuck you!");
		printf("realloc:%s\n", p);
	}
	else {
		puts("realloc failed");
	}
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	while (1)
		Thread::mSleep(1000);
    return 0;
}