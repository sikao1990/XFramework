#include "ProcessEvent_Linux.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

ProcessEvent::ProcessEvent():m_id(-1)
{
    
}

ProcessEvent::~ProcessEvent()
{
    
}

bool ProcessEvent::CreateEvent(const char* strName,int mode)
{
    if(-1!=m_id)return false;
    key_t key;
    key = ftok(strName,mode);
    m_id = semget(key,1,mode|IPC_CREAT);
    if(-1==m_id){
        return false;
    }
    semctl(m_id,0,SETVAL,0);
    return m_id != -1;
}

bool ProcessEvent::OpenEvent(const char* strName,int mode)
{
    printf("OpenEvent mode:%o\n",mode);
    if(-1!=m_id)return false;
    key_t key;
    key = ftok(strName,mode);
    m_id = semget(key,1,mode);
    return m_id != -1; 
}

void ProcessEvent::Wait()
{
    if(-1==m_id)return;
    struct sembuf ops[1] ={
        {.sem_num=0,.sem_op=-1,.sem_flg=SEM_UNDO}
    };
    semop(m_id,ops,1);
}

void ProcessEvent::Post()
{
    if(-1==m_id)return;
    struct sembuf ops[1] ={
        {.sem_num=0,.sem_op=1,.sem_flg=SEM_UNDO}
    };
    semop(m_id,ops,1);
}

void ProcessEvent::DeleteEvent()
{
    if(-1==m_id)return;
    if(-1!=m_id)semctl(m_id,0,IPC_RMID,NULL);
    m_id = -1;
}
