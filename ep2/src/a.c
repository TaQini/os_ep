#include <stdio.h>
#include <unistd.h>
int main(){
	sleep(3);
	printf("[-] pid:%5d done.\n", getpid());
}
