import sys;
import os;
import platform;

env = Environment()

bStatic = ARGUMENTS.get('enable-static','true')
bShared = ARGUMENTS.get('enable-shared','true')

TEMPDIR = '/'
CURDIR = os.getcwd()
WORKDIR = CURDIR+TEMPDIR

ostype = platform.system()

winsource = [
    WORKDIR+'DB/AdoAccess.cpp',
    WORKDIR+'DB/AdoConn.cpp',
    WORKDIR+'DB/DBAccess.cpp',
    WORKDIR+'DB/DBConnect.cpp',
    WORKDIR+'DB/DBConnPool.cpp',
    WORKDIR+'DB/MysqlAccess.cpp',
    WORKDIR+'DB/MysqlConn.cpp',
    WORKDIR+'DirPtr/DirPtrImpl_Win32.cpp',
    WORKDIR+'DirPtr/DirPtr.cpp',
    WORKDIR+'DyPlugin/DyPluginImpl_Win32.cpp',
    WORKDIR+'DyPlugin/DyPlugin.cpp',
    WORKDIR+'Mem/MemStatistics.cpp',
    WORKDIR+'Mem/MemPool.cpp',
    WORKDIR+'Mem/MemMgr.cpp',
    WORKDIR+'Mem/XStaticAlloc.cpp',
    WORKDIR+'Net/BalanceHandle_Win32.cpp',
    WORKDIR+'Net/PackPerData.cpp',
    WORKDIR+'Net/PerIOData.cpp',
    WORKDIR+'Net/PostNodeMgr_Win32.cpp',
    WORKDIR+'Net/SocketSession.cpp',
    WORKDIR+'Net/TcpClientImpl_Win32.cpp',
    WORKDIR+'Net/TcpServerImpl_Win32.cpp',
    WORKDIR+'Net/XTcpClient.cpp',
    WORKDIR+'Net/XTcpServer.cpp',
    WORKDIR+'SharedMemory/SharedMemoryImpl_Win32.cpp',
    WORKDIR+'SharedMemory/SharedMemory.cpp',
    WORKDIR+'SystemApi/SystemApi_Win32.cpp',
    WORKDIR+'SystemApi/TimeInfo.cpp',
    WORKDIR+'Thread/AtomMutexImpl_Win32.cpp',
    WORKDIR+'Thread/AtomMutex.cpp',
    WORKDIR+'Thread/MsTimerMgrImpl_Win32.cpp',
    WORKDIR+'Thread/MsTimerMgr.cpp',
    WORKDIR+'Thread/MutexIntegerImpl_Win32.cpp',
    WORKDIR+'Thread/MutexInteger.cpp',
    WORKDIR+'Thread/RWLockImpl_Win32.cpp',
    WORKDIR+'Thread/RWLock.cpp',
    WORKDIR+'Thread/SemaphoreImpl_Win32.cpp',
    WORKDIR+'Thread/Semaphore.cpp',
    WORKDIR+'Thread/SemaphoreExImpl_Win32.cpp',
    WORKDIR+'Thread/SemaphoreEx.cpp',
    WORKDIR+'Thread/TaskCtrlImpl_Win32.cpp',
    WORKDIR+'Thread/TaskCtrl.cpp',
    WORKDIR+'Thread/Thread.cpp',
    WORKDIR+'Thread/ThreadCtrlExImpl_Win32.cpp',
    WORKDIR+'Thread/ThreadCtrlEx.cpp',
    WORKDIR+'Thread/ThreadPoolImpl_Win32.cpp',
    WORKDIR+'Thread/ThreadPool.cpp',
    WORKDIR+'Thread/TimerMgrImpl_Win32.cpp',
    WORKDIR+'Thread/TimerMgr.cpp'
]
winlib = [
]
wincppflag = [
    '-DWIN32',
    '-DFRAMEWORK_EXPORTS'
]
winlinkflag = [
]
wincpppath = [
]
winlibpath = [
]

