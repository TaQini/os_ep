#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define ms(X) X.tv_sec*1000000+X.tv_usec 

#define debug 1
int main(int argc, char **argv, char **envp){
	int fd[2];
	long buf[1];
	struct timeval start, end;
	pid_t pid;
	pipe(fd);
	if ((pid = fork()) == -1){
		perror("fork error!\n");
	}
	else if (pid == 0){
		if (debug) printf("[+] pid:%5d executing...\n", getpid());
		gettimeofday(&start, NULL);
		buf[0] = ms(start); // start time of exec
		write(fd[1], buf, sizeof(long));
		execvpe(argv[1], argv+1, envp); // parmlist is argv[2:]
	}
	else{
		if (debug) printf("[+] pid:%5d waiting for child proc...\n", getpid());
		read(fd[0], buf, sizeof(long));
		pid_t who = wait(0);
		if (debug) printf("[-] pid:%5d done.\n", who);
		gettimeofday(&end, NULL); // time of child proc done
		if (debug) printf("[-] pid:%5d done.\n", getpid());
	}
	printf("[*] %s cost: %fs.\n", argv[1], (ms(end)-*buf)/1000000.0);
	close(fd[0]);
	close(fd[1]);
	return 0;
}

