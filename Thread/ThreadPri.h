#ifndef __ThreadPri_H__
#define __ThreadPri_H__
#include <sys/types.h>

class ThreadPri
{
public:
	virtual ~ThreadPri(){}
	virtual bool start()=0;
	virtual void stop()=0;
	virtual size_t GetThreadID()=0;
	virtual bool IsRuning()=0;
};

#endif	// ThreadPri.h
