#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 3
//#define SLEEP_TIME 2000000
#define SLEEP_TIME 100
#define SLEEP_TIMES 1000
#define SHMKEY 100
#define fillCount 3  //offset of 同步信号：生产的项目
#define emptyCount 4 //offset of 同步信号：剩余空间
#define mutex 5      //offset of 互斥信号：二值信号灯

int shmid;	// id of 共享主存区

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
	printf("%c %c %c %c %c %c\t",
			addr[0], addr[1], addr[2],
			addr[3]+'0', addr[4]+'0', addr[5]+'0');
	shmdt(addr);
	//fflush(stdout); // refresh stdout
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

void down(int off){
	char *addr = shmat(shmid, 0, 0);	
	char *p = addr + off;
	// 消费者：若缓冲区为空, 等待生产者添加数据后再读取
	// 消费者：无生产的项目可取 down(fillCount)
	// 生产者：缓冲区无剩余空间 down(emptyCount)
	while(*p <= 0);
	*p -= 1;
	shmdt(addr);
	//showShm();
}

void up(int off){
	char *addr = shmat(shmid, 0, 0); 
	char *p = addr + off;
	// 生产者：若缓冲区已满,等待消费者取走数据后再添加
	// 生产者：生产的项目不能放进缓冲区   up(fillCount)
	// 消费者：空闲空间不能超过缓冲区大小 up(emptyCount)
	while(*p >= BUFFER_SIZE);
	*p += 1;
	shmdt(addr);
	//showShm();
}

void putItemIntoBuffer(char c){
	int i;
	char *addr = shmat(shmid, 0, 0);
	// 将一个字符放入缓冲区
	for(i=0; i<BUFFER_SIZE; i++){
		if(addr[i] == '-'){
			addr[i] = c;
			// 显示时间、生产者操作、共享主存区状态
			//showTime();
			printf("\t-> buf[%d] = %c\t", i, c);
			//showShm();
			break;
		}
	}
	// err check
	if(i>=BUFFER_SIZE){
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
			// 显示时间、消费者操作、共享主存区状态
			//showTime();
            printf("\t<- buf[%d] = %c\t", i, c);
			//showShm();
			break;
		}
	}
	// err check
	if(i<0) {
		printf("\tcon failed\t");
	}
	shmdt(addr);
	return c;
}

void producer(){
	int i, j; char item;
	for(i=0; i<6; i++){
		for(j=0; j<randInt(SLEEP_TIMES); j++)
			usleep(randInt(SLEEP_TIME));
		printf("<%d> ", i+1);
		item = produceItem();
		down(emptyCount);
			down(mutex);
				showTime();
				putItemIntoBuffer(item);
				showShm();
			up(mutex);
		up(fillCount);
		showShm();
		newLine();
	}
}

void consumer(){
	int i, j; char item;
	for(i=0; i<4; i++){
		for(j=0; j<randInt(SLEEP_TIMES); j++)
			usleep(randInt(SLEEP_TIME));
		printf("[%d] ",i);
		down(fillCount);
			down(mutex);
				showTime();
				item = removeItemFromBuffer();
				showShm();
			up(mutex);
		up(emptyCount);
		consumeItem(item);
		showShm();
		newLine();
	}
}

int main(){
	int i;
	// 申请共享主存区(size = 6)
	shmid = shmget(SHMKEY, BUFFER_SIZE+3, 0666|IPC_CREAT);
	// 初始化共享主存区: 数据缓冲区、同步信号、互斥信号
	char *addr = shmat(shmid, 0, 0);
	for (i=0; i<BUFFER_SIZE; i++){
		addr[i] = '-';  
	}
	addr[3] = 0; // fillCount  = 0
	addr[4] = 3; // emptyCount = 3
	addr[5] = 1; // mutex = 1
	shmdt(addr);
	// 打印标题 
	// printf("pid\tdate\t\ttime\t\toperation\tshm status\r\n");
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

