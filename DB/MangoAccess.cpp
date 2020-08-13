#include "MangoAccess.h"

DEFINENODE(MangoAccess, "MangoAccess", DBAccess, ALLOCMEM);

MangoAccess::MangoAccess()
{
    //TODO:
}

MangoAccess::~MangoAccess()
{
    //TODO:
}

bool MangoAccess::InitAccessParam(void* p)
{
    //TODO:
    return true;
}

int MangoAccess::Execute(const char* sql)
{
    //TODO:
    return 0;
}

int MangoAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
    //TODO:
    return 0;
}

int MangoAccess::Query(const char* sql, list< list<DBField> >& result)
{
    //TODO:
    return 0;
}

int MangoAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
    //TODO:
    return 0;
}
