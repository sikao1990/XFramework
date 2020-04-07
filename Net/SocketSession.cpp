#include "SocketSession.h"
#include <stdio.h>
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>

#include "XNetCfg.h"
#include "../SystemApi/TimeInfo.h"

#ifdef WIN32
#include "PostNodeMgr_Win32.h"
#include "TcpServerImpl_Win32.h"
#include "TcpClientImpl_Win32.h"
#else
#include "TcpServerImpl_Linux.h"
#include "PostNodeMgr_Linux.h"
#include "TcpClientImpl_Linux.h"
#endif

SocketSession::SocketSession(): m_pHandle(NULL){
	m_wPostIndex = 0;
	m_rPostIndex = 0;
	m_finishW = 1;
	m_bOnline = 1;
	m_nWriteCount = 0;
	m_rErrorPack = 0;
	m_tmpSendBuf = new char[SEND_BUFFER_SIZE];
	m_tmpRecvBuf = new char[RECV_BUFFER_SIZE];
	m_nSessionFrom = 0;
	m_PostMgr = new PostNodeMgr;
}

void SocketSession::Init(const SockHandle& hSock, int type,void* pFrame)
{
	m_hSock = hSock;
	if (NULL != pFrame) {
		m_nSessionFrom = type;
		m_pXNet = (SockBase*)pFrame;
	}
	m_pHandle = new PackageProxy<SocketSession>;
	m_pHandle->Init(0, this);
	m_heartbitTime = time(0);
	InitFrames();
}

SocketSession::~SocketSession()
{
	m_bOnline = 0;
	m_pXNet->Close(m_hSock.m_sock);
	m_rlock.Lock();
	for (auto it = m_recvQueue.begin(); it != m_recvQueue.end(); it++) {
		void* p = (*it).GetData();
		if(NULL!=p)Free(p);
	}
	m_recvQueue.clear();
	m_rlock.UnLock();

	m_wlock.Lock();
	for (auto it=m_sendQueue.begin(); it != m_sendQueue.end(); it++)
		Free((void*)(*it).GetData());
	m_sendQueue.clear();
	m_wlock.UnLock();

	delete m_pHandle;
	delete m_PostMgr;
	delete[] m_tmpRecvBuf;
	delete[] m_tmpSendBuf;

	for (int i = 0; i < m_RecvBuf.size(); i++)
		delete m_RecvBuf[i];
	for (int i = 0; i < m_SendBuf.size(); i++)
		delete m_SendBuf[i];
}

int SocketSession::DispatchData(char* pBuf, int len,int nIndex,void* pPrev,void* pPostNode)
{
	if (nIndex<0 || nIndex > m_RecvBuf.size())
		throw "dispatchData failed error index";
	m_recvLock.Lock();
	if (NULL != pBuf)
		m_perIoDatas.push_back(TmpPerIO(pBuf, len, nIndex, pPrev,pPostNode));
	m_recvLock.UnLock();
	return 0;
}

void SocketSession::NoticeComplateSend(int index, void* pBuf, int len,void* pPostNode)
{
	m_sendLock.Lock();
	if (index < m_SendBuf.size()) {
		m_SendBuf[index]->RecordSendData(pBuf, len,pPostNode);
		if (m_SendBuf[index]->IsComplate()) {
			m_SendBuf[index]->SetNoticeStatus(false);
			if (m_wPostIndex + 1 == m_nWriteCount) {
				m_wPostIndex = 0;
				m_nWriteCount = 0;
				m_finishW.SetVal(1);
			}
			else
				m_SendBuf[++m_wPostIndex]->PostSend();
		}
	}
	m_sendLock.UnLock();
}

