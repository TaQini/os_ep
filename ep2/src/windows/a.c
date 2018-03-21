#include <windows.h>
int main(int argc, char **argv){
	Sleep(3000);
	printf("%d %s done.\n",getpid(),argv[0]);
	return 0;
}

