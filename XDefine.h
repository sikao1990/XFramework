#ifndef __XDefine_H__
#define __XDefine_H__

//使用 const 替换 宏

#define CONNTYPE_TCP		0
#define CONNTYPE_UDP		1

#define SESSION_SERVER		0
#define SESSION_CLIENT		1

#define HEARTBIT_INTERVAL		30					//心跳间隔

//单机测试（server与client同物理机）
//服务器1线程同时接收200个客户端988字节帧率平均49 fps,CPU平均占用25%
//服务器1线程同时接受500客户端988字节发送,客户端每秒可达33
//服务器1线程同时接受1000客户端988字节发送,客户端每秒可达16,CPU平均35%
#define THREAD_COUNT			1					//服务器网络处理线程数量

#define PERIOPOSTSIZE			4096				//TCP->4k最佳->2k合适

#define SEND_BUFFER_SIZE		(2*PERIOPOSTSIZE)	//网络发数据缓冲
#define RECV_BUFFER_SIZE		(4*PERIOPOSTSIZE)	//网络收数据缓冲

//TDP per Frame-max 548 Bytes
//TCP per Frame-max 1460 Bytes
#define TRANSFRAME				1024				//TCP->网络传输单帧大小 Msg(24 bytes) + data(frame max data 1000 bytes )


//网络库的底层协议码
#define BASEPROTO_CONN			1
#define BASEPROTO_CLOSE			2
#define BASEPROTO_HEARTBIT		3


#ifdef WIN32

#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif

#include <windows.h>

typedef HANDLE  hHandle;

#else
	
#define FRAMEWORK_API
typedef int hHandle;

#endif

typedef void (*pProceeExitHandle)(void*);

#endif
