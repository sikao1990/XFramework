#ifndef __RefPtr_H__
#define __RefPtr_H__
#include <stdio.h>

#include <new>
#include <list>
#include <stdlib.h>

template< typename T >
class RefPtr
{
public:
    RefPtr():_pData(0){}
    ~RefPtr(){
        _ref.DelDef();
        if(!_ref.GetRefCount() && NULL!=_pData ){
            //g_pAllocator->Free(_pData);
            delete _pData;
        }
    }
    RefPtr(const T* p):_pData(NULL){
        if(NULL!=p){
            _pData = const_cast<T*>(p);
            _ref.AddRef();
        }
    }    
    RefPtr(const void* p):_pData(NULL){
        if(NULL!=p){
            _pData = const_cast<T*>(p);
            _ref.AddRef();
        }
    }
    RefPtr(const RefPtr& that){
        this->_pData = that._pData;
        if(NULL!=_pData){
            this->_ref.AddRef();
            that._ref.AddRef();
        }
    }
    RefPtr& operator=(const RefPtr& that){
        if(NULL!=_pData){
            _ref.DelRef();
            if(!_ref.GetRefCount() && NULL!=_pData ){
                //g_pAllocator->Free(_pData);
                free(_pData);
            }
            _pData = NULL;
        }
        if(NULL!=that._pData){
            this->_pData = that._pData;
            this->_ref.AddRef();
            that._ref.AddRef();
        }
        return *this;
    }
    T* GetPtr()const{ 
        return _pData; 
    }
    T& operator*(){
        return *_pData;
    }
    T* operator->(){
        return _pData;
    }
    T* operator->()const{
        return _pData;
    }
    bool operator!()const{
        return NULL == _pData;
    }
    bool operator==(const void* p)const{
        return p == _pData;
    }
    bool operator==(const RefPtr<T>& that)const{
        return p == that._pData;
    }
    bool operator!=(const void* p)const{
        return p != _pData;
    }
    bool operator!=(const RefPtr<T>& that)const{
        return p != that._pData;
    }
    friend bool operator!=(const void* p,const RefPtr<T>& that){
        return that!=p;
    }
    friend bool operator==(const void* p,const RefPtr<T>& that){
        return that==p;
    }
private:
    Ref         _ref;
    mutable T*  _pData;
    static GC*  g_pAllocator;
};
#if 0 // test code
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
void Test()
{
    list< RefPtr<Object> > lst;
    lst.push_back( new Object() );
    lst.push_back( new Object() );
    lst.push_back( new Object() );

    lst.clear(); 
    
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
}
#endif // test code

#endif // RefPtr.h