linuxsource = [
    WORKDIR+'DB/DBAccess.cpp',
    WORKDIR+'DB/DBConnect.cpp',
    WORKDIR+'DB/DBConnPool.cpp',
    WORKDIR+'DB/MysqlAccess.cpp',
    WORKDIR+'DB/MysqlConn.cpp',
    WORKDIR+'DirPtr/DirPtrImpl_Linux.cpp',
    WORKDIR+'DirPtr/DirPtr.cpp',
    WORKDIR+'DyPlugin/DyPluginImpl_Linux.cpp',
    WORKDIR+'DyPlugin/DyPlugin.cpp',
    WORKDIR+'Mem/MemStatistics.cpp',
    WORKDIR+'Mem/MemPool.cpp',
    WORKDIR+'Mem/MemMgr.cpp',
    WORKDIR+'Mem/XStaticAlloc.cpp',
    WORKDIR+'Net/AsyncClient.cpp',
    WORKDIR+'Net/AsyncNetClientImpl_Linux.cpp',
    WORKDIR+'Net/AsyncServer.cpp',
    WORKDIR+'Net/AsyncNetServerImpl_Linux.cpp',
    WORKDIR+'Net/BalanceHandle_Linux.cpp',
    WORKDIR+'Net/PackPerData.cpp',
    WORKDIR+'Net/PostNodeMgr_Linux.cpp',
    WORKDIR+'Net/SocketSession.cpp',
    WORKDIR+'Net/TcpClientImpl_Linux.cpp',
    WORKDIR+'Net/TcpServerImpl_Linux.cpp',
    WORKDIR+'Net/XTcpClient.cpp',
    WORKDIR+'Net/XTcpServer.cpp',
    WORKDIR+'SharedMemory/SharedMemoryImpl_Linux.cpp',
    WORKDIR+'SharedMemory/SharedMemory.cpp',
    WORKDIR+'SystemApi/SystemApi_Linux.cpp',
    WORKDIR+'SystemApi/TimeInfo.cpp',
    WORKDIR+'Thread/AtomMutexImpl_Linux.cpp',
    WORKDIR+'Thread/AtomMutex.cpp',
    WORKDIR+'Thread/MsTimerMgrImpl_Linux.cpp',
    WORKDIR+'Thread/MsTimerMgr.cpp',
    WORKDIR+'Thread/MultiThreadSync_Linux.cpp',
    WORKDIR+'Thread/MutexIntegerImpl_Linux.cpp',
    WORKDIR+'Thread/MutexInteger.cpp',
    WORKDIR+'Thread/RWLockImpl_Linux.cpp',
    WORKDIR+'Thread/RWLock.cpp',
    WORKDIR+'Thread/SemaphoreImpl_Linux.cpp',
    WORKDIR+'Thread/Semaphore.cpp',
    WORKDIR+'Thread/SemaphoreExImpl_Linux.cpp',
    WORKDIR+'Thread/SemaphoreEx.cpp',
    WORKDIR+'Thread/TaskCtrlImpl_Linux.cpp',
    WORKDIR+'Thread/TaskCtrl.cpp',
    WORKDIR+'Thread/Thread.cpp',
    WORKDIR+'Thread/ThreadCtrlExImpl_Linux.cpp',
    WORKDIR+'Thread/ThreadCtrlEx.cpp',
    WORKDIR+'Thread/ThreadPoolImpl_Linux.cpp',
    WORKDIR+'Thread/ThreadPool.cpp',
    WORKDIR+'Thread/TimerMgrImpl_Linux.cpp',
    WORKDIR+'Thread/TimerMgr.cpp'
]
linuxlib = [
    'pthread',
    'mysqlclient'
]
linuxcppflag = [
    '-std=c++11'
]
linuxlinkflag = [
]
linuxcpppath = [
]
linuxlibpath = [
    '/usr/lib64/mysql'
]

