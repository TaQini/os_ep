#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#define ms(X) X.tv_sec*1000000+X.tv_usec 
long buf[0];
int main(int argc, char **argv){
	int fd[2];
	struct timeval start, end;
	char *const parmList[] = {argv[1], NULL};
	pid_t pid;
	pipe(fd);
	if ((pid = fork()) == -1){
		perror("fork error!\n");
	}
	else if (pid == 0){
		printf("[+] pid:%5d executing...\n", getpid());
		gettimeofday(&start, NULL);
		buf[0] = ms(start);
		write(fd[1], buf, sizeof(long));
		execv(argv[1], parmList);
	}
	else{
		printf("[+] pid:%5d waiting for child proc...\n", getpid());
		read(fd[0], buf, sizeof(long));
		wait(0);
		gettimeofday(&end, NULL);
		printf("[-] pid:%5d done.\n", getpid());
	}
	printf("[*] cost: %fs.\n", (ms(end)-*buf)/1000000.0);
	return 0;
}
