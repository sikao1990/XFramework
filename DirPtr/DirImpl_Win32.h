#ifndef __DirImpl_Win32_H__
#define __DirImpl_Win32_H__
#include <string>

using namespace std;

#define MAX_PATH (256)

class DirImpl
{
public:
    static string GetCurWorkDir();
    static string GetExeFullPath();
    static bool SetCurWorkDir(const char* path);
    static bool CreateDir(const char* path);
    static bool DeleteDir(const char* path);
    static bool IsFileExist(const char* strFile);
    static bool MoveFile(const char* oldFile,const char* newFile);
    static bool DeleteFile(const char* file);    
};

#endif
