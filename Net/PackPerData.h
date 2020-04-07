#ifndef __PackPerData_H__
#define __PackPerData_H__

#include "XNetCfg.h"
#include "DataHandle.h"

using namespace std;

//附:读和写请求不共用
class SocketSession;
class PackPerData 
{
public:
	PackPerData(hSockFd socket, int nIndex, void* buf, int size, SocketSession* pSession);
	~PackPerData();
	bool IsComplate();
	void SetNoticeStatus(bool f = false);
	void PostRecv(void* pStart=NULL);
	void PostSend();
	int RecvData(void* pdata, int len, int Index, int& nFlag, void* pPrevPerData,void* pPostNode);
	int RecordSendData(void* pData, int len,void* pPostNode);
	int GetTotalFrames() { return m_frame.m_pMsg->tFrames; }
	MsgFrame* GetFrame() { return &m_frame; }
	const MsgFrame GetCurComplateFrame();
	void ResetData();
	int GetPosition()const { return m_index; }
	int HasExtendData() { return m_nExtendLen; }
	void* GetExtendData(int& len) { len = m_nExtendLen; return m_pExtendStart; }
	void MoveExtendData();
	int GetPrevIndex();
	void RestRecvFlag() { m_pPrevPerData = NULL; }
	int GetOtherFrameCount(vector<MsgFrame>& vFrames);
	void ResetIndex(int index) { m_frame.m_index = m_index = index; }
private:
	hSockFd				m_socket;	
	unsigned			m_isFinish : 4,
						m_index : 4,
						m_nPost : 4,
								: 4,
						m_nExtendLen : 16;
	SocketSession*		m_pSession;
	void*				m_pExtendStart;
	PackPerData*		m_pPrevPerData;
	volatile void*		m_pDataCur;
	MsgFrame			m_frame;
};


#endif //PackPerData.h

