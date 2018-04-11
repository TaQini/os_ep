#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <windowsx.h>
#include <string.h>

void Mycp(char *src_d, char *dest_d);
void CpFile(char *src, char *dest);
void FileError(char *filename);
void Usage();

int main(int argc, char *argv[]) {
	WIN32_FIND_DATA lpFindFileData;
	if (argc != 3) {
		Usage();
	}
	else {
		if (FindFirstFile(argv[1], &lpFindFileData) == INVALID_HANDLE_VALUE) {
			FileError(argv[1]);
		}
		if (FindFirstFile(argv[2], &lpFindFileData) == INVALID_HANDLE_VALUE) {
			CreateDirectory(argv[2], NULL);
			//printf("CreateDirectory: %s\n", argv[2]);
		}
		Mycp(argv[1], argv[2]);
	}
	return 0;
}

void Mycp(char *src_d, char *dest_d) { 
	WIN32_FIND_DATA lpFindFileData;
	char src[512];
	char dest[512];
	strcpy(src, src_d);
	strcpy(dest, dest_d);
	strcat(src, "\\*.*");	// ".\\src\\*.*"
	strcat(dest, "\\");
	// find the first file or directory in src
	HANDLE hfind = FindFirstFile(src, &lpFindFileData); 
	if (hfind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hfind, &lpFindFileData) != 0) {
			// cp directory
			if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if ((strcmp(lpFindFileData.cFileName, ".") == 0) ||
					 (strcmp(lpFindFileData.cFileName, "..") == 0)) {
					continue;
				}
				else{
					memset(src, '0', sizeof(src));	// append filename
					strcpy(src, src_d);
					strcat(src, "\\");
					strcat(src, lpFindFileData.cFileName);		
					strcat(dest, lpFindFileData.cFileName);
					
					CreateDirectory(dest, NULL);
					Mycp(src, dest);				// cp subdir (recursive)

					strcpy(src, src_d);				// back to previous directory
					strcat(src, "\\");
					strcpy(dest, dest_d);
					strcat(dest, "\\");
				}
			}
			// cp file
			else {
				memset(src, '0', sizeof(src));
				strcpy(src, src_d);
				strcat(src, "\\");
				strcat(src, lpFindFileData.cFileName);
				strcat(dest, lpFindFileData.cFileName);

				CpFile(src, dest);
				strcpy(src, src_d);
				strcat(src, "\\");
				strcpy(dest, dest_d);
				strcat(dest, "\\");
			}
		}
	}
}

void CpFile(char *src, char *dest) {
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hfind = FindFirstFile(src, &lpFindFileData);
	HANDLE hsrc = CreateFile(src, GENERIC_READ | GENERIC_WRITE, 
							 FILE_SHARE_READ, NULL, OPEN_ALWAYS,	// open 
							 FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hdest = CreateFile(dest, GENERIC_READ | GENERIC_WRITE,
							 FILE_SHARE_READ, NULL, CREATE_ALWAYS,	// create
							 FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD size = lpFindFileData.nFileSizeLow - lpFindFileData.nFileSizeHigh;  //length
	DWORD len;
	LPVOID *buf;
	buf = (LPVOID*)malloc(size*sizeof(LPVOID));
	ReadFile(hsrc, buf, size, &len, NULL);
	WriteFile(hdest, buf, size, &len, NULL);
	CloseHandle(hfind);
	CloseHandle(hsrc);
	CloseHandle(hdest);
}

void FileError(char *filename){
	printf("cp: cannot stat '%s': No such file or directory.\n", filename);
	exit(0);
}

void Usage(){
  	printf("usage: mycp src_folder dest_folder\n");
	exit(0);
}
