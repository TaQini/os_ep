#include "ballistics.h"
#include "GBCSolution.h"
#include "Result.h"
#include "stdio.h"
#include "Model.h"
#include "unistd.h"
#define ms(X) (X.tv_sec*1000000+X.tv_usec)%1000000
#define M 25
#define N 25

#include "sys/time.h"
int main(){
	Result * rst = NULL;
	int cnt = 0;
	struct timeval t;
	pid_t pid;
	if ((pid = fork())== 0){
		Model * m = new Model(M);
		while(rst=m->moveToNext()){
			printf("Time %f Range %f [%d]\n", rst->getTime(), rst->getRange(), cnt++);
			free(rst); // shit free
		}
	}
	else{
		Model * m = new Model(N);
		while(rst=m->moveToNext()){
			printf("\t\t\t\t\t[%d] Time %f Range %f\n", cnt++, rst->getTime(), rst->getRange());
			free(rst); // shit free
		}
	}
}
