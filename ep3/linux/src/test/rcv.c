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
	long int mrecv;
	msgid = msgget((key_t)atoi(argv[1]), 0666|IPC_CREAT);

		mrecv = 1;
		msgrcv(msgid, &data, BUFSIZE, mrecv, 0);
		printf("[1] %s\n", data.mtext);

		mrecv = 2;
		msgrcv(msgid, &data, BUFSIZE, mrecv, 0);
		printf("[2] %s\n", data.mtext);

		mrecv = 2;
		msgrcv(msgid, &data, BUFSIZE, mrecv, 0);
		printf("[3] %s\n", data.mtext);

		mrecv = 1;
		msgrcv(msgid, &data, BUFSIZE, mrecv, 0);
		printf("[4] %s\n", data.mtext);

		mrecv = 2;
		msgrcv(msgid, &data, BUFSIZE, mrecv, 0);
		printf("[5] %s\n", data.mtext);
}

