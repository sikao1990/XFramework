#include <stdio.h>//模拟多站点雷达图像合成完整图像的采集控制
#include <time.h>//主任务必须得到同一时刻所有子任务的数据才能再次重新开始
#include <unistd.h> //任何子任务不能先于其它子任务再次重新开始
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

struct data{//消息队列要发送数据类型
	long no;
	int  data;
};
void showsem(int semid,unsigned short *arr,int n){
	semctl(semid,0,GETALL,arr);//查看信号量集中个数
	printf("M:%d A:%d B:%d C:%d\n",arr[0],arr[1],arr[2],arr[3]);
}
long long getTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main(){
	int i;//子进程个数
	int j;
	int loop;
	struct data msg[2]={};
	key_t key=ftok(".",10);
	int semid=semget(key,4,0600|IPC_CREAT);//创建信号量集
	int msgid=msgget(key,0600|IPC_CREAT);//创建消息队列
	unsigned short arr[4]={2,2,2,2};//nM A B nC, M 用于结束时的同步、C 用于开始的同步 
	unsigned short marr[4]={};

	struct sembuf ops[4]={
		{.sem_num=0,.sem_op=0,.sem_flg=SEM_UNDO},
		{.sem_num=1,.sem_op=0,.sem_flg=SEM_UNDO},
		{.sem_num=2,.sem_op=0,.sem_flg=SEM_UNDO},
		{.sem_num=3,.sem_op=0,.sem_flg=SEM_UNDO},
	};
	semctl(semid,0,SETALL,arr);//初始化信号量集的计数器
	srand(time(0));

	for(i=1;i<3;i++){//创建两个子进程
		if(fork()==0){/*子进程逻辑*/
			for(loop=1;loop<5;loop++){
				for(j=0;j<4;j++)ops[j].sem_op=0;
				struct sembuf childop[2]={
					{.sem_num=i,.sem_op=-1,.sem_flg=SEM_UNDO},//仅进程自身对应的信号量-1
					{.sem_num=3,.sem_op=-1,.sem_flg=SEM_UNDO},
				};
				semop(semid,childop,2);//-A-C | -B-C	//第一步 子进程记录开始	1A 1B 0C
				printf("第一步 -A|-B -C 子进程\t%d\t初始化完毕并等待开始指令 2M 1A 1B 0C time:%lld\n",getpid(),getTimeStamp());
				
				//TODO:各子进程的初始化工作
				childop[0].sem_num=i;
				childop[0].sem_op=0;
				childop[1].sem_num=3;
				childop[1].sem_op=1;
				semop(semid,childop,2);//0A+C | 0B+C	//第三步：等待主进程开始通知
				printf("第三步 0A|0B +C 子进程\t%d\t收到开始指令 2M 0A 0B 1C time:%lld\n",getpid(),getTimeStamp());

				msg[i-1].no=getpid();	
				msg[i-1].data=loop;
				msgsnd(msgid,msg+i-1,sizeof(struct data),0);
				sleep(rand()%5);//子进程业务
				printf("子进程%d逻辑处理完成\n",getpid());

				ops[0].sem_op=-1;
				semop(semid,ops,1);//-M					//第四步 通知主进程完成任务
				printf("第四步 -M 子进程\t%d\t完成任务,并等待结束指令 1M 0A 0B 2C time:%lld\n",getpid(),getTimeStamp());

				childop[0].sem_num=0;
				childop[0].sem_op=1;//+M
				childop[1].sem_num=i;
				childop[1].sem_op=-1;//-A|-B
				semop(semid,childop,2);//-A +M | -B +M	//第七步 等待主进程结束通知
				printf("第七步 -A|-B +M 子进程\t%d\t收到结束指令 2M 0A 0B 2C time:%lld\n",getpid(),getTimeStamp());
			
				struct sembuf back[1]={
					{.sem_num=i,.sem_op=2,.sem_flg=SEM_UNDO},
				};
				semop(semid,back,1);//+2B | +2A			//第八步 重置标记
				printf("第八步 +2B|+2A 子进程\t%d\t完成资源释放 2M 2A 2B 2C time:%lld\n",getpid(),getTimeStamp());
			}
			exit(0);
		}
	}
	for(loop=1;loop<5;loop++){
		showsem(semid,marr,4);
		for(i=0;i<4;i++)ops[i].sem_op=0;
		sleep(1);
		ops[1].sem_op=-1;
		ops[2].sem_op=-1;
		semop(semid,&(ops[0])+1,3);//0C-A-B		//第二步,等待所有子进程初始化并通知开始 0A 0B 0C
		printf("第二步 0C-A-B 等待所有子进程初始化并通知开始 time:%lld\n",getTimeStamp());
		
		semop(semid,ops,1);//0M					//第五步 等待子进程处理数据
		printf("第五步 0M 收到所有子进程数据 time:%lld\n",getTimeStamp());
#if 1	//主进程进行数据处理	
		for(j=0;j<2;j++){
			msgrcv(msgid,msg+j,sizeof(struct data),0,0);
			printf("%d ",(msg+j)->data);
		}
#endif		
		puts("主进程数据处理完毕");
		sleep(1);
		ops[1].sem_op=1;
		ops[2].sem_op=1;
		semop(semid,&(ops[0])+1,2);//+A+B		//第六步 通知子进程结束
		printf("第六步 +A+B 所有子进程准备结束 time:%lld\n",getTimeStamp());
	}
	semctl(semid,0,IPC_RMID,NULL);
	msgctl(msgid,IPC_RMID,NULL);
	while(wait(NULL)>0);
	return 0;
}