void SocketSession::PostSend()
{
	if (!m_bOnline)return;
	m_wlock.Lock();
	//1、判断是否传输完成
	if(m_finishW.IsVal(1) && m_sendQueue.size()){
		//2、拆包
		int nFrameCount = 0;
		vector<MsgFrame*> vecFrames;
		for (int i = 0; i < m_SendBuf.size(); i++) {
			m_SendBuf[i]->SetNoticeStatus();
			m_SendBuf[i]->ResetData();
			vecFrames.push_back(m_SendBuf[i]->GetFrame());
		}
		int ret = m_pHandle->Packaging(vecFrames, m_sendQueue, nFrameCount);
		if (ret > 0) {
			for (int i = 0; i < ret; i++) {
				Free(m_sendQueue.front().GetData());
				m_sendQueue.pop_front();
			}
			m_wPostIndex = 0;
			m_nWriteCount = nFrameCount;
			m_finishW.SetVal(0);
			m_SendBuf[m_wPostIndex]->PostSend();
		}
	}
	m_wlock.UnLock();
}

void SocketSession::ParseData()
{	
	//1、判断当前帧是否为最后一帧数据(判断该次数据传输是否完成)
	bool		bfinishR		= false;
	bool		bFinshPackage	= false;
	int			len				= 0;
	int			ret = -3;
	bool		bComplateOne = false;
	bool		bFlag = false;
	int			nPrevFlag = 0;
	FrameKey	nCurTimestamp = -1;
	m_recvLock.Lock();

	while (!m_perIoDatas.empty()) 
	{
		ret = -3;
		bComplateOne = false;
		bFlag = false;
		nPrevFlag = 0;
		nCurTimestamp = -1;
		TmpPerIO tmpIO = m_perIoDatas.front();
		int nCurIndex = tmpIO.nIndex;
		len		= tmpIO.len;
		if (nCurIndex < m_RecvBuf.size())
		{
			ret = m_RecvBuf[nCurIndex]->RecvData(tmpIO.pBuf,len, tmpIO.nIndex, nPrevFlag,tmpIO.pPrevIOData,tmpIO.pPostNode);
			if (nPrevFlag) //当完成接收当前缓冲请求数据，且将上一帧未收完数据合并完成
			{
				int nIndex = -1;
				long long nTimeStamp = 0;
				nIndex = m_RecvBuf[nCurIndex]->GetPrevIndex();
				if (-1 != nIndex) {
					MsgFrame tmpFrame = m_RecvBuf[nIndex]->GetCurComplateFrame();
					map<FrameKey, vector<MsgFrame> >::iterator it = m_mapFrames.find(tmpFrame.GetTimeStamp());
					if (it != m_mapFrames.end()) {
						it->second.push_back(tmpFrame);
					}
					else {
						vector<MsgFrame> vTmpFrames;
						vTmpFrames.push_back(tmpFrame);
						m_mapFrames.insert(make_pair(tmpFrame.GetTimeStamp(), vTmpFrames));
					}
					nCurTimestamp = tmpFrame.GetTimeStamp();
					bComplateOne = true;
				}
				if (ret > 0)bFlag = true;
			}
			if (ret >= 0)
			{
				if (m_RecvBuf[nCurIndex]->IsComplate()) {
					//如果在当前投递缓冲中存在已完成其他若干帧接收，且或从属于当前包或从属与后一个包等
					m_RecvBuf[nCurIndex]->SetNoticeStatus(false);
					vector<MsgFrame> vOtherFrames;
					vOtherFrames.push_back(m_RecvBuf[nCurIndex]->GetCurComplateFrame());
					int nOther = m_RecvBuf[nCurIndex]->GetOtherFrameCount(vOtherFrames);
					for (int i = 0; i < vOtherFrames.size(); i++) {
						if (m_mapFrames.end() == m_mapFrames.find(vOtherFrames[i].GetTimeStamp())) {
							vector<MsgFrame> vTmpF;
							vTmpF.push_back(vOtherFrames[i]);
							m_mapFrames.insert(make_pair(vOtherFrames[i].GetTimeStamp(), vTmpF));
						}
						else 
							m_mapFrames.find(vOtherFrames[i].GetTimeStamp())->second.push_back(vOtherFrames[i]);
					}
					nCurTimestamp = vOtherFrames[vOtherFrames.size() - 1].GetTimeStamp();
					bComplateOne = true;
				}
			}
			if (bComplateOne)
			{
				//如果存在完成接收一个包
				if (bFlag) {//最后一个完整帧是否为整包的最后一帧
					bFinshPackage = false;
				}
				else {
					map<FrameKey, vector<MsgFrame> >::iterator itCur = m_mapFrames.find(nCurTimestamp);
					if (itCur != m_mapFrames.end() && itCur->second.size() < itCur->second[0].GetTotalFrames()) {
						void* pExtendStart = NULL;
						if (m_RecvBuf[nCurIndex]->HasExtendData())
							pExtendStart = m_RecvBuf[nCurIndex];
						bFinshPackage = false;
						sort(itCur->second.begin(), itCur->second.end());
						if (m_rPostIndex + 1 == m_RecvBuf.size()) {
							int nIndex = -1;
							int iCount = 0;
							nIndex = itCur->second.front().GetIndex();
							if (nIndex > 0) {
								int iCount = nCurIndex - nIndex;
								for (int i = iCount; i > 0; i--) {
									m_RecvBuf.push_back(m_RecvBuf.back());
									m_RecvBuf.pop_back();
								}
								for (int i = 0; i < m_RecvBuf.size(); i++)
									m_RecvBuf[i]->ResetIndex(i);
								m_rPostIndex = iCount+1;
							}
						}
						else
							m_rPostIndex++;

						m_RecvBuf[m_rPostIndex]->PostRecv(pExtendStart);
					}
					else
						bFinshPackage = true, m_rPostIndex = 0;
				}
				map<FrameKey, vector<MsgFrame> >::iterator it = m_mapFrames.begin();
				for (; it != m_mapFrames.end(); it++)
				{
					for (int i = 0; i < it->second.size(); i++)
					{
						if (it->first != it->second[i].GetTimeStamp())
							throw "error";
					}
					if (it->second.size() == it->second[0].GetTotalFrames()) {
						bfinishR = true;
						break;
					}
				}
			}
		}
		printf("[sock:%d] parse ========> ret:%d,bfinishR=%d,bFinshPackage:%d,bComplateOne:%d,bFlag:%d\n",m_hSock.m_sock,ret,bfinishR,bFinshPackage,bComplateOne,bFlag);
		if (bfinishR) {
			//2、数据处理(解压缩、解码)	
			vector< vector<MsgFrame> > vecFrames;
			map<FrameKey, vector<MsgFrame> >::iterator it = m_mapFrames.begin();
			map<FrameKey, vector<MsgFrame> >::iterator itFlag = m_mapFrames.end();
			for (; it != m_mapFrames.end(); it++)
			{
				if (it->second[0].GetTotalFrames() == it->second.size())
				{
					vecFrames.push_back(it->second);
					for (int i = 0; i < it->second.size(); i++) {
						if (it->second[i].GetTimeStamp() != it->first)
							throw "xxx";
					}
					if (itFlag == m_mapFrames.end())itFlag = it;
				}
			}
#if 0
			//清除不完整的包的帧数据并记录统计数量
			if (m_mapFrames.size() > 1 && itFlag != m_mapFrames.end() && itFlag != m_mapFrames.begin()) {
				map<long long, vector<MsgFrame> >::iterator itr = m_mapFrames.begin();
				list<long long> tmpKey;
				for (; itr != itFlag; itr++) {
					tmpKey.push_back(itr->first);
					m_rErrorPack++;
				}
				list<long long>::iterator itp = tmpKey.begin();
				for (; itp != tmpKey.end(); itp++) {
					m_mapFrames.find(*itp)->second.clear();
					m_mapFrames.erase(*itp);
				}
			}
#endif
			//2.1 查找第一个多收数据的帧索引
			int nIndex = -1;
			for (int i = 0; i < m_RecvBuf.size(); i++) {
				if (m_RecvBuf[i]->HasExtendData()) {
					nIndex = i;
					break;
				}
			}

			int nLastIndex = -1;
			if (nIndex + 1 == m_RecvBuf.size()) {
				FrameKey lastTimeStamp = vecFrames[vecFrames.size() - 1][0].GetTimeStamp();
				map<FrameKey, vector<MsgFrame> >::iterator lastItr = m_mapFrames.find(lastTimeStamp);
				map<FrameKey, vector<MsgFrame> >::iterator destItr = m_mapFrames.end();
				if (lastItr != m_mapFrames.end()) {
					destItr = ++lastItr;
					if (destItr != m_mapFrames.end()) {
						if (destItr->second.size() > 1)sort(destItr->second.begin(), destItr->second.end());
						nLastIndex = destItr->second.front().GetIndex();
					}
					else {
						destItr = --lastItr;
						if (destItr->second.size() > 1)sort(destItr->second.begin(), destItr->second.end());
						nLastIndex = destItr->second.back().GetIndex();
					}
				}
				else
					throw "unknow error";
			}

			for (int i = 0; i < vecFrames.size(); i++)
			{
				map<FrameKey, vector<MsgFrame> >::iterator itr = m_mapFrames.find(vecFrames[i][0].GetTimeStamp());
				if (itr == m_mapFrames.end())
					throw "unknow error";
				itr->second.clear();
				m_mapFrames.erase(itr);
			}

			int len = 0;
			if (vecFrames.size())
			{
				char* pTmp = NULL;
				int totalLen = 0;
				for (int i = 0; i < vecFrames.size(); i++)
				{
					if (vecFrames[i].size())
					{
						pTmp = NULL;						
						totalLen = vecFrames[i][0].GetDataLen();
						if (totalLen > 0) 
						{
							pTmp = (char*)Alloc(vecFrames[i][0].GetSrcDataLen());
							if (m_pHandle->SetPackageInfo(pTmp, totalLen, vecFrames[i][0].m_pMsg->tSrcLen) &&
								totalLen == m_pHandle->setMsgFrames(vecFrames[i], vecFrames[i].size()) && m_pHandle->process())
							{
								void* pData = NULL;
								pData = m_pHandle->getData(len);
								//3、加入队列 
								if (NULL != pData) {
									//判断是否是底层子协议
									if (m_pHandle->IsBaseProtocolPackage(vecFrames[i][0].GetBaseProto())) {
										if (!ParseBaseProtocol(vecFrames[i][0].GetBaseProto(), pData, vecFrames[i][0].GetSrcDataLen()))
										{
											m_bOnline = 0;
											goto KICK_END;
										}
									}
									else {
										m_rlock.Lock();
										m_recvQueue.push_back(NetPackage(pData, vecFrames[i][0].GetSrcDataLen()));
										m_rlock.UnLock();
									}
								}
								else
									puts("new NetPackage failed!");
							}
						}
						else {
							if (m_pHandle->IsBaseProtocolPackage(vecFrames[i][0].GetBaseProto()))
							{
								if (!ParseBaseProtocol(vecFrames[i][0].GetBaseProto(), NULL, 0))
								{
									m_bOnline = 0;
									goto KICK_END;
								}
							}
						}
					}
				}
				//4、清空标识
				//5、投递读请求,a、指定使用哪个用来投递，b、上一个帧缓冲存储起始地址，c、数据起始地址，d、数据长度
				void* pStart = NULL;
				if (-1 != nIndex) 
				{
					pStart = m_RecvBuf[nIndex];
					if (nIndex + 1 == m_RecvBuf.size()) {
						if (nLastIndex > 0) {
							nIndex -= nLastIndex;
							int nCount = nIndex + 1;
							for (int i = nCount; i > 0; i--) {
								m_RecvBuf.push_back(m_RecvBuf.back());
								m_RecvBuf.pop_back();
							}
							for (int i = 0; i < m_RecvBuf.size(); i++)
								m_RecvBuf[i]->ResetIndex(i);
							nIndex += 1;
						}
						else
							throw "need new more memory";
					}
					else
						nIndex += 1;
				}
				else
					nIndex = 0;
				if (bFinshPackage)ResetRecvFlagInfo(nIndex), PostRecv(m_rPostIndex = nIndex, pStart), bFinshPackage = false;
				bfinishR = false;
			}
			else
				printf("%d deal data error\n", vecFrames.size());
		}
		m_perIoDatas.pop_front();
	}

KICK_END:
	m_recvLock.UnLock();
}


