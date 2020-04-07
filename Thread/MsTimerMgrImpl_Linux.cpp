#include "MsTimerMgrImpl_Linux.h"

MsTimerMgrImpl* MsTimerMgrImpl::pThis = NULL;

MsTimerMgrImpl::MsTimerMgrImpl():m_pools(2),bFlagOnce(0),bFlagRepeat(0){
}

MsTimerMgrImpl::~MsTimerMgrImpl(){
	Stop();
	sem_destroy(&m_SemOnce);
	sem_destroy(&m_SemRepeat);
	pthread_mutex_destroy(&m_OnceLock);
	pthread_rwlock_destroy(&m_Repeatlock);
}

bool MsTimerMgrImpl::Init(){
	if(NULL!=pThis)return false;
	signal(SIGALRM, &MsTimerMgrImpl::sigHandle);
	pthread_mutex_init(&m_OnceLock,NULL);
	pthread_rwlock_init(&m_Repeatlock,NULL);
	pThis=this;
	if(sem_init(&m_SemOnce,0,0))
		return false;
	if(sem_init(&m_SemRepeat,0,0))
		return false;			

	m_pools.init_pool();
	m_pOncePrecess=new OnceProcess(this);
	m_pRepeatProcess=new RepeatProcess(this);
	m_pools.Register_taskProcess("0",m_pOncePrecess);
	m_pools.Register_taskProcess("1",m_pRepeatProcess);
	
	Task* t0 = new Task("0");
	Task* t1 = new Task("1");
	m_pools.submit_job_req(t0);
	m_pools.submit_job_req(t1);
	
	struct itimerval tick;
	tick.it_value.tv_sec = 0;
	tick.it_value.tv_usec = 1000;
	tick.it_interval.tv_sec = 0;
	tick.it_interval.tv_usec = 1000;
	setitimer(ITIMER_REAL, &tick, NULL);
	return true;
}

bool MsTimerMgrImpl::AddTimer(const char* str,AbstractTimer* timer){
	bool bRes = false;
	if(NULL==timer)
		return false;
	timer->SetStartTime();
	if(timer->GetInterval()){
		pthread_rwlock_wrlock(&m_Repeatlock);
		bRes = repeatSet.insert(make_pair(str,timer)).second;
		pthread_rwlock_unlock(&m_Repeatlock);		
	}else{
		pthread_mutex_lock(&m_OnceLock);
		onceLst.push_back(timer);
		pthread_mutex_unlock(&m_OnceLock);
		bRes=true;		
	}
	return bRes;
}

bool MsTimerMgrImpl::DelTimer(const char* str){
	pthread_rwlock_wrlock(&m_Repeatlock);
	if(repeatSet.find(str)!=repeatSet.end()){
		delete repeatSet.find(str)->second;
		repeatSet.erase(repeatSet.find(str));
		pthread_rwlock_unlock(&m_Repeatlock);
		return true;
	}
	pthread_rwlock_unlock(&m_Repeatlock);
	return false;
}

bool MsTimerMgrImpl::ModifyRepeatTimer(const char* str,int len,int nInterval){
	pthread_rwlock_wrlock(&m_Repeatlock);
	if(repeatSet.find(str)!=repeatSet.end()){
		repeatSet.find(str)->second->SetTimer(len,nInterval);
		pthread_rwlock_unlock(&m_Repeatlock);
		return true;
	}	
	pthread_rwlock_unlock(&m_Repeatlock);
	return false;
}

void MsTimerMgrImpl::Stop(){
	if(NULL==pThis)return;
	struct itimerval tick;
	tick.it_value.tv_sec = 0;
	tick.it_value.tv_usec = 0;
	tick.it_interval.tv_sec = 0;
	tick.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &tick, NULL);
	m_pools.stop_pool();
	m_pOncePrecess->stop();
	m_pRepeatProcess->stop();

	list<AbstractTimer*>::iterator it;
	pthread_mutex_lock(&m_OnceLock);
	for(it=onceLst.begin();it!=onceLst.end();it++){
		delete *it;
	}
	pthread_mutex_unlock(&m_OnceLock);
	
	map<string,AbstractTimer*>::iterator itr;
	pthread_rwlock_wrlock(&m_Repeatlock);
	for(itr=repeatSet.begin();itr!=repeatSet.end();itr++){
		delete itr->second;
	}
	pthread_rwlock_unlock(&m_Repeatlock);
	pThis = NULL;
}

void MsTimerMgrImpl::WaitOnce(){
	sem_wait(&m_SemOnce);
}

void MsTimerMgrImpl::DealOnceHandle(){
	int index = 0;
	vector<int>	tmpVec;
	list<AbstractTimer*>::iterator it;
	pthread_mutex_lock(&m_OnceLock);
	for(it=onceLst.begin();it!=onceLst.end();it++,index++){
		if((*it)->IsTimeout())
		{
			tmpVec.push_back(index);
			(*it)->TimerProc();
		}
	}
	for(int i=tmpVec.size()-1;i>=0;i--){
		it=onceLst.begin();
		advance(it, tmpVec[i]);
		delete *it;
		onceLst.erase(it);
	}
	pthread_mutex_unlock(&m_OnceLock);	
}

void MsTimerMgrImpl::WaitRepeat(){
	sem_wait(&m_SemRepeat);
}

void MsTimerMgrImpl::DealRepeatHandle(){
	pthread_rwlock_rdlock(&m_Repeatlock);
	for(map<string,AbstractTimer*>::iterator it=repeatSet.begin();it!=repeatSet.end();it++){
		if(it->second->IsTimeout()){
			it->second->TimerProc();
			it->second->ResetState();
		}
	}
	pthread_rwlock_unlock(&m_Repeatlock);
}

void MsTimerMgrImpl::TimerHandle(){//定时器处理函数
	bFlagRepeat = bFlagOnce = 0;
	pthread_mutex_lock(&m_OnceLock);
	for(itOnce=onceLst.begin();itOnce!=onceLst.end();itOnce++){
		if((*itOnce)->GetStart() == (*itOnce)->Increment()){
			(*itOnce)->SetFinish();
			bFlagOnce = 1;
		}
	}
	if(bFlagOnce)sem_post(&m_SemOnce);
	pthread_mutex_unlock(&m_OnceLock);
	
	pthread_rwlock_rdlock(&m_Repeatlock);
	for(itRepeat=repeatSet.begin();itRepeat!=repeatSet.end();itRepeat++){
		switch(itRepeat->second->GetState()){
		case 0://第一次时间到
			if(itRepeat->second->GetStart() == itRepeat->second->Increment()){
				itRepeat->second->SetFinish();
				itRepeat->second->RestCount();
				bFlagRepeat = 1;
			}
			break;
		case 1://重复定时器重复时间未到状态
			if(itRepeat->second->GetInterval() == itRepeat->second->Increment()){
				itRepeat->second->RestCount();
				itRepeat->second->SetFinish();
				bFlagRepeat = 1;
			}
			break;			
		case 2:
			break;
		}
	}
	if(bFlagRepeat)sem_post(&m_SemRepeat);
	pthread_rwlock_unlock(&m_Repeatlock);
}

void MsTimerMgrImpl::sigHandle(int sig){
	if(NULL==pThis)return;
	pThis->TimerHandle();
}

void MsTimerMgrImpl::WakeOne()
{
	sem_post(&m_SemOnce);
}

void MsTimerMgrImpl::WakeRepeat()
{
	sem_post(&m_SemRepeat);
}

