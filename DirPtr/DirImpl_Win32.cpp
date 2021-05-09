#include "DirImpl_Win32.h"
#include <shlwapi.h>
#include <windows.h>
#pragma comment(lib, "Shlwapi.lib")

string DirImpl::GetCurWorkDir()
{
    char szPath[MAX_PATH]={};
    GetCurrentDirectory(MAX_PATH,szPath);
    return szPath;
}

string DirImpl::GetExeFullPath()
{
    char exeFullPath[MAX_PATH]={0};
    GetModuleFileName(NULL,exeFullPath,MAX_PATH);
    return exeFullPath;//TODO
}

bool DirImpl::SetCurWorkDir(const char* path)
{
    return TRUE==SetCurrentDirectory(path);
}

bool DirImpl::CreateDir(const char* path)
{
    return TRUE==CreateDirectory(path,NULL);
}

bool DirImpl::DeleteDir(const char* path)
{
    return TRUE==RemoveDirectory(path);
}

bool DirImpl::IsFileExist(const char* strFile)
{
    return TRUE==PathFileExists(strFile);
}

bool DirImpl::MoveFile(const char* oldFile,const char* newFile)
{
    return TRUE==MoveFile(oldFile,newFile);
}

bool DirImpl::DeleteFile(const char* file)
{
    return TRUE==DeleteFile(file);
}
