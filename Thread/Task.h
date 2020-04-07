#ifndef __Task_H__
#define __Task_H__
#include <iostream>

typedef std::string jobtype;
//typedef int jobtype;

#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API Task
#else
class Task
#endif
{
public:
	//0 异步(不通知)�? 同步返回
	Task(const jobtype& type,int flag=0):m_bFinish(false),m_type(type),m_nFlag(flag), m_Error(0){}
	virtual ~Task(){}
public:
	virtual int wait() { return 0; }
	virtual void done(int n) {}
	//覆盖的tCompare接口通常作为函数参数，用于任务池查找同类子类型或不同个体任务
	virtual bool tCompare(jobtype,const Task* pTask)const{ return false; }
protected:
	unsigned char	m_nFlag;
	unsigned char 	m_bFinish;
	unsigned char 	m_Error;//0 成功�? 用户取消，其它用户自定义
public:	
	jobtype 		m_type;
};

typedef bool (Task::*TCompare)(jobtype,const Task*)const;
class TaskTypeCompare {
public://用于查找同类型级同类的不同子类或不同个体
	TaskTypeCompare(jobtype t,Task* pTask=NULL,TCompare f=NULL) :m_type(t), m_New(pTask),m_func(f) {}
	bool operator()(const Task* other)const {
		return NULL!=m_func&&NULL!=m_New?(other->*m_func)(m_type, m_New):other->m_type==m_type;
	}
private:
	jobtype		m_type;
	TCompare 	m_func;
	Task*		m_New;
};
#endif	// Task.h
