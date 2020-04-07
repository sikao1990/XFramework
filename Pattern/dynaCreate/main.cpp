#include <stdio.h>
#include <iostream>
#include <vector>
#include "TreeRelation.h"
#include "Base.h"
#include "Child1.h"
#include "Child2.h"
using namespace std;
/* Base.h
#include "TreeRelation.h"
class Base{
public:
	Base();
	virtual ~Base();
	virtual void method();
	DECLAREBASEMETHOD(Base);
	DECLARENODE(Base);
};
Base::Base(){}
Base::~Base(){}
void Base::method(){	}
DEFINEBASEMETHOD(Base);
DEFINENODE(Base,Base,"Base",Base,ALLOCMEM);
*/
/* Child1.h
class Child1 : public Base{
public:
	void method();
public:
	DECLARENODE(Child1);
};
void Child1::method(){
	std::cout<<"child1"<<std::endl;
}
//DEFINENODE(Base,Child1,"Child1",Base,ALLOCMEM);
DEFINENODE(Base,Child1,"Child1",Base,Alloc);
*/
int main(){				//主程序可在不知道相应模块类名情况下得到实例化对象
	AddToParentX(Base,"Base",&Child1::s_Child1);// AddToParent(Base,Child1);
	AddToParentX(Base,"Base",&Child2::s_Child2);// AddToParent(Base,Child2);
	vector<Base*>	objs;
	for(int i=0;i<5;i++){
		Base* child=Base::getInstance("Child1");
		Base* childptr=Base::getInstance("Child2");
		if(NULL==child)
			return 1;
		objs.push_back(child);
		objs.push_back(childptr);
	}
	int i=0,res=0;
	for(;i<10;i++)
		objs[i]->method();
	res=RelationCmpX(Base,"Base","Child2");
	printf("relation:%d\n",res);
	return 0;
}