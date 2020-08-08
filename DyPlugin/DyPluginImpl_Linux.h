#ifndef __DyPluginImpl_H__
#define __DyPluginImpl_H__

class DyPluginImpl
{
public:
	DyPluginImpl();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
	DyPluginImpl(const DyPluginImpl& th);
	DyPluginImpl& operator=(const DyPluginImpl& th);
private:
	void* 			m_pHandle;
};

#endif	// DyPluginImpl.h
