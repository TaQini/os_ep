#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#define SEMKEY 300

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

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

void file_operation(int semid, char *filepath, int pid){
	struct sembuf se;
	time_t now;
	int i;
	se.sem_num = 0;
	se.sem_op = -1;
	se.sem_flg = 0;
	semop(semid, &se, 1);
	FILE *file;
	file = fopen(filepath, "a");
	for(i=0;i<3;i++){
		time(&now);
		fprintf(file, "%d: file operation by process %d\n", now, getpid());
		sleep(randInt(3));
	}
	se.sem_num = 0;
	se.sem_op = 1;
	se.sem_flg = 0;
	semop(semid, &se, 1);
}

int main(int argc, char **argv){
	union semun sem_val;
	int semid, pid, pid2;
	semid = semget(SEMKEY, 1, 0666|IPC_CREAT);
	sem_val.val = 1;
	semctl(semid, 0, SETVAL, sem_val);
	pid = fork();
	if(pid>0){ //proc 1
		pid2 = fork();
		if(pid2 == 0){
			execl("other","other",0);
		}
		file_operation(semid, "./semfile", getpid());
	}
	else{ // proc 2
		file_operation(semid, "./semfile", getpid());
	}
}








