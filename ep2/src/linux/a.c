#include <stdio.h>
#include <unistd.h>
#define debug 0
int main(){
	sleep(3);
	if (debug) printf("[-] pid:%5d done.\n", getpid());
	return 0;
}

