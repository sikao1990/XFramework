#include "ODBCAccess.h"

DEFINENODE(ODBCAccess, "ODBCAccess", DBAccess, ALLOCMEM);

ODBCAccess::ODBCAccess()
{
    //TODO:
}

ODBCAccess::~ODBCAccess()
{
    //TODO:
}

bool ODBCAccess::InitAccessParam(void* p)
{
    //TODO:
    return true;
}

int ODBCAccess::Execute(const char* sql)
{
    //TODO:
    return 0;
}

int ODBCAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
    //TODO:
    return 0;
}

int ODBCAccess::Query(const char* sql, list< list<DBField> >& result)
{
    //TODO:
    return 0;
}

int ODBCAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
    //TODO:
    return 0;
}
