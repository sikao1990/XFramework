#include <stdio.h>
#include "../../XPublic.h"

class MyTask : public Task
{
public:
	MyTask(const jobtype& type, int flag = 0) :Task(type, flag) { m_data = 0; }
	virtual ~MyTask() {}
public:
	virtual int wait() {
		return 0;
	}
	virtual void done(int n) {
	}
	void SetData(int n) {
		m_data = n;
	}
	void ShowData() {
		printf("ShowData %d\n",m_data);
	}
private:
	int m_data;
};
class MyTaskProcess : public TaskProcess
{
public:
	MyTaskProcess(TaskCtrl* pCtrl):TaskProcess(pCtrl){}
	virtual ~MyTaskProcess() {}
	virtual void operator()(Task* task) {
		MyTask* pTag = dynamic_cast<MyTask*>(task);
		while (1) {
			m_ctrl->wait(this);
			pTag->ShowData();
			m_ctrl->done();
			printf("Thread:%p\n",task);
			Thread::mSleep(1000);
		}
	}
};

int main()
{
	ThreadPool pool(3);
	TaskCtrl tCtrl(3);

	MyTask* pt1 = new MyTask("test");
	MyTask* pt2 = new MyTask("test");
	MyTask* pt3 = new MyTask("test");
	pt1->SetData(1);
	pt2->SetData(2);
	pt3->SetData(3);
	tCtrl.AddThread(pt1);
	tCtrl.AddThread(pt2);
	tCtrl.AddThread(pt3);
	MyTaskProcess* processor = new MyTaskProcess(&tCtrl);
	pool.Register_taskProcess("test", processor);
	if (!pool.init_pool()) {
		puts("init_pool");
		pool.submit_job_req(pt1);
		pool.submit_job_req(pt2);
		pool.submit_job_req(pt3);
	}
    int i = 0;
	while (1) {
		printf("main thread sync signal i:%d\n",i++);
		tCtrl.SyncTaskOneTime();
		Thread::mSleep(1000);
	}
	pool.stop_pool();
    return 0;
}