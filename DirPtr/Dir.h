#ifndef __Dir_H__
#define __Dir_H__
#include <iostream>
#include <string>

using namespace std;

class Dir
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

#endif  //Dir.h
