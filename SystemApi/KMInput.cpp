#incldue "KMInput.h"
#ifdef WIN32
#include "KMInputImpl_Win32.h"
#else
#include "KMInputImpl_Linux.h"
#endif

KMInput::KMInput()
{
    _Impl = new KMInput();
}

KMInput::~KMInput()
{
    delete _Impl;
}

bool KMInput::Init(const char* key/*=NULL*/,const char* mouse/*=NULL*/)
{
    return _Impl->Init(key,mouse);
}

//单独的鼠标事件
void KMInput::SingleMouseEvent(int nbtn,unsigned val,int mStatus)
{
     return _Impl->SingleMouseEvent(nbtn,val,mStatus);
}

//单次鼠标按下弹起
void KMInput::MouseOnceDownUpEvent(int nbtn,unsigned val)
{
    return _Impl->MouseOnceDownUpEvent(nbtn,val);
}

//单独的键盘事件
void KMInput::SingleKeyEvent(unsigned MixKey,int kStatus)
{
    return _Impl->SingleKeyEvent(MixKey,kStatus);
}

//单次键盘按下弹起
void KMInput::KeyOnceDownUpEvent(unsigned MixKey)
{
    return _Impl->KeyOnceDownUpEvent(MixKey);
}

//同一时间键盘鼠标事件1
void KMInput::KeyMouseDownUpEvent(unsigned MixKey,int nbtn,unsigned mVal,int kStatus/*=-1*/,int mStatus/*=-1*/)
{
    return _Impl->KeyMouseDownUpEvent(MixKey,nbtn,mVal,kStatus,mStatus);
}

int KMInput::GetSystemKeyVal(int type,unsigned virVal)
{
    return _Impl->GetSystemKeyVal(type,virVal);
}
