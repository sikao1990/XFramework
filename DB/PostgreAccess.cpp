#include "PostgreAccess.h"

DEFINENODE(PostgreAccess, "PostgreAccess", DBAccess, ALLOCMEM);

PostgreAccess::PostgreAccess()
{
    //TODO:
}

PostgreAccess::~PostgreAccess()
{
    //TODO:
}

bool PostgreAccess::InitAccessParam(void* p)
{
    //TODO:
    return true;
}

int PostgreAccess::Execute(const char* sql)
{
    //TODO:
    return 0;
}

int PostgreAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
    //TODO:
    return 0;
}

int PostgreAccess::Query(const char* sql, list< list<DBField> >& result)
{
    //TODO:
    return 0;
}

int PostgreAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
    //TODO:
    return 0;
}
