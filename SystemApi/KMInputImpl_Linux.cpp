#include "KMInputImpl_Linux.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef __linux__
#include <linux/input.h>
#include <linux/uinput.h>
#endif
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "KMDefine.h"

KMInputImpl::KMInputImpl():m_FdMouse(-1),m_FdKey(-1)
{
    
}

KMInputImpl::~KMInputImpl()
{
    
}

bool KMInputImpl::Init(const char* key/*=NULL*/,const char* mouse/*=NULL*/)
{
    if(0==strlen(key)||0==strlen(mouse))return false;
    m_FdMouse = open(mouse,O_RDWR);
    if(m_FdMouse<=0)
        return false;
    m_FdKey = open(key,O_RDWR);
    if(m_FdKey<=0)
        return false;  
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
