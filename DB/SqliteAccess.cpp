#include "SqliteAccess.h"

DEFINENODE(SqliteAccess, "SqliteAccess", DBAccess, ALLOCMEM);

SqliteAccess::SqliteAccess()
{
    //TODO:
}

SqliteAccess::~SqliteAccess()
{
    //TODO:
}

bool SqliteAccess::InitAccessParam(void* p)
{
    //TODO:
    return true;
}

int SqliteAccess::Execute(const char* sql)
{
    //TODO:
    return 0;
}

int SqliteAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
    //TODO:
    return 0;
}

int SqliteAccess::Query(const char* sql, list< list<DBField> >& result)
{
    //TODO:
    return 0;
}

int SqliteAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
    //TODO:
    return 0;
}
