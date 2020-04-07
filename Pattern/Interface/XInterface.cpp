#include "XInterface.h"
#include "FuncObj.h"

XInterface::~XInterface()
{
	
}

void XInterface::func(emFuncType type,void* fun,...){
	va_list ap;
	va_start(ap,fun);
	switch(type){
	case emFunObj:
		{
			FuncObj* fobj = reinterpret_cast<FuncObj*>(fun);
			(*fobj)(this, ap);
		}
		break;
	case emGloablF:
		{
			funcptr funp = reinterpret_cast<funcptr>(fun);
			(*funp)(this, ap);
		}
		break;
	}
	va_end(ap);
}
