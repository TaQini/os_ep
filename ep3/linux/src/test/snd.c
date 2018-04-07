#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "mymsg.h"
#define BUFSIZE 1
int main(int argc, char **argv){
	int msgid; 
	struct msg data;
	char buffer[4];
	msgid = msgget((key_t)atoi(argv[1]), 0666|IPC_CREAT);

		data.mtype = 1;
		strcpy(data.mtext, "a");
		msgsnd(msgid, &data, BUFSIZE, 0);

		data.mtype = 1;
		strcpy(data.mtext, "b");
		msgsnd(msgid, &data, BUFSIZE, 0);

		data.mtype = 2;
		strcpy(data.mtext, "c");
		msgsnd(msgid, &data, BUFSIZE, 0);

		data.mtype = 2;
		strcpy(data.mtext, "d");
		msgsnd(msgid, &data, BUFSIZE, 0);

		data.mtype = 2;
		strcpy(data.mtext, "e");
		msgsnd(msgid, &data, BUFSIZE, 0);
}
