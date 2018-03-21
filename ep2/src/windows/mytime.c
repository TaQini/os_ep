#include <stdio.h>
#include <windows.h>
#define ms(X) X.wSecond*1000+X.wMilliseconds
HANDLE startproc(char* cmd) {
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	BOOL bCreateOk = CreateProcess(
		NULL,
		cmd,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si, //StartupInfo
		&pi  //ProcessInfo
		);
	if (bCreateOk) {
		return pi.hProcess;
	}
	else {
		return INVALID_HANDLE_VALUE;
	}
}
int main(int argc, char** argv) {
	SYSTEMTIME begin, end;
	HANDLE hchild = startproc(argv[1]);
	GetSystemTime(&begin);
	if (hchild != INVALID_HANDLE_VALUE) {
		WaitForSingleObject(hchild, INFINITE); // wait for child proc done
		GetSystemTime(&end);
		printf("[*] %s cost: %f s\n", argv[1], (ms(end)-ms(begin))/1000.0);
	}
	else {
		printf("create child proc failed.\n");
	}
	return 0;
}

