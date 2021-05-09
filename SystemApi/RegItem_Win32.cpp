#include "RegItem_Win32.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

HKEY sysHKey[] = {
	HKEY_LOCAL_MACHINE,
	HKEY_USERS,
	HKEY_CURRENT_CONFIG,
	HKEY_CLASSES_ROOT,
	HKEY_CURRENT_USER,
};

RegItem::RegItem(RegItem::SysKey hKey,const char* lpSubKey):
    m_hKey(NULL),_hKey(sysHKey[hKey] ),_subKeystr(lpSubKey),m_type(-1)
{
    
}

RegItem::~RegItem()
{
    CloseItem();
}

bool RegItem::OpenItem(bool bCreate)
{
    DWORD lRet  = 0;
    DWORD state = 0;
    if(bCreate)
    {
        lRet = RegCreateKeyEx(_hKey,_subKeystr.c_str(),0,NULL,0,0,NULL,&m_hKey,&state);
        if(lRet == ERROR_SUCCESS)
        {
            if(REG_CREATED_NEW_KEY == state )
                return true;
            else if(REG_OPENED_EXISTING_KEY == state )
                return false;
        }
    }else{
        lRet = RegOpenKeyEx(_hKey, _subKeystr.c_str(), 0, KEY_ALL_ACCESS, &m_hKey);
        if(lRet == ERROR_SUCCESS)
        {
            return true;
        }
    }
    return false;
}

