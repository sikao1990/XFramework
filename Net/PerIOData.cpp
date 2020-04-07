#include "PerIOData.h"
 
// default buffer size
#define BUFFERSIZE 1024
//new buffer and Initialize
PerIOData::PerIOData(void):
	nUseFlag(-1), nMemFlag(0),
	nIndex(-1), m_pPrevIOData(NULL),
	operationtype(NULL_POSTED),
	acceptsocket(INVALID_SOCKET)
{
	databuf.buf = NULL;
	databuf.len = 0;
	ZeroMemory(&overlapped, sizeof(overlapped));  
}

PerIOData::PerIOData(int nflag) :
	nUseFlag(-1),
	nIndex(-1),
	nMemFlag(nflag),
	operationtype(NULL_POSTED),
	acceptsocket(INVALID_SOCKET)
{
	if (nMemFlag) {
		databuf.buf = new char[BUFFERSIZE];
		databuf.len = BUFFERSIZE;
	}
	else {
		databuf.buf = NULL;
		databuf.len = 0;
	}
	ZeroMemory(&overlapped, sizeof(overlapped));
}
 
//delete the buffer
PerIOData::~PerIOData(void)
{
	if( nMemFlag && NULL != databuf.buf)
	{
		delete [] databuf.buf;
		databuf.buf = NULL;
		nMemFlag = FALSE;
	}
}
 
//reset buffer and overlapped of io
void PerIOData::Reset(void)
{
	nIndex = -1;
	nUseFlag = -1;
	ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
}
