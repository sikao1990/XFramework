#include <stdio.h>
#include <iostream>
#include "../../XPublic.h"

using namespace std;

class Object
{
public:
    Object(){
        puts("Object()");
    }
    virtual ~Object(){
        puts("~Object()");
    }
    void show(){
        puts("hello world");
    }
    void* operator new(size_t size)
    {
        void* p = NULL;
        p = ::operator new(size);
        //p = malloc(size);
        printf("Object::operator new with:%p\n",p);
        return p;
    }
     
    void operator delete(void* p)
    {
        printf("Object::operator delete:%p\n",p);
        ::operator delete(p);
        //free(p);
    }
     
    void* operator new[](size_t size)
    {
        puts("Object::operator new[] with size:");
        return malloc(size);
    }
     
    void operator delete[](void* p)
    {
        puts("Object::operator delete[]");
        free(p);
    }
};

class C
{
public:
    C(){
        puts("C()");
    }
    ~C(){
        puts("~C()");
    }
    int x;
};

class B;
class A : public Object
{
public:
    virtual ~A(){
        puts("~A===============>");
    }
	RefPtr<B> spa;
    //C mc;
};
class B : public Object
{
public:
    virtual ~B(){
        puts("~B===============>");
    }
	RefPtr<A> spb;
    //C mc;
};

int test(){
#if 0
    list< RefPtr<Object> > lst;
    lst.push_back( new Object() );
    lst.push_back( new Object() );
    lst.push_back( new Object() );

    lst.clear(); 
    puts("=---------------------=");
#if 1   
    RefPtr<Object> pObj = new Object();
    if(NULL!=pObj){
        pObj->show();
        puts("NULL != RefPtr");
    }
    if(pObj!=NULL){
        puts("RefPtr != NULL");
        (*pObj).show();
    }
    RefPtr<Object> pObject; 
    if(NULL==pObject)
        puts("NULL == pObj");
    else
        puts("NULL != pObj");
    if(pObject==NULL)
        puts("pObj == NULL");
    else
        puts("pObj != pObj");
#endif    
#endif 
    printf("sizeof A:%d,sizeof B %d sizeof Ref:%d\n",sizeof(A),sizeof(B),sizeof(RefPtr<A>));
puts("=---------------------=");
#if 1
    RefPtr<A> pa(new A());//生成新对象的时候给A的引用计数+1
	RefPtr<B> pb(new B());//生成新对象的时候给B的引用计数+1
    pa.EnableLoopRefMode(/*true*/);
    pb.EnableLoopRefMode(/*true*/);
	pa->spa = pb;//保存一个指针的时候B引用计数又+1
	pb->spb = pa;//保存一个指针的时候A引用计数又+1    
#endif

#if 0
    A* pA = new A();
    B* pB = new B();
    delete pA;
    delete pB;
    C c;
#endif
}

int main()
{
    test();
    puts("----------------end-----------------");
    return 0;
}