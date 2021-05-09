#include "DirImpl_Linux.h"
#include <unistd.h>
#include <stdio.h>

string DirImpl::GetCurWorkDir()
{
    char szPath[MAX_PATH]={};
    getcwd(szPath,MAX_PATH);
    return szPath;
}

string DirImpl::GetExeFullPath()
{
    char result[MAX_PATH]={};
    ssize_t count = readlink( "/proc/self/exe", result, MAX_PATH );
    return std::string( result, (count > 0) ? count : 0 );
}

bool DirImpl::SetCurWorkDir(const char* path)
{
    return 0==chdir(path);
}

bool DirImpl::CreateDir(const char* path,mode_t mask)
{
    return 0==mkdir(path,mask);
}

bool DirImpl::DeleteDir(const char* path)
{
    return 0==rmdir(path);
}

bool DirImpl::IsFileExist(const char* strFile)
{
    return 0==access(strFile,X_OK);
}

bool DirImpl::MoveFile(const char* oldFile,const char* newFile)
{
    return 0==rename(oldFile,newFile);
}

bool DirImpl::DeleteFile(const char* file)
{
    return 0==remove(file);
}
