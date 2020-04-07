#include "XObject.h"
#include <typeinfo>
#include <string.h>
#include <stdio.h>

map<XObject*,map<string,set<slotInfo> > >	XObject::m_objSets;

void XObject::XDisconnect(XObject* pSignal,const char* strSig,XObject* pSlot,const char* strSlot)
{
	map<XObject*,map<string,set<slotInfo> > >::iterator it = m_objSets.find(pSignal);
	if(m_objSets.end()!=it)
	{
		map<string,set<slotInfo> >& tagMap = it->second;
		set<slotInfo>& tagSet = tagMap.find(strSig)->second;
		if(tagSet.end()!=tagSet.find(slotInfo(strSlot,pSlot)) )
			tagSet.erase(slotInfo(strSlot,pSlot));
		if( 0==tagSet.size() ){
			tagMap.erase( tagMap.find(strSig) );
			m_objSets.erase( m_objSets.find(pSignal) );
		}
	}
}

bool XObject::XConnect(XObject* pSignal,const char* strSig,XObject* pSlot,const char* strSlot)
{
	if(m_objSets.end()!=m_objSets.find(pSignal))
	{
		map<string,set<slotInfo> >& tagSecond = m_objSets.find(pSignal)->second;
		if( tagSecond.end()!=tagSecond.find(strSig) ){
			set<slotInfo>& tagSet = tagSecond.find(strSig)->second;
			if(tagSet.empty())
				return false;
			tagSet.insert( slotInfo(strSlot, pSlot) );
		}else{
			set<slotInfo> tmpSet;
			tmpSet.insert( slotInfo(strSig,pSlot) );
			tagSecond.insert( make_pair(strSig,tmpSet) );
		}
		return true;
	}
	set<slotInfo> tmpInfo;
	tmpInfo.insert( slotInfo(strSlot,pSlot) );
	map<string,set<slotInfo> > tmpSet;
	tmpSet.insert(make_pair(strSig,tmpInfo));
	m_objSets.insert(make_pair(pSignal,tmpSet));
	return true;
}

void XObject::XInterface(XObject* pSigObj,const char* strSig,va_list apArg)
{
	XObject* 	pSlot = NULL;
	string 		strSlot;
	
	map<XObject*,map<string,set<slotInfo> > >::iterator it = m_objSets.find(pSigObj);
	if (it == m_objSets.end())
		return;
	map<string,set<slotInfo> >& tag = it->second;
	map<string, set<slotInfo> >::iterator itres = tag.find(strSig);
	if (itres == tag.end())
		return;
	set<slotInfo>& tagSet = itres->second;
	for(set<slotInfo>::iterator itr = tagSet.begin();itr!=tagSet.end();itr++)
	{
		va_list  ap;
		memcpy(&ap,apArg,sizeof(va_list));
		pSlot	= itr->slot;
		strSlot	= itr->strSlot;
		XMethod(pSlot,strSlot.c_str(),ap);
	}
}

void XObject::emit_signal(const char* strSig,...)
{
	va_list ap;
	va_start(ap,strSig);
	XInterface(this,strSig,ap);
	va_end(ap);
}

void XObject::XMethod(XObject* pObj,const char* strSlot,va_list ap)
{
	pObj->slot_Interface(strSlot,ap);
}

void XObject::slot_Interface(const char* strSlot,va_list ap)
{
	//TODO:
	puts("slot_Interface");
}


