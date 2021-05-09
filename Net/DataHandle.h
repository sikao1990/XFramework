#ifndef __DataHandle_H__
#define __DataHandle_H__
#include <stdarg.h>
#include <vector>
#include <time.h>
#include <assert.h>
#include "XNetCfg.h"
#include "NetPackage.h"
#include <deque>
#include <list>
#include <stddef.h>

using namespace std;

typedef long long FrameKey;

struct TmpBuf
{
	TmpBuf(char*buf=NULL,int len=0):m_buf(buf),m_len(len){}
	char* m_buf;
	int m_len;
};

struct MsgHead
{
	unsigned char	tFrames;	//总帧数
	unsigned char	nFrames;	//当前帧编号
	unsigned char	nVerifyLen;	//校验区长度(数据包md5值保证一致性，TCP不用)
	unsigned char	nProtoType;	//子协议->底层用途（非用于逻辑层）0:非底层协议、1 连接、2 断开、3 心跳
	unsigned char	nReserved;	//保留
	unsigned char	nRetState;	//帧返回值	返回: 0 成功 、非0 错误码,发送时不填
	unsigned short 	nDataLen;	//数据区长度
	unsigned short	tSrcLen;	//原始总数据长度
	unsigned short	tDataLen;	//总传输长度
	unsigned short	nFrameLen;	//当前帧数据长度
	unsigned short	nUserProto;	//应用层协议
	long long		ntimestamp;	//毫秒级时间戳
	MsgHead() { memset(this, 0, sizeof(*this)); }
};
struct Msg : public MsgHead//HeadInfo total: 24 Bytes
{
	unsigned char	nData[0];	//校验区(TCP不用)+数据区
};

struct MsgFrame//单包
{
	void SetTotalFrames(int n) { m_pMsg->tFrames = n; }
	void SetCurFrameNo(int n) { m_pMsg->nFrames = n; }
	void SetSrcDataLen(int len) { m_pMsg->tSrcLen = len; }
	void SetCurDataLen(int len) { m_pMsg->tDataLen = len; }
	bool IsSmallPackage() const{ return (m_pMsg->tDataLen == m_pMsg->nFrameLen&&m_pMsg->tFrames-1 == m_pMsg->nFrames); }
	int GetSrcDataLen() const{ return m_pMsg->tSrcLen; }
	int GetDataLen()const { return m_pMsg->tDataLen; }
	int GetBaseProto()const { return m_pMsg->nProtoType; }
	void ResetData() { memset(m_buf, 0, len); }
	const FrameKey GetTimeStamp() {
		return m_pMsg->ntimestamp;
	}
	int GetTotalFrames() { return m_pMsg->tFrames; }
	int capacity() { return len; }
	int GetValidDataLen() { return m_nValidLen; }
	void ResetValidDataLen(int len) { m_nValidLen = len; }
	MsgFrame() { memset(this, 0, sizeof(*this)); m_index = -1; }
	int GetIndex()const { return m_index; }
	void SetIndex(int index) { m_index = index; }
	bool operator<(const MsgFrame& t1)const {
		return this->m_index < t1.m_index;
	}
public:
	unsigned short 	len;
	unsigned short	m_nValidLen;
	int				m_index;
	union {
		char*	m_buf;
		Msg*	m_pMsg;
	};
};

