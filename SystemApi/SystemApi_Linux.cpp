#include "SystemApi_Linux.h"

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>

#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

//是不是数字
static int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}

//intCaseSensitive=0大小写不敏感
static int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
	return intCaseSensitive?!strcmp(s1, s2):!strcasecmp(s1, s2);
}

//intCaseSensitive=0大小写不敏感
static int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
{
	union t{ const char* p;int ret;}tt;
	tt.p = NULL;
	tt.p = intCaseSensitive? strstr(haystack, needle):strcasestr(haystack, needle);
	return tt.ret;
}

static pid_t GetPIDbyName_implements(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
{
    char chrarry_CommandLinePath[100]  ;
    char chrarry_NameOfProcess[300]  ;
    char* chrptr_StringToCompare = NULL ;
    pid_t pid_ProcessIdentifier = (pid_t) -1 ;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;

    int (*CompareFunction) (const char*, const char*, int) ;

    if (intExactMatch)
        CompareFunction = &strcmp_Wrapper;
    else
        CompareFunction = &strstr_Wrapper;


    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return (pid_t) -2 ;
    }

    // Loop while not NULL
    while ( (de_DirEntity = readdir(dir_proc)) )
    {
        if (de_DirEntity->d_type == DT_DIR)
        {
            if (IsNumeric(de_DirEntity->d_name))
            {
                strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
                strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
                strcat(chrarry_CommandLinePath, "/cmdline") ;
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  //open the file for reading text
                if (fd_CmdLineFile)
                {
                    fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; //read from /proc/<NR>/cmdline
                    fclose(fd_CmdLineFile);  //close the file prior to exiting the routine

                    if (strrchr(chrarry_NameOfProcess, '/'))
                        chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
                    else
                        chrptr_StringToCompare = chrarry_NameOfProcess ;

                    //printf("Process name: %s\n", chrarry_NameOfProcess);
                    //这个是全路径，比如/bin/ls
                    //printf("Pure Process name: %s\n", chrptr_StringToCompare );
                    //这个是纯进程名，比如ls

                    //这里可以比较全路径名，设置为chrarry_NameOfProcess即可
                    if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
                    {
                        pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
                        closedir(dir_proc) ;
                        return pid_ProcessIdentifier ;
                    }
                }
            }
        }
    }
    closedir(dir_proc) ;
    return pid_ProcessIdentifier ;
}

unsigned SystemApi::GetProcessID(const char* CmdName)
{
	return GetPIDbyName_implements(CmdName, 0,0);//大小写不敏感
}

int SystemApi::StartDaemon(const char* workDir /*= NULL*/ )
{
    int fd = -1;
    std::string strWorkDir;
    if(NULL==workDir||0==strlen(workDir)||-1==access(workDir,X_OK))
        strWorkDir = getenv("HOME");
    else
        strWorkDir = workDir;
    if(fork()!=0) return -1;
	for(fd=0; fd<3; fd++) close(fd);
	setsid();
	umask(0022);
	chdir(strWorkDir.c_str());
    return 0;
}