void SocketSession::PostRecv(int index, void* pStart)
{
	if (!m_bOnline)return;
	if (index < 0 || index >= m_RecvBuf.size())
		throw "invalid index";
	printf("[sock:%d]--->The Post Index:%d\n",m_hSock.m_sock,index);
	m_RecvBuf[index]->PostRecv(pStart);
}

int SocketSession::RecvNetPackage(NetPackage& p)
{	
	int ret = -1;
	m_rlock.Lock();
	if(m_recvQueue.size()){
		p = m_recvQueue.front();
		m_recvQueue.pop_front();
		ret = 0;
	}	
	m_rlock.UnLock();
	return ret;
}
int SocketSession::SendNetPackage(const NetPackage& p)	
{	
	if (!m_bOnline)return 0;
	m_wlock.Lock();
	m_sendQueue.push_back(p);
	m_wlock.UnLock();
	return 0;
}

void SocketSession::InitFrames()
{
	int perFrame = PERIOPOSTSIZE;
	int frameCount = RECV_BUFFER_SIZE/ PERIOPOSTSIZE;
	int lastFrame = RECV_BUFFER_SIZE - frameCount*PERIOPOSTSIZE;
	for (int i = 0; i < frameCount; i++) {
		PackPerData* PerData =new PackPerData(m_hSock.m_sock,i,m_tmpRecvBuf+i*perFrame, perFrame,this);
		m_RecvBuf.push_back(PerData);
	}
	if (lastFrame > 0) {
		PackPerData* PerData=new PackPerData(m_hSock.m_sock, frameCount, m_tmpRecvBuf + frameCount*perFrame, lastFrame, this);
		m_RecvBuf.push_back(PerData);
	}

	frameCount = SEND_BUFFER_SIZE / PERIOPOSTSIZE;
	lastFrame = SEND_BUFFER_SIZE - frameCount*PERIOPOSTSIZE;
	for (int i = 0; i < frameCount; i++) {
		PackPerData* PerData= new PackPerData(m_hSock.m_sock, i, m_tmpSendBuf + i*perFrame, perFrame, this);
		m_SendBuf.push_back(PerData);
	}
	if (lastFrame > 0) {
		PackPerData* PerData=new PackPerData(m_hSock.m_sock, frameCount, m_tmpSendBuf + frameCount*perFrame, lastFrame, this);
		m_SendBuf.push_back(PerData);
	}
}

