#ifndef __KMDefine_H__
#define __KMDefine_H__

//Key Define Start

#define _KEY_1      0x1 //@
#define _KEY_2      0x2 //#
#define _KEY_3      0x3 //$
#define _KEY_4      0x4 //%
#define _KEY_5      0x5 //^
#define _KEY_6      0x6 //&
#define _KEY_7      0x7 //*
#define _KEY_8      0x8 //(
#define _KEY_9      0x9 //)
#define _KEY_0      0xa //!
#define _KEY_`      0xb //~
#define _KEY_-      0xc //_
#define _KEY_=      0xd //+
#define _KEY_[      0xe //{
#define _KEY_]      0xf //}
#define _KEY_\      0x10 //|
#define _KEY_;      0x11 //:
#define _KEY_'      0x12 //"
#define _KEY_,      0x13 //<
#define _KEY_.      0x14 //>
#define _KEY_/      0x15 //?

#define _KEY_a      0x20 //
#define _KEY_b      0x21 //
#define _KEY_c      0x22 //
#define _KEY_d      0x23 //
#define _KEY_e      0x24 //
#define _KEY_f      0x25 //
#define _KEY_g      0x26 //
#define _KEY_h      0x27 //
#define _KEY_i      0x28 //
#define _KEY_j      0x29 //
#define _KEY_k      0x2a //
#define _KEY_l      0x2b //
#define _KEY_m      0x2c //
#define _KEY_n      0x2d //
#define _KEY_o      0x2e //
#define _KEY_p      0x2f //
#define _KEY_q      0x30 //
#define _KEY_r      0x31 //
#define _KEY_s      0x32 //
#define _KEY_t      0x33 //
#define _KEY_u      0x34 //
#define _KEY_v      0x35 //
#define _KEY_w      0x36 //
#define _KEY_x      0x37 //
#define _KEY_y      0x38 //
#define _KEY_z      0x39 //

#define _KEY_ESC     0x3a //
#define _KEY_F1      0x3b //
#define _KEY_F2      0x3c //
#define _KEY_F3      0x3d //
#define _KEY_F4      0x3e //
#define _KEY_F5      0x3f //
#define _KEY_F6      0x40 //
#define _KEY_F7      0x41 //
#define _KEY_F8      0x42 //
#define _KEY_F9      0x43 //
#define _KEY_F10     0x44 //
#define _KEY_F11     0x45 //
#define _KEY_F12     0x46 //


#define _KEY_BACK       0x47 //退格
#define _KEY_TAB        0x48 //
#define _KEY_ENTER      0x49 //回车

#define _KEY_HOME       0x4a //
#define _KEY_END        0x4b //
#define _KEY_INSERT     0x4c //
#define _KEY_DELETE     0x4d //
#define _KEY_PAGEUP     0x4e //
#define _KEY_PAGEDOWN   0x4f //

#define _KEY_LEFT       0x50 //
#define _KEY_RIGHT      0x51 //
#define _KEY_UP         0x52 //
#define _KEY_DOWN       0x53 //
#define _KEY_SPACE      0x54 //空格键
#define _KEY_MENU       0x55 //

#define _KEY_CTRL       0x1 //
#define _KEY_ALT        0x2 //
#define _KEY_SHIFT      0x3 //
#define _KEY_CAPLOCK    0x4 //

#define _KEY_CMD        0x5 // win键

#define _MOUSE_LEFT     0x1 //
#define _MOUSE_RIGHT    0x2 //
#define _MOUSE_MID      0x3 // 滚轮

//Key Define End

//组合键
#define MAKEUP_KEY( ctrl1,ctrl2,key )   ( key|ctrl2<<8|ctrl1<<(8+16) )
//鼠标移动
#define MAKEUP_MOUSE_VAL( x,y )         ( x<<16|y )

//获取复合键 的控制键1
#define GETCTRL1( mixKey )              ( mixKey&0xf000 )
//获取复合键 的控制键2
#define GETCTRL2( mixKey )              ( mixKey&0x0f00 )
//获取复合键 键值
#define GETKEY( mixKey )                ( mixKey&0x00ff )

//获取鼠标键值 高位字
#define GETHIWORD( mVal )               ( mVal&0xff00 )
//获取鼠标键值 低位字
#define GETLOWORD( mVal )               ( mVal&0x00ff )

//组合键
struct KeyVal
{
    unsigned char           _ctrl1;
    unsigned char           _ctrl2;
    unsigned short char     _key;//normal or ctrl
};
//复合键值
union MixKey
{
    struct KeyVal   _mixVal;
    unsigned        _val;
};
//鼠标
struct mValue
{
    unsigned short  _x;
    unsigned short  _y;
};
//鼠标中键
struct mMid
{
    unsigned short  m_direct;   //向前、向后
    unsigned short  m_offset;   //滚轮滚动偏移
};
//复合鼠标值
union MixMouse
{
    struct mValue   _moveVal;
    struct mMid     _midVal;
    unsigned        _val;
};

#endif  // KMDefine.h
