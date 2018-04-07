#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <time.h>
#include <sys/sem.h>

#define BUFFER_SIZE 3
#define SLEEP_TIME 2000000
#define SHMKEY 100
#define FULL_KEY  1234  // 同步信号：生产的项目
#define EMPTY_KEY 1235  // 同步信号：剩余空间
#define MUTEX_KEY 1236  // 互斥信号：二值信号灯

int shmid;                       // id of 共享主存区
int full_id, empty_id, mutex_id; // id of 信号量

union semun{
	int val; // 信号量的值
	struct semid_ds *buf; 
	unsigned short *array;
};

// P,V 操作封装
void P(int sem_id){
   struct sembuf xx;
   xx.sem_num = 0; // 信号量在信号量集中的索引号
   xx.sem_op = -1; // 请求资源
   xx.sem_flg = 0;
   semop(sem_id, &xx, 1); // 执行操作
}

void V(int sem_id){
   struct sembuf xx;
   xx.sem_num = 0;
   xx.sem_op =  1; // 释放资源
   xx.sem_flg = 0;
   semop(sem_id, &xx, 1);
}

void set_semval(int semid, int val){
	union semun sem_val;
	sem_val.val = val; // 初始值
    semctl(semid, 0, SETVAL, sem_val); // 初始化信号量的值
}

void showTime(){
	time_t now;
	time(&now);
	struct tm *p;
	p = localtime(&now);
	printf("%d\t%d-%02d-%02d\t%02d:%02d:%02d", 
			getpid(),
			(1900 + p->tm_year), 
			(1 + p->tm_mon), 
			p->tm_mday,
			(p->tm_hour), 
			p->tm_min, 
			p->tm_sec);
}

void showShm(){
	char *addr = shmat(shmid, 0, 0);
	printf("%c %c %c\t",
			addr[0], addr[1], addr[2]);
	shmdt(addr);
}

void newLine(){
	printf("\r\n");
	fflush(stdout);
}

// generate a random int from 1 to n
int randInt(int n){
	char buf[4];
	int *p = (int*)buf;
	int fd = open("/dev/urandom",O_RDONLY);
	read(fd, buf, 4);
	close(fd);
	unsigned int tmp = *p;
	tmp %= n;
	return tmp + 1; 
}

char produceItem(){
	return 'A' + randInt(26) - 1;
}

void consumeItem(char i){
	//do nothing
}

void putItemIntoBuffer(char c){
	int i;
	char *addr = shmat(shmid, 0, 0);
	// 将一个字符放入缓冲区
	for(i=0; i<BUFFER_SIZE; i++){
		if(addr[i] == '-'){
			addr[i] = c;
			// 显示生产者操作
			printf("\t-> buf[%d] = %c\t", i, c);
			break;
		}
	}
	// err check
	if(i >= BUFFER_SIZE){
		printf("\tpro fail\t");
	}
	shmdt(addr);
}

char removeItemFromBuffer(){
	char *addr = shmat(shmid, 0, 0);
	int i; char c;
	// 从缓冲区中取走一个字符
	for(i=BUFFER_SIZE-1; i>=0; i--){
		if(addr[i] != '-'){
			c = addr[i];
			addr[i] = '-';
			// 显示消费者操作
            printf("\t<- buf[%d] = %c\t", i, c);
			break;
		}
	}
	// err check
	if(i < 0) {
		printf("\tcon failed\t");
	}
	shmdt(addr);
	return c;
}

void producer(){
	int i; char item;
	for(i=0; i<6; i++){
		usleep(randInt(SLEEP_TIME));
		printf("<%d> ", i+1); // 第几次生产
		item = produceItem();
		P(empty_id);
			P(mutex_id);
				showTime();
				putItemIntoBuffer(item);
				showShm();
			V(mutex_id);
		V(full_id);
		newLine();
	}
}

void consumer(){
	int i; char item;
	for(i=0; i<4; i++){
		usleep(randInt(SLEEP_TIME));
		printf("[%d] ", i+1); // 第几次消费
		P(full_id);
			P(mutex_id);
				showTime();
				item = removeItemFromBuffer();
				showShm();
			V(mutex_id);
		V(empty_id);
		consumeItem(item);
		newLine();
	}
}

int main(){
	int i;
	// 申请共享主存区
	shmid = shmget(SHMKEY, BUFFER_SIZE, 0666|IPC_CREAT);
	// 初始化共享主存区(数据缓冲区)
	char *addr = shmat(shmid, 0, 0);
	for (i=0; i<BUFFER_SIZE; i++){
		addr[i] = '-';  
	}
	shmdt(addr);

	// 申请信号量
	full_id  = semget((key_t)FULL_KEY,  1, 0666|IPC_CREAT);
	empty_id = semget((key_t)EMPTY_KEY, 1, 0666|IPC_CREAT);
	mutex_id = semget((key_t)MUTEX_KEY, 1, 0666|IPC_CREAT);
	// 初始化信号量
	set_semval(full_id,  0);
	set_semval(empty_id, 3);
	set_semval(mutex_id, 1);

	// 打印标题 
	printf("times/pid\tdate\t\ttime\t\toperation\tshm\r\n");

	// 创建进程
	pid_t pid;
	for(i=0; i<5; i++){
		if( (pid = fork()) == -1){
			printf("fork failed\n");
			exit(0);
		} else if(pid == 0){
			// do nothing let i++ and do next fork
			continue; 
		} else {
			if(i<2){
				producer();
				// only need one process to be create
				break; 
			} else {
				consumer();
				break;
			}
		}
	}
}
