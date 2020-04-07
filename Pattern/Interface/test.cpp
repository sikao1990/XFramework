#include "FuncObj.h"
/* FuncObj.h
class XInterface;
typedef void (*funcptr)(XInterface* pObj,va_list ap);

class FuncObj{
public:
	virtual ~FuncObj();
	virtual void operator()(XInterface* pObj,va_list ap);
};
*/

#include "XInterface.h"
/* XInterface.h
typedef enum emFuncType
{
	emFunObj,
	emGloablF,
}emFuncType;
class XInterface{
public:
	virtual ~XInterface();
	virtual void func(emFuncType type,void* fun,...);
};
*/
//组合代理XInterface访问
//XInterface派生类提供扩展方法(接口),在FuncObj派生类中使用XInterface方法或接口实现业务逻辑
//将参数封装为信使(类)->建立架构(组件/插件式)
int main()
{
	XInterface* test1=new XInterface;//亦可将type,void* 作为构造函数参数，进行传递
	
	//不同的参数(函数对象、全局函数),实现的逻辑(功能)不同
	test1->func(emFunObj,new FuncObj);//参数以 void* 提供,可实现组建化接口
	test1->func(emGloablF,reinterpret_cast<void* >(globelFunc));

	return 0;
}
