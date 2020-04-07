#ifndef CHILD2_H
#define CHILD2_H

#include "Base.h"
#include "TreeRelation.h"

class Child2 : public Base{
public:
	Child2();
	~Child2();
public:
	void method();
	
	DECLARENODE(Child2);
};

#endif
