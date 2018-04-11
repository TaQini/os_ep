#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

void cp(char *src_d, char *dest_d); 
void cp_file(char *src, char *dest); 
void file_err(char *filename);
void usage();

int main(int argc,char *argv[]) {
  	struct stat statbuf; 
  	DIR *dir; 
	// usage err
	if(argc != 3) {
		usage();
	}
	// src file is not a folder or src is a symbolic link
	lstat(argv[1],&statbuf);
	if((dir = opendir(argv[1])) == NULL || S_ISLNK(statbuf.st_mode) ) {
		file_err(argv[1]);
	}
	// if dest folder does not exist, create it
	if((dir = opendir(argv[2])) == NULL) {
		mkdir(argv[2],statbuf.st_mode);
	}
	// call mycp
	cp(argv[1], argv[2]); 
  	return 0;
}

void cp(char *src_d,char *dest_d) {
	char src[512], dest[512];
	struct stat statbuf;
	DIR *dir;
	struct dirent *entry;
	strcpy(src, src_d);
	strcpy(dest, dest_d);
	dir = opendir(src); 
	while((entry = readdir(dir)) != NULL) {
		if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0) {
			continue;							// ignore . and ..
		}
		// cp symlink file
		if(entry->d_type == DT_LNK) {
			strcat(src, "/");
			strcat(src, entry->d_name);
			strcat(dest, "/");
			strcat(dest, entry->d_name);

			char buf[512];
			memset(buf, 0, sizeof(buf));		// init buf with '\0'
			readlink(src, buf, sizeof(buf));	// filename of symlink file point to
			symlink(buf, dest);					// create symlink file 

			strcpy(src, src_d); 				// back to previous directory
			strcpy(dest, dest_d);
		}
		// cp directory file
  		else if(entry->d_type == DT_DIR) {
			strcat(src, "/");
			strcat(src, entry->d_name);		// append filename of subdir
			strcat(dest, "/");
			strcat(dest, entry->d_name);	

   		 	stat(src, &statbuf);
			mkdir(dest, statbuf.st_mode);
			cp(src, dest);					// cp subdir (recursive)

			strcpy(src, src_d);
			strcpy(dest, dest_d);
  		}
  		// cp other type file
  		else {
			strcat(src, "/");
			strcat(src, entry->d_name);	 	// append filename
			strcat(dest, "/");
			strcat(dest, entry->d_name);

			cp_file(src, dest);	

			strcpy(src, src_d);				// back to previous directory
			strcpy(dest, dest_d);
  		}
	}
}

void cp_file(char *src,char *dest) {
	int fd = open(src, O_RDONLY); 
	int fd2; // fd of dest file
	char buf[1024]; 
	int len;
	struct stat statbuf;
	stat(src, &statbuf);
	fd2 = creat(dest, statbuf.st_mode); 
	while((len = read(fd, buf, 1024)) > 0) {
		write(fd2, buf, len); 
	}
	close(fd); 
	close(fd2);
}

void file_err(char *filename){
	printf("cp: cannot stat '%s': No such file or directory.\n", filename);
	exit(0);
}

void usage(){
  	printf("usage: mycp src_folder dest_folder\n");
	exit(0);
}
