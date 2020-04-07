#ifndef __Thread_H__
#define __Thread_H__

class ThreadPri;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API Thread
#else
class Thread
#endif
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


