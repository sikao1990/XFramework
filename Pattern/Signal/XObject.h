#ifndef __XObject_H__
#define __XObject_H__
#include <map>
#include <set>
#include <iostream>
#include <stdarg.h>

using namespace std;

//该模式解决对象间相互调用，且不用传递对象（非耦合调用-类似Qt信号和槽机制）
//缺点:参数不支持自定义数据类型包括(std::string)，但可以通过传递指针方式弥补

class XObject;
struct slotInfo{
	string		strSlot;
	XObject*	slot;
	slotInfo(const char* str="",XObject* pobj=NULL):strSlot(str),slot(pobj){}
	bool operator<(const slotInfo& d)const{
		return slot < d.slot;
	}
};
class XObject	//解决对象间相互调用->必须继承自XObject类
{
public:
	virtual ~XObject(){}
	virtual void slot_Interface(const char* strSlot,va_list ap);
	void emit_signal(const char* str,...);
public:
	static bool XConnect(XObject* pSignal,const char* strSig,XObject* pSlot,const char* strSlot);
	static void XDisconnect(XObject* pSignal,const char* strSig,XObject* pSlot,const char* strSlot);	
private:
	void XInterface(XObject* pSigObj,const char* strSig,va_list apArg);
	static void XMethod(XObject* pObj,const char* strSlot,va_list ap);
private:
	static map<XObject*,map<string,set<slotInfo> > >	m_objSets;
};

#endif // XObject.h
