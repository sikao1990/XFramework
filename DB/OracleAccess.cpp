#include "OracleAccess.h"

DEFINENODE(OracleAccess, "OracleAccess", DBAccess, ALLOCMEM);

OracleAccess::OracleAccess()
{
    //TODO:
}

OracleAccess::~OracleAccess()
{
    //TODO:
}

bool OracleAccess::InitAccessParam(void* p)
{
    //TODO:
    return true;
}

int OracleAccess::Execute(const char* sql)
{
    //TODO:
    return 0;
}

int OracleAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
    //TODO:
    return 0;
}

int OracleAccess::Query(const char* sql, list< list<DBField> >& result)
{
    //TODO:
    return 0;
}

int OracleAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
    //TODO:
    return 0;
}
