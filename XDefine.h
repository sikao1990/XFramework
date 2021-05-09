#ifndef __XDefine_H__
#define __XDefine_H__

//ʹ�� const �滻 ��

#define CONNTYPE_TCP		0
#define CONNTYPE_UDP		1

#define SESSION_SERVER		0
#define SESSION_CLIENT		1

#define HEARTBIT_INTERVAL		30					//�������

//�������ԣ�server��clientͬ�������
//������1�߳�ͬʱ����200���ͻ���988�ֽ�֡��ƽ��49 fps,CPUƽ��ռ��25%
//������1�߳�ͬʱ����500�ͻ���988�ֽڷ���,�ͻ���ÿ��ɴ�33
//������1�߳�ͬʱ����1000�ͻ���988�ֽڷ���,�ͻ���ÿ��ɴ�16,CPUƽ��35%
#define THREAD_COUNT			1					//���������紦���߳�����

#define PERIOPOSTSIZE			4096				//TCP->4k���->2k����

#define SEND_BUFFER_SIZE		(2*PERIOPOSTSIZE)	//���緢���ݻ���
#define RECV_BUFFER_SIZE		(4*PERIOPOSTSIZE)	//���������ݻ���

//TDP per Frame-max 548 Bytes
//TCP per Frame-max 1460 Bytes
#define TRANSFRAME				1024				//TCP->���紫�䵥֡��С Msg(24 bytes) + data(frame max data 1000 bytes )


//�����ĵײ�Э����
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
