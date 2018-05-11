#include "ballistics.h"
#include "GBCSolution.h"
#include "Result.h"
#include "Model.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/sem.h>

#define SHMKEY 1246
#define MUTEX_KEY 1245
#define BUFFER_SIZE 305*sizeof(double)+105*sizeof(int)
#define ms(X) (X.tv_sec*1000000+X.tv_usec)%1000000
#define M 100
#define N 30
#define debug 0

int shmid;
int mutex_id;
int step[] = {25, 5, 1, 10, 10, 5, 5, 10, 5, 5, 25, 5, 25, 10, 10, 5, 25, 10, 25, 25, 5, 10,
  5, 25, 10, 10, 10, 5, 10, 10, 5, 25, 5, 10, 5, 5, 10, 5, 10, 10, 25, 10, 5, 10, 10, 10, 10, 25,
  5, 5, 25, 10, 5, 25, 5, 10, 5, 10, 10, 5, 25, 10, 5, 10, 10, 10, 5, 10, 10, 5, 5, 10, 10, 5,
  25, 5, 5, 25, 25, 10, 25, 25, 25, 10, 5, 10, 5, 10, 10, 10, 10, 5, 5, 5, 10, 25, 10, 5, 10, 10};

union semun{
	int val; 
	struct semid_ds *buf; 
	unsigned short *array;
};

void P(int sem_id){
    struct sembuf xx;
    xx.sem_num = 0; 
    xx.sem_op = -1; 
    xx.sem_flg = 0;
    semop(sem_id, &xx, 1); 
}

void V(int sem_id){
    struct sembuf xx;
    xx.sem_num = 0;
    xx.sem_op =  1; 
    xx.sem_flg = 0;
    semop(sem_id, &xx, 1);
}

void set_semval(int semid, int val){
	union semun sem_val;
	sem_val.val = val; 
    semctl(semid, 0, SETVAL, sem_val); 
}

struct msg{
	double current; // current time
	double next;    // next time
	double range;   // range
	int    cnt;     // next range
}buf[101];

Result* update(Model * m, int i){
	Result *rst = m->moveToNext();
	if(!rst) return NULL;
	// send msg to shm
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	addr[i].current = rst->getTime();
	addr[i].next    = m->getNextTime();
	addr[i].range   = rst->getRange();
	addr[i].cnt    += step[i];
	printf("Model-\033[47;31m%02d\033[0m moving... step=\033[31m%02d\033[0m, time: %f, range:%f\n",
			i, step[i], addr[i].current, addr[i].range);
	shmdt(addr);
	return rst;
}

double getNext(int i){
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	double t;
	t = addr[i].next;
	shmdt(addr);
	return t;
}

double getCurrent(int i){
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	double t;
	t = addr[i].current;
	shmdt(addr);
	return t;
}

int getCnt(int i){
    struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
    int t;
    t = addr[i].cnt;
    shmdt(addr);
    return t;
}

double showshm(int i){
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	printf("[%d] shm_state time: %f, %f, %f, next moved %d \n", i, 
    		addr[i].current ,addr[i].next, addr[i].range, addr[i].cnt);
	shmdt(addr);
}

int exist(int i, int *t, int sz){
	int j;
	for(j=0;j<sz;j++){
		if(i==t[j]){
			return 1;
		}
	}
	return 0;
}

int randInt(int n){
	char buf[4];
	int *p = (int*)buf;
	int fd = open("/dev/urandom",O_RDONLY);
	read(fd, buf, 4);
	close(fd);
	unsigned int tmp = *p;
	tmp %= n;
	return tmp; 
}

int* init(Model* m, int i){
	struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
	addr[i].current = 0;
	addr[i].next    = m->getNextTime();
	addr[i].range   = 0;
	addr[i].cnt     = step[i];
	shmdt(addr);

	int *table = (int*)malloc(N*sizeof(int));
	memset(table, -1, N);
	int j;
	table[0] = (i+1)%100;
	for(j=1;j<N;){
		int tmp = randInt(M);
		if (!exist(tmp, table, N) && tmp != i){
			table[j] = tmp;
			j++;
		}
	}
	// sleep(1); // wait for all model init
	if(debug) printf("init[%d] succ\n", i);
	usleep(100);
	return table;
}

void check(int i, int * table){
	double next = getNext(i);
	if(next < 0.00001) return;
	double tmp;
	int j;
	for(j=0; j<N; j++){
		tmp = getCurrent(table[j]);
		if(debug) printf("cmp %f ? %f ...\n", next, tmp);
		if (next < tmp){
			if(next){
				printf("errrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr!\n");
			}
			exit(0);
		}
		else{
			if(debug) showshm(i);
			if(debug) showshm(table[j]);
		}
	}
}

bool notMin(int a){
    struct msg *addr = (struct msg *)shmat(shmid, 0, 0);
    int t;
    int i;
    for(i=0;i<100;i++){
    	t = addr[i].cnt;
    	if(t<a){
    		// printf("%d < %d\n", t, a);
    		// usleep(300);
    		shmdt(addr);
    		return 1;
    	}
    }
    shmdt(addr);
    return 0;
}

bool pause(int i, int * table){
	int a = getCnt(i);
	int b;
	int j;
	for(j=0; j<N; j++){
		b = getCnt(table[j]);
		if(notMin(a)){
			// putchar('0'+i);
			return 1;
		}
	}
	return 0;
}

int main(){
	Result * rst = NULL;
	int i;

	// create shm
	shmid = shmget(SHMKEY, sizeof(buf), 0666| IPC_CREAT);

	// create mutex
	mutex_id = semget((key_t)MUTEX_KEY, 1, 0666|IPC_CREAT);
	set_semval(mutex_id, 1);

	pid_t pid;
	for(i=0; i<M; i++){
		if( (pid = fork()) == -1){
			printf("fork failed\n");
			exit(0);
		} 
		else if(pid == 0){
			// do nothing let i++ and do next fork
			continue; 
		} 
		else{
			if(debug) printf("pid[%d], ppid[%d]\n", getpid(), getppid());
			if(debug) sleep(1);
			Model * m = new Model(step[i]); // the node who send msg
			if(debug) printf("model[%d].step = %d\n",i, step[i]);
			// alarm(2);
			int * table;
			table = init(m, i);

			int a;
			if(debug) printf("model[%d].down:", i);
			for(a=0;a<N;a++){
				if(debug) printf("%d ", table[a]);
			}
			if(debug) printf("\n");

			Result *rst = NULL;

			if(debug) printf("start operation\n");
			
			while(1){
				if(debug) printf("start p[%d]\n", i);
				while(pause(i, table));
		
				P(mutex_id);
				if(debug) printf("p[%d] ok\n", i);
				check(i, table);
				rst = update(m, i);
				if(!rst){
					printf("end!\n");
					break;
				}
				free(rst);
				V(mutex_id);
				if(debug) printf("V[%d] ok\n", i);
				fflush(stdout);
			}
			break;
		}
	}
}