class FRAMEWORK_API DataHandle
{
public:
	DataHandle() :m_pTmpbuf(NULL){}
	virtual ~DataHandle(){}
public:	
	virtual int Init(int type,...){return 0;}
	//发送数据的处理(负责释放,使用者负责申请)
	virtual bool SetPackageInfo(void* buf,int len,int Srclen){ return 0;}
	void* getData(int& len){ return m_pTmpbuf;}
	virtual bool IsBaseProtocolPackage(int nProto) { return false; }
	//拆包,此处MsgFrame已在外部分配空间
	virtual int Packaging(vector<MsgFrame*>& frames, const std::deque<NetPackage >& theQueue,int& nFrameCount) {	return 0; }
	//组包
	virtual int setMsgFrames(const vector<MsgFrame>& frames,int count)
	{ 
		int len = 0;
		if (NULL == m_pTmpbuf) return 0;
		for (int i = 0; i < count; i++)
		{
			if (len > m_len || len + frames[i].m_pMsg->nDataLen > m_len)
				throw "setMsgFrames the recv data datalen not same as dataLen";

			memcpy(m_pTmpbuf + len, frames[i].m_pMsg->nData, frames[i].m_pMsg->nDataLen);
			len += frames[i].m_pMsg->nDataLen;
		}
		return len;
	}
	//数据处理
	bool process()
	{
		return encode()&&uncompress();
	}
protected:
	virtual int code(const char* src, int srclen, int len = 0) { return 0; }
	virtual int compress(const char* src, int srclen, int len = 0) { return 0; }
	//解密
	virtual bool encode(){ return true; }
	//解压缩
	virtual int uncompress(){ return 0; }
protected:
	int		m_len;
	char* 	m_pTmpbuf;
};

