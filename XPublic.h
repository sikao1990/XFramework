#ifndef __XPublic_H__
#define __XPublic_H__

#include "DirPtr/DirPtr.h"
#include "DyPlugin/DyPlugin.h"
#include "Mem/MemMgr.h"
#include "Mem/MemPool.h"
#include "Mem/XAllocator.h"
#include "Mem/ObjPool.h"
#include "Mem/MemStatistics.h"
#include "Pattern/TreeRelation.h"
#include "ShareMemory/ShareMemory.h"
#include "SystemApi/ForceConvert.h"
#include "SystemApi/TimeInfo.h"
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
// #include "DB/DBDefine.h"
// #include "DB/DBAccess.h"
// #include "DB/DBConnect.h"
// #include "DB/DBConnPool.h"
// #include "DB/MysqlAccess.h"
// #include "DB/MysqlConn.h"
// #include "NetFrame/DataHandle.h"
// #include "NetFrame/InterfaceAdapter.h"
// #include "NetFrame/NetConfig.h"
// #include "NetFrame/NetPackage.h"

#ifdef WIN32
#include "SystemApi/SystemApi_Win32.h"
#include "Thread/ThreadImpl_Win32.h"
// #include "DB/AdoAccess.h"
// #include "DB/AdoConn.h"
#else
#include "SystemApi/SystemApi_Linux.h"
#include "Thread/MultiThreadSync_Linux.h"
#include "Thread/ThreadImpl_Linux.h"
#include "Thread/MultiThreadSync_Linux.h"
#endif

#endif	// XPublic.h

