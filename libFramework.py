import sys;
import os;
import platform;

env = Environment()
env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1

bStatic = ARGUMENTS.get('enable-static','true')
bShared = ARGUMENTS.get('enable-shared','true')

TEMPDIR = '/'
CURDIR = os.getcwd()
WORKDIR = CURDIR+TEMPDIR
print(WORKDIR)
#fpos = int(WORKDIR.rindex('bin'))
BASEDIR = CURDIR#= WORKDIR[0:int(fpos)]

env.VariantDir('bin',BASEDIR,duplicate=0)
ostype = platform.system()

sources = [
    BASEDIR+'/DB/DBAccess.cpp',
	BASEDIR+'/DB/DBConnect.cpp',
	BASEDIR+'/DB/DBConnPool.cpp',
	BASEDIR+'/DB/MangoAccess.cpp',
	BASEDIR+'/DB/MangoConn.cpp',
	BASEDIR+'/DB/ODBCAccess.cpp',
	BASEDIR+'/DB/ODBCConn.cpp',
	BASEDIR+'/DB/OracleAccess.cpp',
	BASEDIR+'/DB/OracleConn.cpp',
	BASEDIR+'/DB/SqliteAccess.cpp',
	BASEDIR+'/DB/SqliteConn.cpp',
	BASEDIR+'/DB/PostgreAccess.cpp',
	BASEDIR+'/DB/PostgreConn.cpp',
    BASEDIR+'/DirPtr/Dir.cpp',
	BASEDIR+'/DirPtr/DirPtr.cpp',
    BASEDIR+'/DyPlugin/DyPlugin.cpp',
    BASEDIR+'/Mem/MemMgr.cpp',
    BASEDIR+'/Mem/MemPool.cpp',
    BASEDIR+'/Mem/MemStatistics.cpp',
    BASEDIR+'/Mem/XStaticAlloc.cpp',
	BASEDIR+'/Net/XUdpServer.cpp',
	BASEDIR+'/Net/XUdpClient.cpp',
	BASEDIR+'/Net/XTcpServer.cpp',
	BASEDIR+'/Net/XTcpClient.cpp',
	BASEDIR+'/Net/SocketSession.cpp',
	BASEDIR+'/Net/PackPerData.cpp',
    BASEDIR+'/SharedMemory/SharedMemory.cpp',
    BASEDIR+'/SystemApi/KMInput.cpp',
	BASEDIR+'/SystemApi/Process.cpp',
	BASEDIR+'/SystemApi/TimeInfo.cpp',
	BASEDIR+'/Thread/AtomMutex.cpp',
	BASEDIR+'/Thread/MsTimerMgr.cpp',
	BASEDIR+'/Thread/MutexInteger.cpp',
	BASEDIR+'/Thread/RWLock.cpp',
	BASEDIR+'/Thread/Semaphore.cpp',
	BASEDIR+'/Thread/SemaphoreEx.cpp',
	BASEDIR+'/Thread/Thread.cpp',
	BASEDIR+'/Thread/TaskCtrl.cpp',
	BASEDIR+'/Thread/ThreadCtrlEx.cpp',
	BASEDIR+'/Thread/ThreadPool.cpp',
	BASEDIR+'/Thread/TimerMgr.cpp',
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

winsource = [
    BASEDIR+'/DB/AdoAccess.cpp',
	BASEDIR+'/DB/AdoConn.cpp',
	BASEDIR+'/DB/MysqlAccess.cpp',
	BASEDIR+'/DB/MysqlConn.cpp',
    BASEDIR+'/DirPtr/DirPtrImpl_Win32.cpp',
    BASEDIR+'/DirPtr/DirImpl_Win32.cpp',
    BASEDIR+'/DyPlugin/DyPluginImpl_Win32.cpp',
	BASEDIR+'/Net/TcpServerImpl_Win32.cpp',
	BASEDIR+'/Net/TcpClientImpl_Win32.cpp',
	BASEDIR+'/Net/PostNodeMgr_Win32.cpp',
	BASEDIR+'/Net/BalanceHandle_Win32.cpp',
	BASEDIR+'/Net/PerIOData.cpp',
    BASEDIR+'/SharedMemory/SharedMemoryImpl_Win32.cpp',
	BASEDIR+'/SystemApi/SystemApi_Win32.cpp',
	BASEDIR+'/SystemApi/Service_Win32.cpp',
	BASEDIR+'/SystemApi/RegItem_Win32.cpp',
	BASEDIR+'/SystemApi/ProcessImpl_Win32.cpp',
	BASEDIR+'/SystemApi/ProcessEvent_Win32.cpp',
	BASEDIR+'/SystemApi/ProcessMutex_Win32.cpp',
	BASEDIR+'/SystemApi/KMInputImpl_Win32.cpp',
	BASEDIR+'/Thread/AtomMutexImpl_Win32.cpp',
	BASEDIR+'/Thread/AtomMutexEx_Win32.cpp',
	BASEDIR+'/Thread/MsTimerMgrImpl_Win32.cpp',
	BASEDIR+'/Thread/MutexIntegerImpl_Win32.cpp',
	BASEDIR+'/Thread/RWLockImpl_Win32.cpp',
	BASEDIR+'/Thread/SemaphoreImpl_Win32.cpp',
	BASEDIR+'/Thread/SemaphoreExImpl_Win32.cpp',
	BASEDIR+'/Thread/TaskCtrlImpl_Win32.cpp',
	BASEDIR+'/Thread/ThreadCtrlExImpl_Win32.cpp',
	BASEDIR+'/Thread/ThreadPoolImpl_Win32.cpp',
	BASEDIR+'/Thread/TimerMgrImpl_Win32.cpp',
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
	BASEDIR+'/DB/MysqlAccess.cpp',
	BASEDIR+'/DB/MysqlConn.cpp',
    BASEDIR+'/DirPtr/DirImpl_Linux.cpp',
	BASEDIR+'/DirPtr/DirPtrImpl_Linux.cpp',
    BASEDIR+'/DyPlugin/DyPluginImpl_Linux.cpp',
	BASEDIR+'/Net/TcpServerImpl_Linux.cpp',
	BASEDIR+'/Net/TcpClientImpl_Linux.cpp',
	BASEDIR+'/Net/PostNodeMgr_Linux.cpp',
	BASEDIR+'/Net/BalanceHandle_Linux.cpp',
	BASEDIR+'/Net/AsyncServer.cpp',
	BASEDIR+'/Net/AsyncClient.cpp',
	BASEDIR+'/Net/AsyncNetServerImpl_Linux.cpp',
	BASEDIR+'/Net/AsyncNetClientImpl_Linux.cpp',
    BASEDIR+'/SharedMemory/SharedMemoryImpl_Linux.cpp',
    BASEDIR+'/SystemApi/SystemApi_Linux.cpp',
	BASEDIR+'/SystemApi/ProcessImpl_Linux.cpp',
	BASEDIR+'/SystemApi/KMInputImpl_Linux.cpp',
	BASEDIR+'/Thread/AtomMutexImpl_Linux.cpp',
	BASEDIR+'/Thread/MsTimerMgrImpl_Linux.cpp',
	BASEDIR+'/Thread/MultiThreadSync_Linux.cpp',
	BASEDIR+'/Thread/MutexIntegerImpl_Linux.cpp',
	BASEDIR+'/Thread/RWLockImpl_Linux.cpp',
	BASEDIR+'/Thread/SemaphoreImpl_Linux.cpp',
	BASEDIR+'/Thread/SemaphoreExImpl_Linux.cpp',
	BASEDIR+'/Thread/TaskCtrlImpl_Linux.cpp',
	BASEDIR+'/Thread/ThreadCtrlExImpl_Linux.cpp',
	BASEDIR+'/Thread/ThreadPoolImpl_Linux.cpp',
	BASEDIR+'/Thread/TimerMgrImpl_Linux.cpp',
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
	BASEDIR+'/DB/MysqlAccess.cpp',
	BASEDIR+'/DB/MysqlConn.cpp',
    BASEDIR+'/DirPtr/DirImpl_Linux.cpp',
	BASEDIR+'/DirPtr/DirPtrImpl_Linux.cpp',
    BASEDIR+'/DyPlugin/DyPluginImpl_Linux.cpp',
	BASEDIR+'/Net/TcpServerImpl_Linux.cpp',
	BASEDIR+'/Net/TcpClientImpl_Linux.cpp',
	BASEDIR+'/Net/PostNodeMgr_Linux.cpp',
	BASEDIR+'/Net/BalanceHandle_Linux.cpp',
	BASEDIR+'/Net/AsyncServer.cpp',
	BASEDIR+'/Net/AsyncClient.cpp',
	BASEDIR+'/Net/AsyncNetServerImpl_Apple.cpp',
	BASEDIR+'/Net/AsyncNetClientImpl_Apple.cpp',
    BASEDIR+'/SharedMemory/SharedMemoryImpl_Linux.cpp',
	BASEDIR+'/SystemApi/SystemApi_Linux.cpp',
	BASEDIR+'/SystemApi/ProcessImpl_Linux.cpp',
	BASEDIR+'/SystemApi/KMInputImpl_Linux.cpp',
	BASEDIR+'/Thread/AtomMutexImpl_Linux.cpp',
	BASEDIR+'/Thread/MsTimerMgrImpl_Linux.cpp',
	BASEDIR+'/Thread/MultiThreadSync_Linux.cpp',
	BASEDIR+'/Thread/MutexIntegerImpl_Linux.cpp',
	BASEDIR+'/Thread/RWLockImpl_Linux.cpp',
	BASEDIR+'/Thread/SemaphoreImpl_Linux.cpp',
	BASEDIR+'/Thread/SemaphoreExImpl_Linux.cpp',
	BASEDIR+'/Thread/TaskCtrlImpl_Linux.cpp',
	BASEDIR+'/Thread/ThreadCtrlExImpl_Linux.cpp',
	BASEDIR+'/Thread/ThreadPoolImpl_Linux.cpp',
	BASEDIR+'/Thread/TimerMgrImpl_Linux.cpp',
]
maclib = [
    'pthread',
    'mysqlclient'
]
maccppflag = [
    '-std=c++11',
    '-I/usr/local/Cellar/mysql-client/8.0.23/include'
]
maclinkflag = [
]
maccpppath = [
]
maclibpath = [
    '/usr/local/Cellar/mysql-client/8.0.23/lib/'
]

print (BASEDIR)

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
    bStatic = 'false';
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
