#ifndef __ShareMemoryImpl_H__
#define __ShareMemoryImpl_H__

class ShareMemoryImpl
{
public:
	ShareMemoryImpl();
	bool Open(const char* str,int size)const;
	bool Attach()const;
	void* GetAddress();
	const void* GetAddress()const;
	void Detach()const;
	void Close()const;
private:
	ShareMemoryImpl(const ShareMemoryImpl&);
	ShareMemoryImpl& operator=(const ShareMemoryImpl&);	
private:
	int 	m_shmid;
	void* 	m_pStart;
};

#endif	// ShareMemoryImpl.h