void SocketSession::ResetRecvFlagInfo(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_RecvBuf.size()) {
		for (int i = 0; i < nIndex; i++)
			m_RecvBuf[i]->RestRecvFlag();
	}
}

bool SocketSession::PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode )
{
	if(NULL==m_pXNet)return false;
	if(NULL==pPostNode&&NULL!=m_PostMgr)
		pPostNode = m_PostMgr->ApplyPostNode();
	return m_pXNet->PostRecvReq(fd,pStart,len,nIndex,pExtend,pPostNode);
}

bool SocketSession::PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode)
{
	if(NULL==m_pXNet)return false;
	if(NULL==pPostNode&&NULL!=m_PostMgr)
		pPostNode = m_PostMgr->ApplyPostNode();
	return m_pXNet->PostSendReq(fd,pStart,len,nIndex,pPostNode);
}

bool SocketSession::VerifyBaseConn(void* pDataStart, int len)
{
	//派生类中实现
	//从数据库连接池中获取一个连接
	//查询玩家信息
	//失败 发送通知 SetSessionState(false);
	//成功 SetSessionState(true);
	return true;
}

bool SocketSession::ParseBaseProtocol(int nProtoType, void* pData, int len)
{
	switch (nProtoType)
	{
	case 1://获得由经简单格式化、加密的客户端设备基本信息.或验证是否能够ping通或合法IP
		VerifyBaseConn(pData, len);
		break;
	case 2://心跳
		updateLastTime();
		break;
	}
	return true;
}

void SocketSession::NoticeConnFailed()
{
	NetPackage msg;
	msg.m_timestamp = TimeInfo::getTimeStamp();
	msg.SetBaseProtol(1);
	msg.SetPackageState(1);//用宏定义
	SendNetPackage(msg);//TODO:检查长度为0
}