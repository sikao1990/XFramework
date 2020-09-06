
ProcessEvent::ProcessEvent():m_hHandle(NULL),m_bWait(true),m_bManual(false)
{
    
}

ProcessEvent::~ProcessEvent()
{
    if(NULL!=m_hHandle)CloseHandle(m_hHandle);
}

bool ProcessEvent::CreateEvent(const char* strName,bool bManual)
{
    if(NULL!=m_hHandle)return false;
    m_bWait = true;
    m_bManual = bManual;
    return NULL!=(m_hHandle=CreateEvent(NULL,bManual?FALSE:TRUE, FALSE,strName));
}

bool ProcessEvent::OpenEvent(const char* strName)
{
    if(NULL!=m_hHandle)return false;
    m_bWait = false;
    return NULL!=(m_hHandle=OpenEvent(EVENT_ALL_ACCESS,NULL,strName));
}

void ProcessEvent::Wait()
{
    if(NULL!=m_hHandle&&m_bWait) WaitForSingleObject(m_hHandle,INFINITE);
}

void ProcessEvent::Post()
{
    if(NULL!=m_hHandle&&!m_bWait)SetEvent(m_hHandle);
}

void ProcessEvent::ManualReset()
{
    if(NULL!=m_hHandle&&m_bManual)ResetEvent(m_hHandle);
}
