#ifndef __SocketSession_H__
#define __SocketSession_H__

#include <map>
#include <time.h>
#include "XNetCfg.h"
#include "PackPerData.h"
#include "../Thread/AtomMutex.h"
#include "../Thread/MutexInteger.h"
#include "SockBase.h"

using namespace std;

class PostNodeMgr;
class FRAMEWORK_API SocketSession
{
	struct TMPBUF
	{
		int		index;
		int		len;
		char*	buf;
		TMPBUF(int index = -1, void* buf = NULL, int len = 0) :index(index), len(len), buf((char*)buf) {}
	};
	struct TmpPerIO
	{
		int			nIndex;
		int			len;
		char*		pBuf;
		void*		pPrevIOData;
		void* 		pPostNode;
		TmpPerIO(char* p = NULL, int len = 0,int index=-1,void* pPrev=NULL,void* pNode=NULL) :pBuf(p), 
			len(len),nIndex(index),pPrevIOData(pPrev),pPostNode(pNode){ }
	};
	friend class PackPerData;
public:
	SocketSession();
	virtual ~SocketSession();
	//param type: 0 来源于server-BalanceHandle 、 1 来源于client
	void Init(const SockHandle& hSock, int type, void* pFrame = NULL);
	int DispatchData(char* pBuf, int len, int nIndex, void* pPrev,void* pPostNode = NULL);
	void NoticeComplateSend(int index, void* pBuf, int len,void* pPostNode = NULL);
	void PostSend();
	void PostRecv(int index = 0, void* pStart = NULL);//客户端联入是的第一次投递
	void ParseData();
	virtual void* Alloc(int len) { return NULL; }
	virtual void Free(void* p) {}

	virtual int code(const char* src, int srclen, int len = 0) { return len; }
	virtual int compress(const char* src, int srclen, int len = 0) { return len; }
	virtual void* getData(int& len) { return NULL; }
	virtual bool SetPackageInfo(void* buf, int len, int srcLen) { return true; }
	virtual bool encode() { return true; }
	virtual int uncompress() { return 0; }
	virtual bool ParseBaseProtocol(int nProtoType, void* pData, int len);
	//客户端连接服务器的基本验证(TCP客户端连接校验不是必须实现；UDP最好实现,否则服务器收到第一条数据后，视为连接成功)
	virtual bool VerifyBaseConn(void* pDataStart, int len);

	int RecvNetPackage(NetPackage& p);
	int SendNetPackage(const NetPackage& p);		
	time_t getLastTime(){ return m_heartbitTime; }
	void updateLastTime(){ m_heartbitTime=time(0); }
	hSockFd getSock()const { return m_hSock.m_sock; }
	int	getConnType()const { return m_hSock.m_type; }
	string getClientIP()const { return m_hSock.m_ip; }
	int getPort()const { return m_hSock.m_port; }
	int GetRecvPackageCount() { return m_recvQueue.size(); }
	void SetSessionState(bool bState) { m_bOnline = bState; }
	bool GetSessionState() { return m_bOnline; }
	SockHandle GetSockHandle() { return m_hSock; }
	void NoticeConnFailed();
private:
	void InitFrames();
	void ResetRecvFlagInfo(int nIndex);
	bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend = NULL,void* pPostNode = NULL);
	bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex, void* pPostNode = NULL);
private:
	unsigned				m_bOnline		: 2,
							m_nSessionFrom	: 2,
							m_nWriteCount	: 4,
							m_wPostIndex	: 4,
							m_rPostIndex	: 4,						
							m_rErrorPack	: 16;
	time_t					m_heartbitTime;
	AtomMutex				m_recvLock;
	AtomMutex				m_sendLock;	
	MutexInterger			m_finishW;	
	SockBase*				m_pXNet;
	char*					m_tmpSendBuf;
	char*					m_tmpRecvBuf;
	DataHandle*				m_pHandle;
	list<TmpPerIO>			m_perIoDatas;
	map<FrameKey, vector<MsgFrame> >	m_mapFrames;
	SockHandle				m_hSock;
	vector<PackPerData*>	m_RecvBuf;
	vector<PackPerData*>	m_SendBuf;

	deque<NetPackage>		m_recvQueue;
	deque<NetPackage> 		m_sendQueue;

	AtomMutex				m_rlock;
	AtomMutex				m_wlock;
	PostNodeMgr*			m_PostMgr;
private:
	SocketSession(const SocketSession&) {}
	SocketSession& operator=(const SocketSession&) { return *this; }
};

#endif	//	socketSession.h