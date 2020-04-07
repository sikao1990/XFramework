#include "PackPerData.h"
#include <stdlib.h>
#include "SocketSession.h"

PackPerData::PackPerData(hSockFd socket, int nIndex, void* buf, int size, SocketSession* pSession)
	:m_socket(socket),m_index(nIndex), m_isFinish(0),m_pSession(pSession)
{  
	m_frame.m_buf = (char*)buf;
	m_frame.len = size;
	m_frame.m_index = nIndex;
	m_nPost = 0;
	//m_lock = 0;
	m_pDataCur = NULL;
	m_nExtendLen = 0;
	m_pExtendStart = NULL;
	m_pPrevPerData = NULL;
}

PackPerData::~PackPerData(void)
{

}

int PackPerData::RecvData(void* pdata,int len, int Index, int& nFlag, void* pPrev,void* pPostNode)
{
	MsgFrame tmp;
	tmp.m_buf = (char*)pdata;
	int originalLen = len;
	PackPerData*	pPrevPerData = reinterpret_cast<PackPerData*>(pPrev);
	printf("[sock:%d] RecvData ====== >start:%p,len:%d,pBufstart:%p,index:%d\n",m_socket,pdata,len,m_frame.m_buf,Index);
	int ret = 1;

	//1、判断上一个帧数据长度并合并数据
	int nPrevLen = 0;
	int nTotalCopy = 0;
	if (NULL != pPrevPerData) {
		if ((nPrevLen = pPrevPerData->HasExtendData()) > 0) {
			pPrevPerData->MoveExtendData();
			if (len > sizeof(Msg)) {
				if (nPrevLen < sizeof(Msg))
					memcpy(pPrevPerData->m_frame.m_buf + nPrevLen, m_frame.m_buf, sizeof(Msg) - nPrevLen);
				if (pPrevPerData->m_frame.m_pMsg->tFrames <= 0)
					throw "error recv";
				if (pPrevPerData->m_frame.m_pMsg->tSrcLen != pPrevPerData->m_frame.m_pMsg->tDataLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->nFrameLen)
					throw "error frame";
				if (pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->nFrameLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->tDataLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->tSrcLen)
					throw "recv error";
				nTotalCopy = pPrevPerData->m_frame.m_pMsg->nFrameLen - nPrevLen;
				if (nTotalCopy <= 0) {
					throw "logic error";//TODO:
				}
				printf("[sock:%d] totalCopy:%d\n",m_socket,nTotalCopy);
				if (len >= nTotalCopy) {
					memcpy(pPrevPerData->m_frame.m_buf + nPrevLen, m_frame.m_buf, nTotalCopy);
					pPrevPerData->m_nExtendLen += nTotalCopy;
					len -= nTotalCopy;
					for (int i = 0; i < len; i++)
						m_frame.m_buf[i]=m_frame.m_buf[nTotalCopy + i];
				}
				else {
					memcpy(pPrevPerData->m_frame.m_buf + nPrevLen, m_frame.m_buf, len);
					pPrevPerData->m_nExtendLen += len;
					len = 0;
				}
			}
			else {
				memcpy(pPrevPerData->m_frame.m_buf + nPrevLen, m_frame.m_buf, len);
				pPrevPerData->m_nExtendLen += len;
				len = 0;
			}
			if (pPrevPerData->m_nExtendLen == pPrevPerData->m_frame.m_pMsg->nFrameLen) {
				if (pPrevPerData->m_frame.m_pMsg->tSrcLen != pPrevPerData->m_frame.m_pMsg->tDataLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->nFrameLen)
					throw "error frame";
				if (pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->nFrameLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->tDataLen ||
					pPrevPerData->m_frame.m_pMsg->nDataLen > pPrevPerData->m_frame.m_pMsg->tSrcLen)
					throw "recv error";
				pPrevPerData->m_isFinish = true;
				pPrevPerData->m_nExtendLen = 0;
				nFlag = 1;//如果够一个包/帧
				printf("[sock:%d] complate rebuild the prev frame timestamp:%lld\n",m_socket,pPrevPerData->m_frame.m_pMsg->ntimestamp);
			}
		}
	}
	m_pDataCur = (char*)pdata + len;
	if (len <= 0) {
		ret = -3;
		goto FINISH;
	}
	if ((char*)m_pDataCur-m_frame.m_buf > sizeof(Msg)) {
		if (m_frame.m_pMsg->nFrameLen <= 0)
			throw "error frame";
		if (m_frame.m_pMsg->nFrameLen > m_frame.len) {
			m_pDataCur = m_frame.m_buf;
			m_isFinish = false;
			ret = -2;
			goto FINISH;
		}
		int rlen = m_frame.m_buf + m_frame.m_pMsg->nFrameLen - (char*)m_pDataCur;
		if (rlen <= 0) {
			m_pDataCur = m_frame.m_buf;
			m_isFinish = true;
			ret = 0;
			if(len > m_frame.m_pMsg->nFrameLen)
				m_nExtendLen = len - m_frame.m_pMsg->nFrameLen;
			MsgFrame Tmp;
			Tmp.m_buf= (m_frame.m_buf + m_frame.m_pMsg->nFrameLen);
			if (m_nExtendLen > PERIOPOSTSIZE || m_nExtendLen< 0)
				throw "error extend data";
			m_pExtendStart = m_frame.m_buf + m_frame.m_pMsg->nFrameLen;
			printf("[sock:%d] complate recv one frame,the extendlen:%d\n",m_socket,m_nExtendLen);
		}
		else if(rlen>0){
			if(m_pSession->PostRecvReq(m_socket,(char*)m_pDataCur,rlen,m_index,pPrev,pPostNode));
				ret = -1;
			ret = 2;
		}
	}
FINISH:
	return ret;
}

