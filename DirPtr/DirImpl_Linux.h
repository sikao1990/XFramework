#ifndef __DirImpl_Linux_H__
#define __DirImpl_Linux_H__
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

#define MAX_PATH (256)

using namespace std;

class DirImpl
{
public:
    static string GetCurWorkDir();
    static string GetExeFullPath();
    static bool SetCurWorkDir(const char* path);
    static bool CreateDir(const char* path,mode_t mask=0777);
    static bool DeleteDir(const char* path);
    static bool IsFileExist(const char* strFile);
    static bool MoveFile(const char* oldFile,const char* newFile);
    static bool DeleteFile(const char* file);    
};

#endif
