#ifndef __DyPluginImp_H__
#define __DyPluginImp_H__

class DyPluginImp
{
public:
	DyPluginImp();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
	DyPluginImp(const DyPluginImp& th);
	DyPluginImp& operator=(const DyPluginImp& th);
private:
	void* 			m_pHandle;
};

#endif	// DyPluginImp.h
