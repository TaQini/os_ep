#include "ballistics.h"
#include "GBCSolution.h"
#include "Result.h"
#include "Model.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/time.h>
#define SHMKEY 1246
#define BUFFER_SIZE 205*sizeof(double)
#define ms(X) (X.tv_sec*1000000+X.tv_usec)%1000000
#define M 200
#define N 200

int shmid;

struct msg{
	double current;
	double next;
	double range;
}buf[101];

void update(Result* rst, int i){
	// send msg to shm
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	addr[i].t = rst->getTime();
	addr[i].r = rst->getRange();
	shmdt(addr);
}

double showshm(int i){
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	double t;
	t = addr[i].t;
    	printf("[%d] shm_state time: %f, %f \n", i, addr[i].t ,addr[i].r);
    	shmdt(addr);
	return t;
}

void check(int a, int b){
	if (showshm(a) <= showshm(b)){
		printf("errrrrrrrrr!\n");
		exit(0);
	}
	else{
	//	printf("succccccccccess!!!!\n");
	}
}

int main(){
	Result * rst = NULL;
	int cnt = 0;
	struct timeval t;

	// create shm
	shmid = shmget(SHMKEY, sizeof(buf), 0666| IPC_CREAT);

	// 
	shmid_ds = shmget(SHMKEY_DS, sizeof(buf), 0666| IPC_CREAT);
	// struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	// printf("successful!!!!!\n\n");
	// shmdt(addr);

	pid_t pid;
	if ((pid = fork())== 0){
		Model * m = new Model(M); // the node who send msg
		//while(rst=m->moveToNext()){
	//		sleep(1);
			printf("pred time: %f\n", m->getNextTime());
			rst=m->moveToNext();
			printf("Time %f Range %f [%d]\n", rst->getTime(), rst->getRange(), cnt++);
			// send msg to shm
			update(rst, 0);

			// check err
			check(0,1);

			free(rst); // shit free
		//}
	}
	else{
		Model * m = new Model(N); // the node 
		int i = 1;
		//while(i){
		//	i++;
		//	if(i%4)
		for(i = 0; i<2; i++){
			rst=m->moveToNext();
			sleep(1);
		//	if(!rst) break;
		//	if(i%4) 
			printf("\t\t\t\t\t[%d] Time %f Range %f\n", cnt++, rst->getTime(), rst->getRange());
		//	if(i%4) 
			update(rst, 1);
			check(0,1);

			free(rst); // shit free
		}
	}
}
