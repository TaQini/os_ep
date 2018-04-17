#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>  
#include <psapi.h>

void showSys(){
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	LPCVOID *MinAddr = si.lpMinimumApplicationAddress;
	LPCVOID *MaxAddr = si.lpMaximumApplicationAddress;

	printf("Page Size: %d KB\n", si.dwPageSize / 1024);
	printf("Processor Num: %d\n", si.dwNumberOfProcessors);
	printf("Cpu Arch: %d\n", si.dwProcessorType);
	printf("VM Fineness: %d KB\n", si.dwAllocationGranularity / 1024);
	printf("Valiable VM: %0.2f GB\n", 
		((DWORD)MaxAddr - (DWORD)MinAddr) / (1024.0*1024.0*1024.0));
	printf("Range Of VM: 0x%08x - 0x%08x\n", MinAddr, MaxAddr);
	return ;
}

void showMem(){
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	printf("Memory Used: %d%%\n", ms.dwMemoryLoad);
	DWORD TotalPhys = ms.dwTotalPhys;
	DWORD AvailPhys = ms.dwAvailPhys;
	printf("Phys Mem: %.2fM/%.2fM Free:%.1f%%\n", 
		(float)AvailPhys/1024/1024,
		(float)TotalPhys/1024/1024, 
		(1-(float)AvailPhys/TotalPhys)*100);
	DWORD TotalVirs = ms.dwTotalVirtual;
	DWORD AvailVirs = ms.dwAvailVirtual;
	printf("Virs Mem: %.2fM/%.2fM Free:%.1f%%\n", 
		(float)AvailVirs/1024/1024, 
		(float)TotalVirs/1024/1024, 
		(1-(float)AvailVirs/TotalVirs)*100);
	return ;
}

void showAllProc(){
	// get the snapshoot
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    if( hProcessSnap==INVALID_HANDLE_VALUE ) {
        printf("CreateToolhelp32Snapshot fialed\n");  
        return ;
    }

    // get info for each process
	PROCESSENTRY32 stcProcessInfo;
	stcProcessInfo.dwSize = sizeof(stcProcessInfo);

	BOOL bRet = Process32First(hProcessSnap, &stcProcessInfo);
	PROCESS_MEMORY_COUNTERS pmc;
	pmc.cb = sizeof(pmc);

	while (bRet) {
		// process info
		printf("Name:\t%s\nPID:\t%d\nThreads:%d\nPPID:\t%d\n", 
			stcProcessInfo.szExeFile, 
			stcProcessInfo.th32ProcessID, 
			stcProcessInfo.cntThreads, 
			stcProcessInfo.th32ParentProcessID);  

		// workset infomation
		HANDLE hProcess =  OpenProcess(PROCESS_ALL_ACCESS, TRUE, stcProcessInfo.th32ProcessID);;

		GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
		printf("Used:\t%d KB\n", (int)pmc.PagefileUsage / 1024);
		printf("WorkSet:%d KB\n", (int)pmc.WorkingSetSize / 1024);
		printf("\n");
 
		CloseHandle(hProcess);
		bRet = Process32Next(hProcessSnap, &stcProcessInfo);
	}
	CloseHandle(hProcessSnap);
	return ;
}

// showVM address
void WalkVM(HANDLE hProcess) {
	SYSTEM_INFO si;                     
	ZeroMemory(&si, sizeof(si));		
	GetSystemInfo(&si);                 
	MEMORY_BASIC_INFORMATION mbi;       
	ZeroMemory(&mbi, sizeof(mbi));

	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;

	while (pBlock < si.lpMaximumApplicationAddress) {
		if (VirtualQueryEx(hProcess,pBlock,&mbi,sizeof(mbi)) == sizeof(mbi)) {
			
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];

			// show
			printf("0x%08X -- 0x%08X: ",pBlock, pEnd);
			
			// show the status about the block in the process
			switch (mbi.State) {
				case MEM_COMMIT:printf(" Committed"); break;
				case MEM_FREE:printf(" Free"); break;
				case MEM_RESERVE:printf(" Reserved"); break;
			}

			// show the type
			switch (mbi.Type) {
				case MEM_IMAGE:printf(", Image"); break;
				case MEM_MAPPED:printf(", Mapped"); break;
				case MEM_PRIVATE:printf(", Private"); break;
			}

			printf("\n");
			// get the next block
			pBlock = pEnd;
		}
	}
}

// show process info by PID
void showSingleProcess() {
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	HANDLE hProcess;

    if( hProcessSnap==INVALID_HANDLE_VALUE ) {
        printf("CreateToolhelp32Snapshot fialed\n");  
        return ;
    }

    // get info for each process
	PROCESSENTRY32 stcProcessInfo;
	stcProcessInfo.dwSize = sizeof(stcProcessInfo);

	BOOL bRet = Process32First(hProcessSnap, &stcProcessInfo);
	PROCESS_MEMORY_COUNTERS pmc;
	pmc.cb = sizeof(pmc);

	int pid;
	printf("Input: ");
	scanf("%d", &pid);

	while (bRet) {
		if (pid == stcProcessInfo.th32ProcessID) {
			printf("PName: %s\n", stcProcessInfo.szExeFile);
			printf("PID: %d\n", stcProcessInfo.th32ProcessID);
			printf("ThreadNum: %d\n", stcProcessInfo.cntThreads);
			
			// get the memory used status about this process
			HANDLE hProcess =  OpenProcess(PROCESS_ALL_ACCESS, TRUE, stcProcessInfo.th32ProcessID);;

			GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
			printf("Used:\t%d KB\n", (int)pmc.PagefileUsage / 1024);
			printf("WorkSet:%d KB\n", (int)pmc.WorkingSetSize / 1024);
			printf("\n");

			WalkVM(hProcess);

			CloseHandle(hProcess);
			break;
		}
		// get the next handle about the process
		bRet = Process32Next(hProcessSnap, &stcProcessInfo);
	}
	CloseHandle(hProcessSnap);
}

void usage(){
	printf("\n");
	printf("1.show SYSTEM Infomation\n");
	printf("2.show MEMORY Infomation\n");
	printf("3.show PROCESS Infomation\n");
	printf("4.show target PROCESS Info\n");
}

int main() {
	usage();
	while (1){
		printf("\n$ ");
		int cmd = 0;
		scanf("%d", &cmd);
		switch(cmd){
			case 1: showSys(); break;
			case 2: showMem(); break;
			case 3: showAllProc(); break;
			case 4: showSingleProcess(); break;
			default: exit(0);
		}
	}
}
