#include <string.h>
#include <stdio.h>
int main(){
	char buf[1024];
	fgets(buf, 1024, stdin);
	printf("%d\n",strncmp(buf, "end", 3));
	printf("%d\n",strncmp(buf, "abc", 3));
}
