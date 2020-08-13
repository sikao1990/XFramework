#include "PostgreConn.h"

DEFINENODE(PostgreConn, "PostgreConn", DBConnect, ALLOCMEM);

PostgreConn::PostgreConn()
{
    //TODO:
}

PostgreConn::~PostgreConn()
{
    //TODO:
}

bool PostgreConn::InitConnecion(void* p)
{
    //TODO:
    return true;
}

bool PostgreConn::ConnectDB()
{
    //TODO:
    return true;
}

void PostgreConn::DisConnectDB()
{
    //TODO:
}

int PostgreConn::BeginTrans()
{
    //TODO:
    return true;
}

int PostgreConn::CommitTrans(bool b)
{
    //TODO:
    return true;
}

bool PostgreConn::InitLib()
{
    //TODO:
    return true;
}

bool PostgreConn::UninitLib()
{
    //TODO:
    return true;
}
