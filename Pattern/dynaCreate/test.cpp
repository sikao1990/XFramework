#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include "Base.h"

typedef bool (*InitPtr)();

using namespace std;

int main(){				//主程序可在不知道相应模块类名情况下得到实例化对象
	vector<Base*>	objs;
	void* pHandle1 = dlopen("./libChild1.so",RTLD_NOW);
	void* pHandle2 = dlopen("./libChild2.so",RTLD_NOW);
	if(NULL==pHandle1){
		puts("cont find so1");
		return 1;
	}
	if(NULL==pHandle2){
		puts("cont find so2");
		return 2;
	}
	InitPtr initFuncPtr1= (InitPtr)dlsym(pHandle1,"InitChild1");
	InitPtr initFuncPtr2= (InitPtr)dlsym(pHandle2,"InitChild2");
	if(NULL==initFuncPtr1){
		puts("cont find func1");
		return 3;
	}
	if(NULL==initFuncPtr2){
		puts("cont find func2");
		return 4;
	}
	initFuncPtr1();
	initFuncPtr2();
	for(int i=0;i<5;i++){
		Base* child=Base::getInstance("Child1");
		Base* childptr=Base::getInstance("Child2");
		objs.push_back(child);
		objs.push_back(childptr);
	}
	int i=0;
	for(;i<10;i++){
		objs[i]->method();
	}
	cout << "i=" << i << endl;
	dlclose(pHandle1);
	dlclose(pHandle2);
	return 0;
}

