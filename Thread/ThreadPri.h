#ifndef __ThreadPri_H__
#define __ThreadPri_H__

class ThreadPri
{
public:
	virtual ~ThreadPri(){}
	virtual bool start()=0;
	virtual void stop()=0;
	virtual unsigned GetThreadID()=0;
	virtual bool IsRuning()=0;
};

#endif	// ThreadPri.h
