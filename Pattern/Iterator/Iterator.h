#ifndef __Iterator_H__
#define __Iterator_H__

class Iterator
{
public:
	virtual ~Iterator(){}
	virtual void First() = 0;
	virtual void Next() = 0;
	virtual bool IsDone() = 0;
	virtual void* CurrentItem() = 0;
protected:
	Iterator(){}
private:
};

#endif	// Iterator.h
