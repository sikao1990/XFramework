#include "Child2.h"
#include "Base.h"
#include "MemTest.h"

Child2::Child2(){}
Child2::~Child2(){}
void Child2::method(){ std::cout<<"child2"<<std::endl; }

//DEFINENODE(Base,Child2,"Child2",Base,ALLOCMEM);
DEFINENODE(Base,Child2,"Child2",Base,Alloc);
