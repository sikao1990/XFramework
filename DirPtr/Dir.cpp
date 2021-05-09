#include "Dir.h"

#ifdef WIN32
#include "DirImpl_Win32.h"
#else
#include "DirImpl_Linux.h"
#endif

string Dir::GetCurWorkDir()
{
    return DirImpl::GetCurWorkDir();
}

string Dir::GetExeFullPath()
{
    return DirImpl::GetExeFullPath();
}

bool Dir::SetCurWorkDir(const char* path)
{
    return DirImpl::SetCurWorkDir(path);
}

bool Dir::CreateDir(const char* path)
{
    return DirImpl::CreateDir(path);
}

bool Dir::DeleteDir(const char* path)
{
    return DirImpl::DeleteDir(path);
}

bool Dir::IsFileExist(const char* strFile)
{
    return DirImpl::IsFileExist(strFile);
}

bool Dir::MoveFile(const char* oldFile,const char* newFile)
{
    return DirImpl::MoveFile(oldFile,newFile);
}

bool Dir::DeleteFile(const char* file)
{
    return DirImpl::DeleteFile(file);
}
