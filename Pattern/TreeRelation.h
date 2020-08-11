/*----------------------------------------------------------------
// 文件名：TreeReleation.h
// 文件功能描述：封装创建任意派生类对象的接口
// 创建者:zhuliwu
// 时间：2018/9/27 14:56:32
// 版本：V1.0.0
//----------------------------------------------------------------*/

#ifndef __TreeRelation_H__
#define __TreeRelation_H__
#include <list>
#include <iostream>
#include <assert.h>

typedef void* (*GetInstance)(void*);
//TODO:
#define ALLOCMEM(type)		(new type)

#define CONSTRUCTNOALLOC(pStart,type)	(new (pStart)type)
#define DESTRUCTNOFREE(pStart,type)	(pStart->~type())

#define NODEADDRES(type)  ( (TreeRelation*)&(type::s_##type) )

#define DECLARENODE(type)				\
	public:								\
	static TreeRelation s_##type;		\
	static TreeRelation* getParent();	\
	static void*Create##type##Instance(void* p)	

#define DEFINENODE(type,kind,parent,AllocMem) 		\
	void* type::Create##type##Instance(void* p){	\
		return AllocMem(type);					\
	}										\
	TreeRelation* type::getParent(){		\
		return s_##type.st_parent;			\
	}										\
	TreeRelation type::s_##type=TreeRelation(&type::Create##type##Instance,kind,NODEADDRES(parent))	


#define AddToParent(BaseClass,child)	\
	BaseClass::AddChild(child::getParent()->st_type,NODEADDRES(child))

/*	arg(string,string) for plugin by so(dll)*/
#define AddToParentX(BaseClass,parentType,NodeAddress)	\
	BaseClass::AddChild(parentType,NodeAddress)	


#define RelationCmp(BaseClass,typex,typey)	\
	BaseClass::relationcmp(NODEADDRES(typex)->st_type,NODEADDRES(typey)->st_type)

/*	arg(string,string) for plugin by so(dll)*/
#define RelationCmpX(BaseClass,typex,typey)	\
	BaseClass::relationcmp(typex,typey)

#define DECLAREBASEMETHOD(BaseClass)	\
	private:							\
		static int	s_children;			\
	public:								\
		static BaseClass* getInstance(const std::string& type,void*p);				\
		static int relationcmp(const std::string& type,const std::string& typed);	\
		static bool AddChild(const std::string& parenttype,TreeRelation*leaf);		\
		static int removeChild(const std::string& type);							\
		static void clearTreeReleation();											\
		static int getChildrenCount();												\
	private:																		\
		static void clearReleation(TreeRelation* root);								\
		static TreeRelation* findNode(const std::string& type,TreeRelation* root)

#define DEFINEBASEMETHOD(BaseClass)	\
	BaseClass* BaseClass::getInstance(const std::string& type,void* p){		\
		TreeRelation* node=findNode(type,NODEADDRES(BaseClass));	\
		if(node){													\
			return reinterpret_cast<BaseClass*>(node->st_funcptr(p)); \
		}else{														\
			return NULL;											\
		}															\
	}																\
	bool BaseClass::AddChild(const std::string& parenttype,TreeRelation* leaf){	\
		TreeRelation* node=findNode(parenttype,NODEADDRES(BaseClass));		\
		if(node){															\
			node->st_children.push_back(leaf);								\
			s_children++;													\
			return true;													\
		}else{																\
			return false;													\
		}																	\
	}																		\
	TreeRelation* BaseClass::findNode(const std::string& type,TreeRelation* root){	\
		assert(type.size()&&NULL!=root);									\
		if(type==root->st_type){											\
			return root;													\
		}else{ 																\
			std::list<TreeRelation*>::iterator it=root->st_children.begin();\
			for(;it!=root->st_children.end();it++){							\
				TreeRelation* ptr=findNode(type,(*it));						\
				if(ptr)														\
					return ptr;												\
			}																\
			return NULL;													\
		}																	\
	}																		\
	int BaseClass::relationcmp(const std::string& type,const std::string& typed){\
		TreeRelation* ptrtype=findNode(type,NODEADDRES(BaseClass));			\
		TreeRelation* ptrtyped=findNode(typed,NODEADDRES(BaseClass));		\
		TreeRelation* tmptr=NULL;											\
		if(ptrtype&&ptrtyped){												\
			if( ptrtype->st_parent==ptrtyped->st_parent || type==typed )	\
				return 0;													\
			else{															\
				tmptr=findNode(typed,ptrtype);								\
				if(tmptr)													\
					if(tmptr->st_parent->st_type==type)						\
						return 1;											\
					else													\
						return 1+relationcmp(tmptr->st_parent->st_type,typed);\
				tmptr=NULL;													\
				tmptr=findNode(type,ptrtyped);								\
				if(tmptr)													\
					if(tmptr->st_parent->st_type==typed)					\
						return -1;											\
					else													\
						return -1+relationcmp(type,tmptr->st_parent->st_type);\
			}																\
		}else																\
			throw "The Arg type was no't in relation Tree";					\
		return -2;															\
	}																		\
	int BaseClass::removeChild(const std::string& type){					\
		assert(!type.size());												\
		if(type==NODEADDRES(BaseClass)->st_type)							\
			return 2;														\
		TreeRelation* ptype=findNode(type,NODEADDRES(BaseClass));			\
		if(ptype){														\
			if(!ptype->st_children.size()){								\
				ptype->st_parent->st_children.remove(ptype);			\
				s_children--;											\
				return 0;												\
			}else														\
				return 1;												\
		}else															\
			return -1;													\
	}																		\
	void BaseClass::clearReleation(TreeRelation* root){						\
		if(0==root->st_children.size()){									\
			if(root==NODEADDRES(BaseClass))									\
				return;														\
			root->st_parent->st_children.remove(root);						\
			return;															\
		}else{																\
			std::list<TreeRelation*>::iterator it=root->st_children.begin();\
			for(;it!=root->st_children.end();it++)							\
				clearReleation(*it);										\
			s_children=1;													\
		}																	\
	}																		\
	void BaseClass::clearTreeReleation(){									\
		clearReleation(NODEADDRES(BaseClass));								\
	}																		\
	int BaseClass::getChildrenCount(){ return s_children; }					\
	int BaseClass::s_children=1

struct TreeRelation 				{
	std::string 			st_type;
	GetInstance				st_funcptr;
	TreeRelation*			st_parent;
	std::list<TreeRelation*>	st_children;	
public:
	TreeRelation(GetInstance ptr,const std::string& type,TreeRelation*parent=NULL)
		:st_funcptr(ptr),st_type(type),st_parent(parent){
		if(st_parent==this)
			st_parent=NULL;
	}
};

#endif