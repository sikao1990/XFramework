#ifndef __Thread_H__
#define __Thread_H__

class ThreadPri;

#include "../XDefine.h"
class FRAMEWORK_API Thread
{
public:
	Thread();
	virtual ~Thread();
	bool start();
	virtual void stop();
	unsigned GetThreadID();
	bool IsRuning();
	static void mSleep(unsigned ms);
protected:
	virtual void run();
private:
	ThreadPri*			_Imp;
};


#endif // Thread.h


