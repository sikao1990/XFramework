#ifndef __XPublic_H__
#define __XPublic_H__

#include "DirPtr/Dir.h"
#include "DirPtr/DirPtr.h"
#include "DyPlugin/DyPlugin.h"
#include "Mem/RefPtr.h"
#include "Mem/XMem.h"
#include "Mem/MemMgr.h"
#include "Mem/MemPool.h"
#include "Mem/XAllocator.h"
#include "Mem/XStaticAlloc.h"
#include "Mem/ObjPool.h"
#include "Mem/MemStatistics.h"
#include "Pattern/TreeRelation.h"
#include "SharedMemory/SharedMemory.h"
#include "SystemApi/ForceConvert.h"
#include "SystemApi/TimeInfo.h"
#include "SystemApi/KMDefine.h"
#include "SystemApi/KMInput.h"
#include "SystemApi/Process.h"
#include "Thread/AtomMutex.h"
#include "Thread/RWLock.h"
#include "Thread/Semaphore.h"
#include "Thread/SemaphoreEx.h"
#include "Thread/Task.h"
#include "Thread/TaskCtrl.h"
#include "Thread/TaskProcess.h"
#include "Thread/Thread.h"
#include "Thread/ThreadCtrlEx.h"
#include "Thread/ThreadPool.h"
#include "Thread/Timer.h"
#include "Thread/TimerMgr.h"
#include "Thread/MsTimerMgr.h"
#include "Thread/ThreadPri.h"
#include "Thread/MutexInteger.h"
#include "DB/DBDefine.h"
#include "DB/DBAccess.h"
#include "DB/DBConnect.h"
#include "DB/DBConnPool.h"
#include "DB/MysqlAccess.h"
#include "DB/MysqlConn.h"
#include "Net/DataHandle.h"
#include "Net/NetPackage.h"
#include "Net/XNetCfg.h"
#include "Net/PackPerData.h"
#include "Net/SockBase.h"
#include "Net/SockHandle.h"
#include "Net/SocketSession.h"
#include "Net/XTcpClient.h"
#include "Net/XTcpServer.h"
#include "Net/XUdpClient.h"
#include "Net/XUdpServer.h"

#ifdef WIN32
#include "SystemApi/SystemApi_Win32.h"
#include "SystemApi/Service_Win32.h"
#include "SystemApi/RegItem_Win32.h"
#include "SystemApi/ProcessMutex_Win32.h"
#include "SystemAPi/ProcessEvent_Win32.h"
#include "Thread/ThreadImpl_Win32.h"
#include "Thread/AtomMutexEx_Win32.h"
#include "DB/AdoAccess.h"
#include "DB/AdoConn.h"
#include "Net/PerIOData.h"
#include "Net/BalanceHandle_Win32.h"
#include "Net/PostNodeMgr_Win32.h"
#elif __linux__
#include "SystemApi/SystemApi_Linux.h"
#include "Thread/MultiThreadSync_Linux.h"
#include "Thread/ThreadImpl_Linux.h"
#include "Net/AsyncClient.h"
#include "Net/AsyncServer.h"
#include "Net/AsyncNetClientImpl_Linux.h"
#include "Net/AsyncNetServerImpl_Linux.h"
#include "Net/BalanceHandle_Linux.h"
#include "Net/PostNodeMgr_Linux.h"
#elif   __APPLE__ || __MACH__
#include "SystemApi/SystemApi_Linux.h"
#include "Thread/ThreadImpl_Linux.h"
#include "Thread/MultiThreadSync_Linux.h"
#include "Net/AsyncNetServerImpl_Apple.h"
#include "Net/AsyncClient.h"
#include "Net/AsyncServer.h"
#include "Net/AsyncNetClientImpl_Apple.h"
#include "Net/BalanceHandle_Linux.h"
#include "Net/PostNodeMgr_Linux.h"
#endif

#endif	// XPublic.h

