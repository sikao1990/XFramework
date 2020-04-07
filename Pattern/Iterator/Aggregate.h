#ifndef __Aggregate_H__
#define __Aggregate_H__

class Iterator;
class Aggregate
{
public:
	virtual ~Aggregate(){}
	virtual Iterator* CreateIterator() = 0;
	virtual void* GetItem(int idx){ return NULL; }
	virtual void* GetItem(void* pKey){ return NULL; }
	virtual int GetSize() = 0;
	virtual void ReleaseIterator(Iterator* pIt) = 0;
protected:
	Aggregate(){}
private:
};

#endif	// Aggregate.h
