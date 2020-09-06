#ifndef __KMInputImpl_Win32_H__
#define __KMInputImpl_Win32_H__

class KMInputImpl
{
    KMInputImpl();
    ~KMInputImpl();
    bool Init(const char* key=NULL,const char* mouse=NULL);
    //TODO:鼠标拖动事件
    //单独的鼠标事件
    //nbtn 0 左 1 右 2 中
    //mouse 鼠标值x,y、方向、偏移量
    //mStatus 0 弹起 1 按下
    void SingleMouseEvent(int nbtn,unsigned val,int mStatus);
    //单次鼠标按下弹起
    void MouseOnceDownUpEvent(int nbtn,unsigned val);
    //单独的键盘事件
    void SingleKeyEvent(unsigned MixKey,int kStatus);
    //单次键盘按下弹起
    void KeyOnceDownUpEvent(unsigned MixKey);
    //同一时间键盘鼠标事件1
    //kStatus -1 表示键盘按键一次完整的按下-弹起
    //mStatus -1 表示鼠标按键一次完整的按下-弹起
    void KeyMouseDownUpEvent(unsigned MixKey,int nbtn,unsigned mVal,int kStatus=-1,int mStatus=-1);
    //获取对应操作系统虚拟键值
    //type:0 键盘；1 鼠标
    int GetSystemKeyVal(int type,unsigned virVal);
    //获取对应操作系统虚拟键值
    //type:0 键盘；1 鼠标
    int GetSystemKeyVal(int type,unsigned virVal);
};

#endif  //KMInputImpl_Win32.h