template < typename T >
class FRAMEWORK_API PackageProxy : public DataHandle
{
public:
	PackageProxy() {}
	int Init(int type,...)
	{
		va_list ap;
		va_start(ap,type);
		m_handle = (T*)va_arg(ap, void*);
		va_end(ap);
		return 0;
	}
	bool SetPackageInfo(void* buf,int len, int Srclen)
	{ 
		m_pTmpbuf = (char*)buf,m_len = Srclen;
		return m_handle->SetPackageInfo(buf, len, Srclen);
	}
	//说明:将待发送缓冲队列中数据,拆分(加密、压缩)并添加到IOCP请求帧缓冲中
	virtual int Packaging(vector<MsgFrame*>& frames, const std::deque<NetPackage >& theQueue,int& nFrameCount) {
		int fIndex = 0;
		char* pStart = frames[fIndex]->m_buf;
		int totalSend = 0;
		int count = 0;
		int totalCapacity = 0;
		int nOffset = 0;
	RESTART:
		for(int i=0;i<frames.size();i++)totalCapacity+=frames[i]->capacity();
		for (int i = 0; i < theQueue.size(); i++)
		{
			int retLen = m_handle->code((const char*)(theQueue[i].GetData()), theQueue[i].GetLen());
			int validLen = m_handle->compress((const char*)theQueue[i].GetData(), theQueue[i].GetLen(), retLen);
			count = i + 1;
			if (totalSend + validLen + (i + 1) * sizeof(Msg) > totalCapacity) {		
				if (0 == i)
				{
					totalCapacity = 0;
					frames.push_back(new MsgFrame);//TODO:此处需要进一步确认->一次数据发送大于帧缓冲大小
					goto RESTART;
				}
				if (i > 0 && nOffset > 0)
					frames[fIndex]->ResetValidDataLen(nOffset);
				count = i;
				break;
			}
			else {
				int len = 0;
				char* pData = NULL;
				long long ntimestamp = theQueue[i].GetTimestamp();
				int totalFrames = 0;
				list<TmpBuf>	transFrames;
				pData = (char*)m_handle->getData(len);
				if (len != validLen || NULL == pData )
					throw "process error";
				if (len > 0)
				{
					totalFrames = len / (TRANSFRAME - sizeof(Msg)) + (len - (len / (TRANSFRAME - sizeof(Msg))) * (TRANSFRAME - sizeof(Msg)) > 0 ? 1 : 0);
					for (int ci = 1; ci <= totalFrames; ci++)
					{
						if (ci*(TRANSFRAME - sizeof(Msg)) < len)
							transFrames.push_back(TmpBuf(pData + (ci - 1)*(TRANSFRAME - sizeof(Msg)), TRANSFRAME - sizeof(Msg)));
						else
							transFrames.push_back(TmpBuf(pData + (ci - 1)*(TRANSFRAME - sizeof(Msg)), len - (ci - 1)*(TRANSFRAME - sizeof(Msg))));
					}
					for (int fc = 0; !transFrames.empty(); fc++) {
						int nLen = transFrames.front().m_len;
						char* pDataStart = transFrames.front().m_buf;
						BuildPackage(&pStart, pDataStart, nLen, fIndex, frames, nOffset, ntimestamp, fc, theQueue[i].GetLen(), totalFrames, theQueue[i]);
						transFrames.pop_front();
					}
				}
				else {
					BuildPackage(&pStart, NULL, 0, fIndex, frames, nOffset, ntimestamp, 0, 0, 1, theQueue[i]);
				}
				if (fIndex == frames.size())
					break;
				totalSend += validLen;
			}

		}
		nFrameCount = fIndex + 1;
		//设置每个帧的总帧数
		return count;
	}
protected:	
	bool encode(){ return m_handle->encode(); }
	int uncompress(){ return m_handle->uncompress(); }
private:
	//param- nOffset :当前已使用的帧缓冲大小 
	void BuildPackage(char** ppStart, const char* pSrc, int len,int& fIndex,vector<MsgFrame*>& frames,int& nOffset,const long long& ntimestamp,
		int nFrameNo,int srcLen,int totalFrames,const NetPackage& curPackage)
	{	
		char* pStart = *ppStart;
		int nSrcLenbak = len;
		int AvailableFrameCount = frames[fIndex]->capacity() - nOffset;
		Msg frame ;
		memset(&frame, 0, sizeof(Msg));
		frame.nVerifyLen = curPackage.m_nVerifyLen;
		frame.ntimestamp = ntimestamp;
		frame.nProtoType = curPackage.m_nBaseProtol;
		frame.nFrames = nFrameNo;
		frame.tSrcLen = srcLen;//未经加密、压缩的原始数据长度
		frame.tDataLen = srcLen;
		frame.tFrames = totalFrames;
		frame.nUserProto = curPackage.m_nUserProtol;

		if (NULL == pSrc)
		{
			if (0 != len || 0 != nFrameNo || 0 != srcLen || 1 != totalFrames)
				throw "BuildPackage unknow error";
		}
		//1、发送帧缓冲大于传输帧
		if (len+sizeof(Msg)<= AvailableFrameCount) {
			frame.nDataLen = len;
			frame.nFrameLen = sizeof(Msg) + len;		
			memcpy(pStart, &frame, sizeof(Msg));
			if(NULL!=pSrc&&len)memcpy(pStart+sizeof(Msg), pSrc, len);
			pStart += len + sizeof(Msg);
			nOffset += len + sizeof(Msg);
			*ppStart = pStart;
			frames[fIndex]->ResetValidDataLen(nOffset);
			if (len + sizeof(Msg) == AvailableFrameCount) {
				*ppStart = frames[++fIndex]->m_buf;
				nOffset = 0;
			}
			len = 0;
			return;
		}
		else 
		{
			int lastLen = 0;
			int tmpOffset = 0;
			if (sizeof(Msg) <= AvailableFrameCount)
			{
				if (fIndex == frames.size() - 1)
					throw "buildPackage error";
				memcpy(pStart, &frame, sizeof(Msg));
				Msg* pTmpF =(Msg*) pStart;
				if((tmpOffset=AvailableFrameCount - sizeof(Msg))>0)memcpy(pStart + sizeof(Msg), pSrc, tmpOffset),len -= tmpOffset;
				frames[fIndex]->ResetValidDataLen(frames[fIndex]->capacity());
				pStart = frames[++fIndex]->m_buf;
				memcpy(pStart , pSrc + tmpOffset, lastLen = len > TRANSFRAME-sizeof(Msg)-tmpOffset ? TRANSFRAME-sizeof(Msg)- tmpOffset : len);
				len -= lastLen;
				nOffset = lastLen;
				pStart += lastLen;
				*ppStart = pStart;
				tmpOffset += lastLen;
				pTmpF->nFrameLen = sizeof(Msg) + tmpOffset;
				pTmpF->nDataLen = tmpOffset;
				frames[fIndex]->ResetValidDataLen(nOffset);
			}
			else {
				union shortUncharA
				{
					unsigned short  data;
					unsigned char	daA[2];
				};
				unsigned char* pAFrameLen[2] = {};
				unsigned char* pADataLen[2] = {};
				unsigned char* pTmp = NULL;
				shortUncharA	frameLen = { 0 };
				shortUncharA	dataLen = { 0 };

				memcpy(pStart, &frame, AvailableFrameCount);
				pTmp = (unsigned char*)pStart;
				frames[fIndex]->ResetValidDataLen(frames[fIndex]->capacity());
				pStart = frames[++fIndex]->m_buf;

				if (AvailableFrameCount >= offsetof(MsgHead, nFrameLen) + 1)
				{
					pAFrameLen[0] = pTmp + offsetof(MsgHead, nFrameLen);
					pAFrameLen[1] = pTmp + offsetof(MsgHead, nFrameLen) + 1;

					pADataLen[0] = pTmp + offsetof(MsgHead, nDataLen);
					pADataLen[1] = pTmp + offsetof(MsgHead, nDataLen) + 1;
				}
				else if (AvailableFrameCount == offsetof(MsgHead, nFrameLen))
				{
					pAFrameLen[0] = pTmp + offsetof(MsgHead, nFrameLen);
					pAFrameLen[1] = (unsigned char*)pStart;

					pADataLen[0] = pTmp + offsetof(MsgHead, nDataLen);
					pADataLen[1] = pTmp + offsetof(MsgHead, nDataLen) + 1;
				}
				else if (AvailableFrameCount >= offsetof(MsgHead, nDataLen) + 1)
				{
					pAFrameLen[0] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount;
					pAFrameLen[1] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount + 1;

					pADataLen[0] = pTmp + offsetof(MsgHead, nDataLen);
					pADataLen[1] = pTmp + offsetof(MsgHead, nDataLen) + 1;
				}
				else if (AvailableFrameCount == offsetof(MsgHead, nDataLen))
				{
					pAFrameLen[0] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount;
					pAFrameLen[1] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount + 1;

					pADataLen[0] = pTmp + offsetof(MsgHead, nDataLen);
					pADataLen[1] = (unsigned char*)pStart;
				}
				else if (AvailableFrameCount < offsetof(MsgHead, nDataLen))
				{
					pAFrameLen[0] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount;
					pAFrameLen[1] = (unsigned char*)pStart + offsetof(MsgHead, nFrameLen) - AvailableFrameCount + 1;

					pADataLen[0] = (unsigned char*)pStart + offsetof(MsgHead, nDataLen) - AvailableFrameCount;
					pADataLen[1] = (unsigned char*)pStart + offsetof(MsgHead, nDataLen) - AvailableFrameCount + 1;
				}

				memcpy(pStart, (char*)&frame+ AvailableFrameCount,sizeof(Msg) - AvailableFrameCount);
				pStart += sizeof(Msg) - AvailableFrameCount;
				memcpy(pStart, pSrc, lastLen = len > TRANSFRAME - sizeof(Msg) ? TRANSFRAME - sizeof(Msg) : len);
				len -= lastLen;
				tmpOffset = lastLen;
				nOffset = lastLen;
				pStart += lastLen;
				*ppStart = pStart;

				frameLen.data = sizeof(Msg) + tmpOffset;
				dataLen.data = tmpOffset;
				*pAFrameLen[0] = frameLen.daA[0];
				*pAFrameLen[1] = frameLen.daA[1];
				*pADataLen[0] = dataLen.daA[0];
				*pADataLen[1] = dataLen.daA[1];
				frames[fIndex]->ResetValidDataLen(nOffset+ sizeof(Msg) - AvailableFrameCount);
			}
		}
	}
private:
	T*	m_handle;
};

#endif // DataHandle.h

