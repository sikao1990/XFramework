#ifndef BASE_H
#define BASE_H

#include "TreeRelation.h"

class Base{
public:
	Base();
	virtual ~Base();
	
	virtual void method();

	DECLAREBASEMETHOD(Base);
	DECLARENODE(Base);
};

#endif
