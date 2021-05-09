#include <stdio.h>
#include "../../XPublic.h"

struct A
{
	A():m_a(1),m_b(2){}
	int m_a;
	int m_b;
};

class DataBConnPool : public ObjPool<A>
{
public:
protected:
	bool InitObj(A** ppObj,void* pParam){
		*ppObj = new A;
		puts("DataBConnPool::Init");
		return true;
	}
    void Destroy(void* p){
        A* pA = (A*)p;
        delete pA;
    }
};

int main()
{
	DataBConnPool pools;
	//ObjPool<A> pools;
	pools.Init(3);
	printf("total:%d\n",pools.Capacity());
	A* p = (A*) pools.Alloc();//0
    puts("........................");
	p->m_a = 2;
	p->m_b = 1;
	printf("%d,%d p@%p\n",p->m_a,p->m_b,p);
#if 1
    A* p1 = (A*)pools.Alloc();//1
    A* p2 = (A*)pools.Alloc();//2
    puts("begin free");
    pools.Free(p1);//1
    printf("total:%d\n",pools.Capacity());
    A* pe1 = (A*)pools.Alloc(2);//3
    printf("total:%d,pe1:%p\n",pools.Capacity(),pe1);
    pools.Free(p2);//2
    A* pe2 = (A*)pools.Alloc(2);//1
    printf("total:%d,pe2:%p\n",pools.Capacity(),pe2);
    printf("---------use:%d\n",pools.GetUsed());//0 1 2 3 4
#endif   
    return 0;
}