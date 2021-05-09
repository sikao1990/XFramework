#ifndef __RefPtr_H__
#define __RefPtr_H__
#include <stdio.h>

#include <new>
#include <set>
#include <map>
#include <stdlib.h>
//TODO:unfinished
class Ref{
public:
    void AddRef(void* pMem,void* pObj){
        if(NULL==pMem||NULL==pObj)return;
        std::map<void*,std::set<void*> >::iterator itf = m_refMaps.find(pMem);
        if(m_refMaps.end()==itf){
            m_refMaps.insert(std::make_pair(pMem,std::set<void*>()));
            m_refInfo.insert(std::make_pair(pMem,pObj));
        }
        m_refMaps.find(pMem)->second.insert(pObj);
    }
    void DelRef(void* pMem,void* pObj){
        if(NULL==pMem||NULL==pObj)return;
        std::map<void*,std::set<void*> >::iterator itf = m_refMaps.find(pMem);
        if(m_refMaps.end()!=itf){
            std::set<void*>& tagPtrSet = itf->second;
            //printf("pMem:%p,pObj:%p size:%d\n",pMem,pObj,tagPtrSet.size());
            if( tagPtrSet.end() != tagPtrSet.find(pObj) ){
                tagPtrSet.erase(pObj);
                //printf("-------%d\n",tagPtrSet.size());
                if(0==tagPtrSet.size())
                    m_refMaps.erase( itf );
            }
        }
    }
    void ClearTagRef(void* pMem){
        if(m_refMaps.end()==m_refMaps.find(pMem))
            m_refMaps.erase( m_refMaps.find(pMem) );
    }
    int GetRefCount(void* pMem){
        if(NULL==pMem)return 0;
        if(m_refMaps.end()==m_refMaps.find(pMem))return 0;
        return m_refMaps.find(pMem)->second.size();
    }
    void* GetOwner(void* pMem){
        if(m_refInfo.end()==m_refInfo.find(pMem))return NULL;
        return m_refInfo.find(pMem)->second;
    }
    void RLock(){}
    void RUnlock(){}
    void WLock(){}
    void WUnlock(){}
    std::map<void*,std::set<void*> >& getMapInfos(){ return m_refMaps; }
private:
    std::map<void*,std::set<void*> > m_refMaps;
    std::map<void*,void*> m_refInfo;
};