void RegItem::CloseItem()
{
    if(NULL != m_hKey)
    {
        RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
}

void RegItem::DeleteItem()
{
    RegDeleteKey(_hKey,_subKeystr.c_str());
}

void RegItem::SetValue(const char* keyName,unsigned Val)
{
    if(NULL != m_hKey )
        RegSetValueEx(m_hKey,keyName,0,REG_DWORD,(const BYTE*)&Val,sizeof(Val));
}

void RegItem::SetValue(const char* keyName,const char* strVal)
{
    if(NULL != m_hKey )
        RegSetValueEx(m_hKey,keyName,0,REG_SZ,(BYTE *)strVal,strlen(strVal)+1);
}

void RegItem::SetValue(const char* keyName,const char* pData,int len)
{
    if(NULL != m_hKey )
        RegSetValueEx(m_hKey,keyName,0,REG_BINARY,(BYTE *)pData,len);
}

void RegItem::EraseValue(const char* keyName)
{
    if(NULL != m_hKey )
        RegDeleteValue(m_hKey,keyName);
}

RegItem::RegKeyType RegItem::Type(unsigned type)
{
    switch(type){
    case REG_DWORD:
        return UDWORD;
	case REG_QWORD:
		return QDWORD;
    case REG_SZ:
    case REG_MULTI_SZ:
    case REG_EXPAND_SZ:
        return STRING;
    case REG_BINARY:
        return BINARY;
    }
    return NONE;
}

RegItem::RegKey RegItem::getKeyValue(const char* keyName)
{
    DWORD lRet = 0;
    DWORD dwSize = 0;
    lRet = RegQueryValueEx(m_hKey,keyName,0,&m_type,NULL,&dwSize);
    RegKey val;
    val._name = keyName;
    val.rType = Type(m_type);
    if(ERROR_SUCCESS == lRet)
    {
        switch(m_type){
        case REG_DWORD:
            lRet = RegQueryValueEx(m_hKey,keyName,0,&m_type, (LPBYTE)&(val.val._dVal),&dwSize);
            break;
		case REG_QWORD:
			lRet = RegQueryValueEx(m_hKey, keyName, 0, &m_type, (LPBYTE)&(val.val._xVal), &dwSize);
			break;
        case REG_SZ:
        case REG_MULTI_SZ:
        case REG_EXPAND_SZ:
            val._dLen = dwSize;
            val.val._cVal = new char[ dwSize+1 ];
            lRet = RegQueryValueEx(m_hKey,keyName,0,&m_type, (LPBYTE)val.val._cVal,&dwSize);
            break;
        case REG_BINARY:
            val._dLen = dwSize;
            val.val._cVal = new char[ dwSize ];
            lRet = RegQueryValueEx(m_hKey,keyName,0,&m_type, (LPBYTE)val.val._cVal,&dwSize);
            break;
        }
    }
    return val;
}

string RegItem::GetItemInfos(list<RegKey>& lst, list<RegItem::SubItem>& SubKeyLst)
{
	char	 achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	char  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	char achData[MAX_VALUE_NAME];
	DWORD cchData = MAX_VALUE_NAME;
	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		m_hKey,                    // key handle 
		NULL,                    // buffer for class name 
		NULL,                    // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // 子键数量
		&cbMaxSubKey,            // 子键名称长度 
		NULL,                    // longest class string 
		&cValues,                // 键值数量 
		&cchMaxValue,            // 键值名称最大长度 
		&cbMaxValueData,         // 键值数据最大长度 
		NULL,                    // security descriptor 
		&ftLastWriteTime);       // last write time 

								 // Enumerate the subkeys, until RegEnumKeyEx fails.
	SYSTEMTIME ut;
	char szbuf[32] = {};
	FileTimeToSystemTime(&ftLastWriteTime, &ut);
	sprintf(szbuf, "%04d-%02d-%02d %02d:%02d:%02d", ut.wYear, ut.wMonth, ut.wDay, ut.wHour + 8, ut.wMinute, ut.wSecond);
	string curItemDate = szbuf;

	if (cSubKeys)
	{
		printf("\nNumber of subkeys: %d\n", cSubKeys);

		for (i = 0; i<cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(m_hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				FileTimeToSystemTime(&ftLastWriteTime, &ut);
				sprintf(szbuf,"%d-%d-%d %d:%d:%d\n",
					ut.wYear, ut.wMonth, ut.wDay, ut.wHour + 8, ut.wMinute, ut.wSecond);
				SubItem subitem;
				subitem._name = achKey;
				subitem._date = szbuf;
				SubKeyLst.push_back(subitem);
			}
		}
	}
	if (SubKeyLst.size() != cSubKeys)return "";

	// Enumerate the key values. 

	if (cValues)
	{
		printf("\nNumber of values: %d\n", cValues);
		DWORD ktype;
		for (i = 0, retCode = ERROR_SUCCESS; i<cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			memset(achValue, 0, sizeof(achValue));

			cchData = MAX_VALUE_NAME;
			memset(achData, 0, sizeof(achData));
			retCode = RegEnumValue(m_hKey, i,
				achValue,
				&cchValue,
				NULL,
				&ktype,//type
				(PBYTE)achData,//PBYTE
				&cchData);//BYTELEN

			if (retCode == ERROR_SUCCESS)
			{
				RegKey kVal;
				kVal._name = achValue;
				switch (ktype)
				{
				case REG_DWORD:
					kVal.rType = UDWORD;
					kVal.val._dVal = *((unsigned*)achData);
					break;
				case REG_QWORD:
					kVal.rType = QDWORD;
					kVal.val._xVal = *((unsigned long long*)achData);
					break;
				case REG_SZ:
				case REG_MULTI_SZ:
				case REG_EXPAND_SZ:
					kVal.rType = STRING;
					strcpy(kVal.val._cVal = new char[kVal._dLen = cchData + 1], achData);
					break;
				case REG_BINARY:
					kVal.rType = BINARY;
					memcpy(kVal.val._cVal = new char[kVal._dLen = cchData], achData,cchData);
					break;
				}
				lst.push_back(kVal);
			}
		}
	}
	if (lst.size() != cValues)return "";
    return curItemDate;
}

bool RegItem::Save2Disk(const char* pFilePath)
{
	if (NULL == m_hKey || NULL == pFilePath || !strlen(pFilePath))return false;
	if (ERROR_SUCCESS != RegSaveKey(m_hKey, pFilePath,NULL) )
		return false;
	return true;
}

bool RegItem::RestoreFromDisk(const char* pFilePath, bool bForce )
{
	if (NULL == m_hKey || NULL == pFilePath || !strlen(pFilePath))return false;
	if (ERROR_SUCCESS != RegRestoreKey(m_hKey, pFilePath, bForce ? REG_FORCE_RESTORE : REG_WHOLE_HIVE_VOLATILE))
		return false;
	return true;
}
