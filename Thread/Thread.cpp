#include "Thread.h"

#ifdef	WIN32
#include <windows.h>
#include "ThreadImpl_Win32.h"
#else
#include <unistd.h>
#include "ThreadImpl_Linux.h"
#endif

Thread::Thread()
{
	_Imp = new ThreadImpl<Thread>(this,&Thread::run);
}

Thread::~Thread()
{
	delete _Imp;
}

bool Thread::start()
{
	return _Imp->start();
}

void Thread::run()
{
	
}

unsigned Thread::GetThreadID()
{
	return _Imp->GetThreadID();
}

void Thread::stop()
{
	_Imp->stop();
}

bool Thread::IsRuning()
{
	return _Imp->IsRuning();
}

void Thread::mSleep(unsigned ms)
{
#ifdef	WIN32
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}
