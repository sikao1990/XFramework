#include "KMInputImpl_Win32.h"
#include <windows.h>
#include "KMDefine.h"

KMInputImpl::KMInputImpl()
{
    
}

KMInputImpl::~KMInputImpl()
{
    
}

bool KMInputImpl::Init(const char* key/*=NULL*/,const char* mouse/*=NULL*/)
{
    return true;
}

//单独的鼠标事件
void KMInputImpl::SingleMouseEvent(int nbtn,unsigned val,int mStatus)
{
    
}

//单次鼠标按下弹起
void KMInputImpl::MouseOnceDownUpEvent(int nbtn,unsigned val)
{
    
}

//单独的键盘事件
void KMInputImpl::SingleKeyEvent(unsigned MixKey,int kStatus)
{
    
}

//单次键盘按下弹起
void KMInputImpl::KeyOnceDownUpEvent(unsigned MixKey)
{
    
}

//同一时间键盘鼠标事件1
void KMInputImpl::KeyMouseDownUpEvent(unsigned MixKey,int nbtn,unsigned mVal,int kStatus/*=-1*/,int mStatus/*=-1*/)
{
    
}

int KMInputImpl::GetSystemKeyVal(int type,unsigned virVal)
{
    
}

