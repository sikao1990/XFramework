#ifndef __RegItem_H__
#define __RegItem_H__
#include <list>
#include <iostream>
#include <windows.h>
#include <winreg.h>

using namespace std;

class RegItem
{
public:
	enum SysKey {
		SK_LocalMachine,
		SK_Users,
		SK_CurrConfig,
		SK_ClassesRoot,
		SK_CurrUser,
	};
    enum RegKeyType
    {
        NONE,
        UDWORD,
		QDWORD,
        STRING,
        BINARY,
        SUBKEY,
    };
	struct SubItem {
		string _name;
		string _date;
	};
    struct RegKey           {
        enum RegKeyType rType;
        union{
            unsigned _dVal;
            unsigned long long _xVal;
            char*    _cVal;
        }val;
        int          _dLen;
        string       _name;
        RegKey():rType(NONE),_dLen(0){
			val._xVal = 0;
		}
        ~RegKey(){
            if( _dLen > 0 ){
                delete [] val._cVal;
                _dLen = 0;
            }
        }
        RegKey(const RegKey& that){
            if(that._dLen>0){
                this->val._cVal = new char[ this->_dLen = that._dLen];
                memcpy(this->val._cVal,that.val._cVal,that._dLen);
            }else{
                this->val._xVal = that.val._xVal;
                this->_name = that._name;
            }
        }
        RegKey& operator=(const RegKey& that){
            if(this->_dLen>0){
                this->~RegKey();
            }
             if(that._dLen>0){
                this->val._cVal = new char[ this->_dLen = that._dLen];
                memcpy(this->val._cVal,that.val._cVal,that._dLen);
            }else{
                this->val._xVal = that.val._xVal;
                this->_name = that._name;
            }           
            return *this;
        }
    };
public:
	//lpSubKey example : "Software\\Microsfot\\Windows\\CurrentVersion\\Run"
    RegItem(RegItem::SysKey hKey,const char* lpSubKey);
    ~RegItem();
    bool OpenItem(bool bCreate=false);
    void CloseItem();
    void DeleteItem();
    void SetValue(const char* keyName,unsigned Val);
    void SetValue(const char* keyName,const char* strVal);
    void SetValue(const char* keyName,const char* pData,int len);
    void EraseValue(const char* keyName);
	RegItem::RegKey getKeyValue(const char* keyName);
    string GetItemInfos(list<RegItem::RegKey>& lst,list<RegItem::SubItem>& SubKeyLst);
	bool Save2Disk(const char* pFilePath);
	bool RestoreFromDisk(const char* pFilePath, bool bForce = false);
private:
    RegKeyType Type(unsigned type);
private:
    HKEY    m_hKey;
    HKEY    _hKey;
    string  _subKeystr;
    DWORD   m_type;
};

#endif  // RegItem.h
