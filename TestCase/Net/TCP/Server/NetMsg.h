#ifndef __NetMsg_H__
#define __NetMsg_H__

#include "../../Framework/XPublic.h"

#define SERVERPORT 5863

enum MsgType
{
	MT_NONE,
	MT_LOGON,
	MT_LOGOFF,
	//...
};

struct TestData
{
	char comBuf[996];
	char other[500];//500
};

struct NetMsg
{
	//MsgType		type;
	union 
	{
		//char	comBuf[992];
		char	comBuf[960];//the max for internet 996
		//TestData	tData;
		//logon
		//logoff
		//...
	}Data;
};

#define NETMSGSIZE	(sizeof(NetMsg))

#endif	// NetMsg.h
