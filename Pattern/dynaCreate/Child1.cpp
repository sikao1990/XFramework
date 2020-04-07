#include "Child1.h"
#include "Base.h"
#include "MemTest.h"

Child1::Child1(){}
Child1::~Child1(){}
void Child1::method(){
	std::cout<<"child1"<<std::endl;
}

//DEFINENODE(Base,Child1,"Child1",Base,ALLOCMEM);
DEFINENODE(Base,Child1,"Child1",Base,Alloc);
