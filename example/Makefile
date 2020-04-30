EXE = test
SRC = 	../../Framework/DB/DBAccess.cpp							\
		../../Framework/DB/DBConnect.cpp						\
		../../Framework/DB/DBConnPool.cpp						\
		../../Framework/DB/MysqlAccess.cpp						\
		../../Framework/DB/MysqlConn.cpp						\
		../../Framework/DirPtr/DirPtr.cpp						\
		../../Framework/DirPtr/DirPtrImpl_Linux.cpp				\
		../../Framework/DyPlugin/DyPlugin.cpp					\
		../../Framework/DyPlugin/DyPluginImp_Linux.cpp			\
		../../Framework/Mem/MemMgr.cpp							\
		../../Framework/Mem/MemPool.cpp							\
		../../Framework/Mem/MemStatistics.cpp					\
		../../Framework/Mem/XStaticAlloc.cpp					\
		../../Framework/Net/BalanceHandle_Linux.cpp				\
		../../Framework/Net/EpollClient.cpp						\
		../../Framework/Net/EpollServer.cpp						\
		../../Framework/Net/PackPerData.cpp						\
		../../Framework/Net/PostNodeMgr_Linux.cpp				\
		../../Framework/Net/SocketSession.cpp					\
		../../Framework/Net/TcpClientImpl_Linux.cpp				\
		../../Framework/Net/TcpServerImpl_Linux.cpp				\
		../../Framework/Net/XTcpClient.cpp						\
		../../Framework/Net/XTcpServer.cpp						\
		../../Framework/ShareMemory/ShareMemory.cpp				\
		../../Framework/ShareMemory/ShareMemoryImpl_Linux.cpp	\
		../../Framework/SystemApi/SystemApi_Linux.cpp			\
		../../Framework/SystemApi/TimeInfo.cpp					\
		../../Framework/Thread/AtomMutex.cpp					\
		../../Framework/Thread/AtomMutexImpl_Linux.cpp			\
		../../Framework/Thread/MultiThreadSync_Linux.cpp		\
		../../Framework/Thread/MutexInteger.cpp					\
		../../Framework/Thread/MutexIntegerImpl_Linux.cpp		\
		../../Framework/Thread/MsTimerMgr.cpp					\
		../../Framework/Thread/MsTimerMgrImpl_Linux.cpp			\
		../../Framework/Thread/RWLock.cpp						\
		../../Framework/Thread/RWLockImpl_Linux.cpp				\
		../../Framework/Thread/Semaphore.cpp					\
		../../Framework/Thread/SemaphoreEx.cpp					\
		../../Framework/Thread/SemaphoreExImpl_Linux.cpp		\
		../../Framework/Thread/SemaphoreImpl_Linux.cpp			\
		../../Framework/Thread/TaskCtrl.cpp						\
		../../Framework/Thread/TaskCtrlImpl_Linux.cpp			\
		../../Framework/Thread/Thread.cpp						\
		../../Framework/Thread/ThreadCtrlEx.cpp					\
		../../Framework/Thread/ThreadCtrlExImpl_Linux.cpp		\
		../../Framework/Thread/ThreadPool.cpp					\
		../../Framework/Thread/ThreadPoolImpl_Linux.cpp			\
		../../Framework/Thread/TimerMgr.cpp						\
		../../Framework/Thread/TimerMgrImpl_Linux.cpp			\
		test.cpp
		
CFLAGS = -g -rdynamic -Werror -ldl -std=c++11
		
LIB = -lpthread	-lmysqlclient -L/usr/lib64/mysql

CC = g++

TARGET:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE) $(LIB)
clean:
	rm *.o $(EXE)
	