macsource = [
    WORKDIR+'DB/DBAccess.cpp',
    WORKDIR+'DB/DBConnect.cpp',
    WORKDIR+'DB/DBConnPool.cpp',
    WORKDIR+'DB/MysqlAccess.cpp',
    WORKDIR+'DB/MysqlConn.cpp',
    WORKDIR+'DirPtr/DirPtrImpl_Linux.cpp',
    WORKDIR+'DirPtr/DirPtr.cpp',
    WORKDIR+'DyPlugin/DyPluginImpl_Linux.cpp',
    WORKDIR+'DyPlugin/DyPlugin.cpp',
    WORKDIR+'Mem/MemStatistics.cpp',
    WORKDIR+'Mem/MemPool.cpp',
    WORKDIR+'Mem/MemMgr.cpp',
    WORKDIR+'Mem/XStaticAlloc.cpp',
    WORKDIR+'Net/AsyncClient.cpp',
    WORKDIR+'Net/AsyncNetClientImpl_Apple.cpp',
    WORKDIR+'Net/AsyncServer.cpp',
    WORKDIR+'Net/AsyncNetServerImpl_Apple.cpp',
    WORKDIR+'Net/BalanceHandle_Linux.cpp',
    WORKDIR+'Net/PackPerData.cpp',
    WORKDIR+'Net/PostNodeMgr_Linux.cpp',
    WORKDIR+'Net/SocketSession.cpp',
    WORKDIR+'Net/TcpClientImpl_Linux.cpp',
    WORKDIR+'Net/TcpServerImpl_Linux.cpp',
    WORKDIR+'Net/XTcpClient.cpp',
    WORKDIR+'Net/XTcpServer.cpp',
    WORKDIR+'SharedMemory/SharedMemoryImpl_Linux.cpp',
    WORKDIR+'SharedMemory/SharedMemory.cpp',
    WORKDIR+'SystemApi/SystemApi_Linux.cpp',
    WORKDIR+'SystemApi/TimeInfo.cpp',
    WORKDIR+'Thread/AtomMutexImpl_Linux.cpp',
    WORKDIR+'Thread/AtomMutex.cpp',
    WORKDIR+'Thread/MsTimerMgrImpl_Linux.cpp',
    WORKDIR+'Thread/MsTimerMgr.cpp',
    WORKDIR+'Thread/MultiThreadSync_Linux.cpp',
    WORKDIR+'Thread/MutexIntegerImpl_Linux.cpp',
    WORKDIR+'Thread/MutexInteger.cpp',
    WORKDIR+'Thread/RWLockImpl_Linux.cpp',
    WORKDIR+'Thread/RWLock.cpp',
    WORKDIR+'Thread/SemaphoreImpl_Linux.cpp',
    WORKDIR+'Thread/Semaphore.cpp',
    WORKDIR+'Thread/SemaphoreExImpl_Linux.cpp',
    WORKDIR+'Thread/SemaphoreEx.cpp',
    WORKDIR+'Thread/TaskCtrlImpl_Linux.cpp',
    WORKDIR+'Thread/TaskCtrl.cpp',
    WORKDIR+'Thread/Thread.cpp',
    WORKDIR+'Thread/ThreadCtrlExImpl_Linux.cpp',
    WORKDIR+'Thread/ThreadCtrlEx.cpp',
    WORKDIR+'Thread/ThreadPoolImpl_Linux.cpp',
    WORKDIR+'Thread/ThreadPool.cpp',
    WORKDIR+'Thread/TimerMgrImpl_Linux.cpp',
    WORKDIR+'Thread/TimerMgr.cpp'
]
maclib = [
    'pthread',
    'mysqlclient'
]
maccppflag = [
    '-std=c++11'
]
maclinkflag = [
]
maccpppath = [
    '/usr/local/Cellar/mysql-client/8.0.21/include'
]
maclibpath = [
    '/usr/local/Cellar/mysql-client/8.0.21/lib/'
]

sources = [
]
libs = [
]
cppflags = [
]
linkflags = [
]
libpath = [
]
cpppath = [
]

print (WORKDIR)

if ostype == 'Windows':
    print ('Windows')
    sources.extend(winsource)
    libs.extend(winlib)
    cppflags.extend(wincppflag)
    linkflags.extend(winlinkflag)
    libpath.extend(winlibpath)
    cpppath.extend(wincpppath)
elif ostype == 'Linux':
    print ('Linux')
    sources.extend(linuxsource)
    libs.extend(linuxlib)
    cppflags.extend(linuxcppflag)
    linkflags.extend(linuxlinkflag)
    libpath.extend(linuxlibpath)
    cpppath.extend(linuxcpppath)
elif ostype == 'Darwin':
    print ('Darwin')
    sources.extend(macsource)
    libs.extend(maclib)
    cppflags.extend(maccppflag)
    linkflags.extend(maclinkflag)
    libpath.extend(maclibpath)
    cpppath.extend(maccpppath)


if bStatic == 'true':
    print ('build static')
    StaticLibrary('Framework',
        sources,
        LIBS = libs,
        CPPFLAGS = cppflags,
        LINKFLAGS = linkflags,
        LIBPATH = libpath,
        CPPPATH = cpppath,
    )
if bShared == 'true':
    print ('build shared')
    SharedLibrary('Framework',
        sources,
        LIBS = libs,
        CPPFLAGS = cppflags,
        LINKFLAGS = linkflags,
        LIBPATH = libpath,
        CPPPATH = cpppath,
    )