int PackPerData::RecordSendData( void* pdata, int len,void* pPostNode)
{
	if (!len)return -1;
	m_pDataCur = (char*)pdata + len;
	int total = m_frame.GetValidDataLen();
	//while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
	if ((char*)m_pDataCur <= m_frame.m_buf+total ) {	
		int wlen = m_frame.m_buf + total - (char*)m_pDataCur;//TCP客户端发送速度很快时，若一帧数量小于一次投递，则多收
		//printf("[sock:%d] total:%d,wlen:%d,m_pDataCur:%p\n", m_socket,total, wlen, m_pDataCur);
		if (0 == wlen) {
			m_pDataCur = m_frame.m_buf;
			m_frame.ResetValidDataLen(0);
			wlen = m_frame.len;
			m_isFinish = true;
		}
		else if(wlen>0){
			m_pSession->PostSendReq(m_socket,(char*)m_pDataCur,wlen,m_index,pPostNode);
		}
		//::InterlockedExchange(&m_lock, 0);
		return 0;
	}
	//::InterlockedExchange(&m_lock, 0);
	return 1;
}

void PackPerData::ResetData()
{
	m_frame.ResetValidDataLen(0);
	memset(m_frame.m_buf, 0, m_frame.len);
}

void PackPerData::PostRecv(void* pStart)
{
	//printf("index:%d\n", m_index);
	if (m_nPost > 2) {
		printf("New RecvPost was failed!\n");
		m_nPost = 0;
		return;
	}
	if (NULL == m_frame.m_buf) {
		printf("Init Frame was failed!\n");
		return;
	}
	m_pDataCur = m_frame.m_buf;

	m_nExtendLen = 0;
	m_nPost = 0;
	m_pPrevPerData = reinterpret_cast<PackPerData*>(pStart);
	if(!m_pSession->PostRecvReq(m_socket,m_frame.m_buf,m_frame.len,m_index,pStart))
		printf("PostRecv failed!\n");
}

void PackPerData::PostSend()
{
	if (m_nPost > 2) {
		printf("New SendPost was failed!\n");
		m_nPost = 0;
		return;
	}
	m_pDataCur = m_frame.m_buf;
	if (NULL == m_frame.m_buf) {
		printf("Init Frame was failed!\n");
		return;
	}
	int wlen = m_frame.GetValidDataLen();
	if (wlen>0) {
		m_pSession->PostSendReq(m_socket,(char*)m_pDataCur,wlen,m_index);
	}
}

bool PackPerData::IsComplate()
{ 
	bool bFlag = false;
	//while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
	bFlag = m_isFinish;
	//::InterlockedExchange(&m_lock, 0);
	return bFlag;
}

void PackPerData::SetNoticeStatus(bool f)
{ 
	//while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
	m_isFinish = f; 
	//::InterlockedExchange(&m_lock, 0);
}


void PackPerData::MoveExtendData()
{
	char* pArr =(char*) m_pExtendStart;
	if (NULL!=m_pExtendStart && m_nExtendLen > 0)
	{
		//printf("[sock:%d]index:%d the ExtendLen = %d\n",m_socket,m_index,m_nExtendLen);
		for (int i = 0; i < m_nExtendLen; i++)
			m_frame.m_buf[i] = pArr[i];
		m_pExtendStart = NULL;
	}
}

int PackPerData::GetPrevIndex()
{
	if (m_pPrevPerData)
		return m_pPrevPerData->m_index;
	else
		return -1;
}

const MsgFrame PackPerData::GetCurComplateFrame()
{ 
	MsgFrame tmpFrame;
	tmpFrame.m_buf = m_frame.m_buf;
	tmpFrame.len = m_frame.m_pMsg->nFrameLen;
	tmpFrame.m_index = m_index;
	return tmpFrame;
}

int PackPerData::GetOtherFrameCount(vector<MsgFrame>& vFrames)
{
	int count = 0;
	int extendLen = m_nExtendLen;
	int pOffset = 0;
	char* pStart = m_frame.m_buf + m_frame.m_pMsg->nFrameLen;
	while(extendLen > sizeof(Msg))
	{
		Msg* pFrame = (Msg*)(pStart + pOffset);
		int nLastLen = extendLen - sizeof(Msg);
		int nDataLen = pFrame->nDataLen;
		if (nLastLen >= nDataLen) {
			MsgFrame tmpFrame;
			tmpFrame.m_buf = (char*)pFrame;
			tmpFrame.len = nDataLen;
			tmpFrame.m_index = m_index;
			vFrames.push_back(tmpFrame);

			pOffset += sizeof(Msg) + nDataLen;
			extendLen = nLastLen - nDataLen;
			count++;
		}
		else
			break;	
	}
	m_nExtendLen = extendLen;
	m_pExtendStart = m_frame.m_buf + m_frame.m_pMsg->nFrameLen + pOffset;
	printf("[sock:%d] *****************index:%d extendlen:%d,start:%p,count:%d\n",m_socket,m_index,m_nExtendLen,m_pExtendStart, count);
	return count;
}

