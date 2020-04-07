#ifndef CHILD1_H
#define CHILD1_H

#include "Base.h"
#include "TreeRelation.h"

class Child1 : public Base{
public:
	Child1();
	~Child1();
public:
	void method();
public:
	DECLARENODE(Child1);
};

#endif
