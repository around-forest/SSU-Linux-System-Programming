#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

void remove_all(char *backup_dir, int *nfiles, int *ndirs); //-c option
void get_backup_filename(char *backup_filename, char *original_filename, char *dst_file_path); //backuped filename
void remove_file(char *file_path, char *backup_dir, char *real_path); //remove file
void remove_directory(char *real_dir); //remove directory

int main(int argc, char *argv[])
{
        // argv[0] = hash, [1] = home_dir, [2] = backup_dir, [3] = current_dir, [4] = filename, [5] = option, [6] = option2

        char *home_dir = argv[1];
        char *real_path;
	char backuped_path[4096];
        char *backup_dir = argv[2];
        char *current_dir = argv[3];
        struct stat file_info;
	char temp[4096];
	strcpy(temp, argv[4]);
	int a = 0;
	int b = 0;
	int *nfiles;
	int *ndirs;

	nfiles = &a;
	ndirs = &b;
//error
        if(argv[4] == NULL){
                fprintf(stderr, "Usage1 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory");
                exit(1);
        }

        if(argv[5] != NULL && strcmp(argv[5],"-a") != 0){
                fprintf(stderr, "Usage1 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
                exit(1);
        }

	if(argc > 6){
		fprintf(stderr, "Usage1 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
                exit(1);
	}

	if(argv[4] == "-c" && (argc > 5)){
		fprintf(stderr, "Usage1 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
                exit(1);
	}

        //<filename> 구분
        if(strncmp(argv[4], "/home", 5) == 0){ //절대경로
                if(strncmp(argv[4], home_dir, strlen(home_dir)) != 0 || strncmp(argv[4], backup_dir, strlen(backup_dir)) == 0){
                        fprintf(stderr, "\"%s\" can't be backuped\n", argv[4]);
                        exit(1);
                }
                real_path = argv[4];
        }
        else if(strncmp(argv[4], ".", 1) == 0){ //상대경로
                for(int i = 0; i < strlen(argv[4]); i++){
			temp[i] = temp[i+1];
		}
		real_path = backup_dir;
		strcat(real_path, temp);
        }
	else if(strcmp(argv[4], "-c") == 0){
		real_path = argv[4];
		remove_all(backup_dir, nfiles, ndirs);
		printf("backup directory cleared(%d regular files and %d subdirectories totally).\n", *nfiles, *ndirs);
		exit(1);
	}
	else if(strcmp(argv[4], "P1") == 0 || strcmp(argv[4], current_dir) == 0){
                real_path = current_dir;
        }

        else{ //파일명
		char btemp[4096];
		strcpy(btemp, current_dir);
                strcat(btemp, "/");
                strcat(btemp, argv[4]);
                real_path = btemp;
        }
	//backup dir path
	strcpy(backuped_path, backup_dir);
	char rtemp[4096];
	strcpy(rtemp, real_path);
	strcat(backuped_path, strrchr(rtemp, '/'));
	//access
        if(access(real_path, R_OK|W_OK) != 0){
                fprintf(stderr, "Usage2 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
                exit(1);
        }
	//filename length
        if(strlen(real_path) > 4096){
                fprintf(stderr, "<FILENAME> should be shorter than 4096 bytes\n");
                exit(1);
        }
	//error
        if(stat(real_path, &file_info) != 0){
                fprintf(stderr, "Usage3 : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
                exit(1);
        }

        if(S_ISREG(file_info.st_mode) != 1 && S_ISDIR(file_info.st_mode) != 1){
                fprintf(stderr, "<FILENAME> should be either regular file or directory\n");
                exit(1);
        }

        if(S_ISREG(file_info.st_mode) == 1 && argv[5] != NULL){
		if(argv[5] != "-a"){
                	fprintf(stderr, "only \"-a\" is available for regular file\n");
                	exit(1);
		}
        }

        if(S_ISDIR(file_info.st_mode) == 1){
		if(strncmp(argv[5], "-a", 2) != 0){
                	fprintf(stderr, "\"%s\" is a directory file\n", real_path);
                	exit(1);
		}
        }

        //백업 코드
        //디렉토리
        if(S_ISDIR(file_info.st_mode) == 1 && argv[5] == "-a"){
                remove_directory(backuped_path);
        }
        //파일
        else{
                remove_file(backuped_path, backup_dir, real_path);
        }

        exit(0);
}
// '-c' option clear backup dir
void remove_all(char *backup_dir, int *nfiles, int *ndirs){
	DIR *dir = opendir(backup_dir);
	struct dirent *entry;
	struct stat fileStat;
	char filePath[4096];

	if(dir == NULL) {
		perror("error opening directory\n");
		exit(1);
	}

	while((entry = readdir(dir)) != NULL){
		if(entry->d_type == DT_REG){ //file
			snprintf(filePath, sizeof(filePath), "%s/%s", backup_dir, entry->d_name);
			if(stat(filePath, &fileStat) == -1){
				perror("error getting file status");
				continue;
			}

			if(remove(filePath) == -1){ //remove file
				perror("Error deleting file");
				continue;
			}
			(*nfiles)++;
		}
		else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){//directory
			snprintf(filePath, sizeof(filePath), "%s/%s", backup_dir, entry->d_name);
			remove_all(filePath, nfiles, ndirs);
			if(rmdir(filePath) == -1){
				perror("Error deleting directory");
				continue;
			}
			(*ndirs)++;
		}
	}
	//if there is no file correspond
	if(nfiles == 0)
		printf("no file(s) in the backup\n");
	closedir(dir);
}
// filename -> backuped filename
void get_backup_filename(char *backup_filename, char *original_filename, char *dst_file_path) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%y%m%d%H%M%S", &tm);
    char *filename = strrchr(original_filename, '/') + 1;
    sprintf(backup_filename, "%s/%s_%s", dst_file_path, filename, time_str);
}

// directory remove
void remove_directory(char *real_dir) {
	DIR *dir = opendir(real_dir);
        struct dirent *entry;
        struct stat fileStat;
        char filePath[4096];

        if(dir == NULL) {
                perror("error opening directory\n");
                exit(1);
        }

        while((entry = readdir(dir)) != NULL){
                if(entry->d_type == DT_REG){ //file
                        snprintf(filePath, sizeof(filePath), "%s/%s", real_dir, entry->d_name);
                        if(stat(filePath, &fileStat) == -1){
                                perror("error getting file status");
                                continue;
                        }

                        if(remove(filePath) == -1){ //remove file
                                perror("Error deleting file");
                                continue;
                        }
			else
				printf("\"%s\" backup file removed\n", filePath);
                }
                else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){//directory
                        snprintf(filePath, sizeof(filePath), "%s/%s", real_dir, entry->d_name);
			rmdir(filePath);
                        remove_directory(filePath);
                }
        }

        closedir(dir);
}


// file remove
void remove_file(char *src_path, char *dst_path, char *real_path) {
	DIR *dir = opendir(dst_path);
        struct dirent *entry;
        struct stat fileStat;
        char filePath[4096];
	int input;
	int nfiles = 0;
	char input_str[4096];
	struct fileinfo {
		char *filelist;
		long long filesizelist;
	};
	struct fileinfo fileinfo[4096];

        if(dir == NULL) {
                perror("error opening directory\n");
                exit(1);
        }
	//count numbers of same files
        while((entry = readdir(dir)) != NULL){
                if(entry->d_type == DT_REG){ //file
                        snprintf(filePath, sizeof(filePath), "%s/%s", dst_path, entry->d_name);
                        if(stat(filePath, &fileStat) == -1){
                                perror("error getting file status");
                                continue;
                        }
			//save the filename into struct
                        if(strncmp(filePath, src_path, strlen(src_path)) == 0){
				fileinfo[nfiles].filelist = strdup(filePath);
				fileinfo[nfiles].filesizelist = fileStat.st_size;
				nfiles++;
                	}
		}
                else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){//directory
                        snprintf(filePath, sizeof(filePath), "%s/%s", dst_path, entry->d_name);
                        remove_file(src_path, filePath, real_path);
			rmdir(filePath);
                }
        }
	//according to the number of files, execute remove
	if(nfiles > 1){
		printf("backup file list of \"%s\"\n", real_path);
		printf("0. exit\n");
		for(int i = 0; i < nfiles; i++){
			char *ptr;
                	ptr = strrchr(fileinfo[i].filelist, '_') + 1;
			
			//file size in buffer
			int len;
			int j;
			int a = 0;

			char buffer[50]; 
			sprintf(buffer, "%lld", fileinfo[i].filesizelist);
			char *digits;

			buffer[49] = 0;

			len = strlen(buffer);
			digits = buffer;

			printf("%d. %s		", i+1, ptr);
			for(j = len; j > 0;){
				printf("%c", *digits++);
				j--;
				if(j > 0 && (j % 3) == 0)
					printf(",");
			}
			printf("bytes\n");
		}//when there are numbers of same files
		printf("Choose file to remove\n>> ");
		fgets(input_str, sizeof(input_str), stdin);
		sscanf(input_str, "%d", &input);
		if(input == 0)//no same file
			exit(1);

		else if(remove(fileinfo[input-1].filelist) != -1){
			printf("\"%s\" backup file removed\n", fileinfo[input-1].filelist);
		}
	}
	else if(nfiles == 1){
		if(remove(fileinfo[0].filelist) != -1){
                        printf("\"%s\" backup file removed\n", fileinfo[0].filelist);
                }
	}
        closedir(dir);

}


