#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SLEEP_TIME 2500
PROCESS_INFORMATION StartClone(int nCloneID);

struct SHM {
	int read;
	int write;
	char buf[3][2];
};

int randInt(int n){
    srand(time(0));
    return rand()%n;
}

int main(int argc, char* argv[]) {
	int nClone = 0;
	int i, time, j;
	HANDLE SEM_FULL;
	HANDLE SEM_EMPTY;
	HANDLE SEM_MUTEX;
	HANDLE hMap;
	PROCESS_INFORMATION nH[5];
	struct SHM *shm;

	if (argc > 1) {
		// 从第二个参数中提取克隆ID
		sscanf(argv[1], "%d", &nClone);
	}

	if (nClone == 0) {		//主进程
		//建立信号量
		SEM_FULL  = CreateSemaphore(NULL, 0, 6, "FULL");
		SEM_EMPTY = CreateSemaphore(NULL, 6, 6, "EMPTY");
		SEM_MUTEX = CreateSemaphore(NULL, 1, 1, "MUTEX");

		//建立共享内存
		HANDLE CurrentProcess = GetCurrentProcess();

		hMap = CreateFileMapping(
			INVALID_HANDLE_VALUE,	//使用页式临时文件
			NULL,					//默认的安全性
			PAGE_READWRITE,			//可读写权
			0,						//最大尺寸（高32位）
			sizeof(*shm),			//最小尺寸（低32位）
			"buffer");				//该文件映射的作为共享内存的缓冲区，取名为“buffer”

		//在文件映射上创建视图
		LPVOID pData = MapViewOfFile(
			hMap,				//保存文件的对象
			FILE_MAP_WRITE,		//映射可读可写
			0,					//在文件的开头处（高32位）开始
			0,					//在文件的开头处（低32位）
			sizeof(*shm));		//整个文件要映射4个字节

		if (pData != NULL) {
			ZeroMemory(pData, sizeof(shm));	 //分配内存空间，并清零
		}
		
        // 初始化数据缓冲区
        hMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, "buffer");
        shm = (struct SHM*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, sizeof(*shm));
		for (i = 0; i<3; i++) {
            strcpy(shm->buf[i], "-");
		}

		//建立5个子进程
		for (i = 0; i<5; i++) {
			nH[i] = StartClone(++nClone);
		}

		//等待子进程结束
		for (i = 0; i<5; i++)
			WaitForSingleObject(nH[i].hProcess, INFINITE);

		//关闭子进程句柄
		for (i = 0; i<5; i++) {
			CloseHandle(nH[i].hProcess);
			CloseHandle(nH[i].hThread);
		}
		//关闭句柄
		CloseHandle(SEM_MUTEX);
		CloseHandle(hMap);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
	}
	else if (nClone > 0 && nClone < 3) {		// id:1,2 生产者
		//获得句柄
		SEM_EMPTY = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "EMPTY");
		SEM_FULL = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "FULL");
		SEM_MUTEX = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "MUTEX");

		hMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, "buffer");
		shm = (struct SHM*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, sizeof(*shm));

		for (i = 1; i<=6; i++) {
            //随机等待
            time = (1+randInt(nClone))*randInt(SLEEP_TIME);            
            Sleep(time);

            WaitForSingleObject(SEM_EMPTY, INFINITE);	// P(empty)
            WaitForSingleObject(SEM_MUTEX, INFINITE);	// P(mutex)

            //向缓冲区添加产品,把0置为1
            int t_writ = shm->write;
            char t_buf[2];
			sprintf(t_buf, "%c", 'A'+(randInt(26)+nClone*randInt(26))%26);
			strcpy(shm->buf[shm->write], t_buf);
			shm->write = (shm->write + 1) % 3;

			//获得时间
			SYSTEMTIME curtime;
			GetSystemTime(&curtime);

			//输出时间、操作、缓冲区状态
			printf("[%d] P%d\t%02d:%02d:%02d\t-> buf[%d] = %s\t"
				,i, nClone, curtime.wHour, curtime.wMinute, curtime.wSecond, t_writ, t_buf);
			for (j = 0; j<3; j++) {
				printf("%s ", shm->buf[j]);
			}
			printf("\n");

			ReleaseSemaphore(SEM_MUTEX, 1, NULL);	// V(mutex)
			ReleaseSemaphore(SEM_FULL,  1, NULL);	// V(full)
		}

		//关闭句柄
		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMap);

	}
	else if (nClone>2 && nClone< 6) {		//id:3,4,5 消费者 
		//获得句柄
		SEM_EMPTY = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "EMPTY");
		SEM_FULL  = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "FULL");
        SEM_MUTEX = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, "MUTEX");

        hMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, "buffer");
        shm = (struct SHM*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, sizeof(*shm));

        for (i = 1; i<=4; i++) {
			//随机等待
			time = (1+randInt(nClone))*randInt(SLEEP_TIME);
			Sleep(time);

            WaitForSingleObject(SEM_FULL,  INFINITE);	// P(full)
            WaitForSingleObject(SEM_MUTEX, INFINITE);	// P(mutex)

            SYSTEMTIME curtime;
            GetSystemTime(&curtime);

            int t_read = shm->read;
            char t_buf[2];
            strcpy(t_buf, shm->buf[shm->read]);

            strcpy(shm->buf[shm->read], "-");
            shm->read = (shm->read + 1) % 3;

            //输出当前信息
            printf("<%d> C%d\t%02d:%02d:%02d\t<- buf[%d] = %s\t"
                ,i, nClone - 2, curtime.wHour, curtime.wMinute, curtime.wSecond, t_read, t_buf);
            for (j = 0; j<3; j++) {
				printf("%s ", shm->buf[j]);
            }
            printf("\n");

            ReleaseSemaphore(SEM_MUTEX, 1, NULL);	// V(mutex)
            ReleaseSemaphore(SEM_EMPTY, 1, NULL);	// V(empty)
		}

		CloseHandle(SEM_MUTEX);
		CloseHandle(SEM_EMPTY);
		CloseHandle(SEM_FULL);
		CloseHandle(hMap);
	}
	return 0;
}

// 创建一个克隆的进程
PROCESS_INFORMATION StartClone(int nCloneID) {
	TCHAR szFilename[MAX_PATH];
	GetModuleFileName(NULL, szFilename, MAX_PATH);

	TCHAR szCmdLine[MAX_PATH];
	sprintf(szCmdLine, "\"%s\" %d", szFilename, nCloneID);

	STARTUPINFO si;
	ZeroMemory((&si), sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;

	BOOL bCreateOK = CreateProcess(
		szFilename,
		szCmdLine,
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&si,
		&pi);
	return pi;
}
