#ifndef __SharedMemoryImpl_H__
#define __SharedMemoryImpl_H__

class SharedMemoryImpl
{
public:
	SharedMemoryImpl();
	bool Open(const char* str,int size)const;
	bool Attach()const;
	void* GetAddress();
	const void* GetAddress()const;
	void Detach()const;
	void Close()const;
private:
	SharedMemoryImpl(const SharedMemoryImpl&);
	SharedMemoryImpl& operator=(const SharedMemoryImpl&);	
private:
	int 	m_shmid;
	void* 	m_pStart;
};

#endif	// SharedMemoryImpl.h