template< typename T >
class RefPtr
{
public:
    ~RefPtr(){
        if(_bLoopRef){
            //1 如果是强引用RefPtr，则直接释放内存并通知所有弱引用RefPtr
            if(NULL==_fromRef&&!_bHasFree&&NULL!=_pData){
                NotifyFree();
                //g_pAllocator->Free(_pData);
                delete _pData;
                _pData = NULL;
            }//2 如果是弱引用则删除记录
            else if(NULL!=_fromRef){
                _ref.DelRef(_pData,this);
                _fromRef=NULL;
                _pData = NULL;
            }
        }else{//非循环引用用途模式
            _ref.DelRef(_pData,this);
            if(0==_ref.GetRefCount(_pData)){
                //g_pAllocator->Free(_pData);
                delete _pData;
            }
            _pData = NULL;   
        }
    }
    RefPtr():_pData(NULL),_fromRef(NULL),_bHasFree(false),_bLoopRef(false){}
    //循环引用用途模式下，默认为强引用
    RefPtr(const T* p):_pData(NULL),_fromRef(NULL),_bHasFree(false),_bLoopRef(false){
        if(NULL!=p){
            _ref.AddRef(_pData = const_cast<T*>(p),this);
            //printf("------------pData:%p,this:%p\n",_pData,this);
        }
    }
    //循环引用用途模式下，默认为强引用    
    RefPtr(const void* p):_pData(NULL),_fromRef(NULL),_bHasFree(false),_bLoopRef(false){
        if(NULL!=p){
            _ref.AddRef(_pData = const_cast<T*>(p),this);
        }
    }
    RefPtr(const RefPtr& that){
        if(NULL!=(this->_pData=that._pData)){
            //判断源是弱还是强类型
            NULL!=that._fromRef?this->_fromRef = that._fromRef:this->_fromRef =(RefPtr<T>*)(&that);
            this->_bHasFree = that._bHasFree;
            this->_bLoopRef = that._bLoopRef;
            this->_ref.AddRef(_pData,this);
        }
    }
    RefPtr& operator=(const RefPtr& that){
        if(this == &that)return *this;
        if(_bLoopRef){
            //1 赋值给强类型RefPtr
            if(NULL==_fromRef&&!_bHasFree&&NULL!=_pData){
                NotifyFree();
                //g_pAllocator->Free(_pData);
                delete _pData;
                _pData = NULL;
            }//2 赋值给弱类型
            else if(NULL!=_fromRef&&NULL!=_pData){
                _ref.DelRef(_pData,this);
                _fromRef=NULL;
                _pData = NULL;
                _bHasFree=false;
            }
        }else{
            _ref.DelRef(_pData,this);
            _pData = NULL;
        }
        if(NULL!=that._pData){
            //判断源是弱还是强类型
            NULL!=that._fromRef?this->_fromRef = that._fromRef:this->_fromRef =(RefPtr<T>*)(&that);
            _ref.AddRef(_pData = that._pData,this);
            this->_bHasFree = that._bHasFree;
            this->_bLoopRef = that._bLoopRef;
        }
        return *this;
    }
    //启用循环引用模式，
    void EnableLoopRefMode(){
        _bLoopRef = true;
    }
    bool isLoopRefMode()const{ return _bLoopRef;}
    T* GetPtr()const{
        return _pData; 
    }
    //Release后，置空 
    T* Release()const{
        T* pTag = _pData;
        RefPtr<T>* pRef = GetOwner(_pData);
        if(NULL != pRef){
            _ref.DelRef(_pData,pRef);
            pRef->_pData = NULL;
        }
        NotifyFree();
        _pData = NULL;
        return pTag;
    }
    RefPtr<T>& MoveFrom(RefPtr<T>& that){
        
    }
    RefPtr<T>& CloneFrom(RefPtr<T>& that){
        
    }
    T& operator*(){
		//TODO:_bHasFree
        return *_pData;
    }
    T* operator->(){
		//TODO:_bHasFree
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
        return _pData == that._pData;
    }
    bool operator!=(const void* p)const{
        return p != _pData;
    }
    bool operator!=(const RefPtr<T>& that)const{
        return _pData != that._pData;
    }
    friend bool operator!=(const void* p,const RefPtr<T>& that){
        return that._pData!=p;
    }
    friend bool operator==(const void* p,const RefPtr<T>& that){
        return that._pData==p;
    }
private:
    void NotifyFree(){
        //_ref.WLock();
        std::map<void*,std::set<void*> >& mapRef = _ref.getMapInfos();
        std::map<void*,std::set<void*> >::iterator itf = mapRef.find(_pData);
        if(mapRef.end()!=itf){
            std::set<void*>& refSet = itf->second;
            if(refSet.size()>0)
            {
                for(std::set<void*>::iterator it=refSet.begin();it!=refSet.end();it++)
                {
                    RefPtr<T>* pTObj = (RefPtr<T>*)(*it);
                    pTObj->_bHasFree = true;//通知源(强引用)已释放
                }
            }
            mapRef.erase(itf);
        }
        //_ref.WUnlock();
    }
private:
    bool        _bHasFree;//源RefPtr是否被释放
    bool        _bLoopRef;
    mutable T*  _pData;
    RefPtr<T>*  _fromRef;
    static Ref  _ref;
    //static GC*  g_pAllocator;
};
template< typename T >
Ref RefPtr<T>::_ref;

#endif // RefPtr.h
