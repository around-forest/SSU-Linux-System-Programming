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

void get_recover_filename(char *backup_filename, char *original_filename, char *dst_file_path);
void recover_file1(char *file_path, char *backup_dir_path, char *backup_dir);
void recover_file2(char *src_path, char *dst_path, char *real_path);
void recover_directory(char *dir_path, char *backup_dir_path);
int hash_check(char *file_path, char *backup_dir);

int main(int argc, char *argv[])
{
        // argv[0] = hash, [1] = home_dir, [2] = backup_dir, [3] = current_dir, [4] = filename, [5] = option, [6] = newname

        char *home_dir = argv[1];
        char *real_path;
        char *backup_dir = argv[2];
        char dst_dir[4096];
        char *current_dir = argv[3];
	char *new_name_path;
        struct stat file_info;
        char temp[4096];
	char ntemp[4096];
        strcpy(temp, argv[4]);
	strcpy(ntemp, argv[6]);

        if(argv[4] == NULL){
                fprintf(stderr, "Usage1 : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
                exit(1);
        }

        if(argv[5] != NULL && strstr(argv[5],"-n") != 0){
                fprintf(stderr, "Usage1 : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
                exit(1);
        }

	if(strstr(argv[5], "-n") == 0 && argc != 7){
		fprintf(stderr, "Usage1 : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
		exit(1);
	}

	if(strstr(argv[5], "-n") == 0 && argc == 7){
		//<filename> 구분
	        if(strncmp(argv[6], "/home", 5) == 0){ //절대경로
        	        if(strncmp(argv[6], home_dir, strlen(home_dir)) != 0 || strncmp(argv[6], backup_dir, strlen(backup_dir)) == 0){
                	        fprintf(stderr, "\"%s\" can't be recovered\n", argv[6]);
                        	exit(1);
                	}
                	new_name_path = argv[6];
        	}
        	else if(strncmp(argv[6], ".", 1) == 0){ //상대경로
                	for(int i = 0; i < strlen(argv[6]); i++){
                        	ntemp[i] = ntemp[i+1];
                	}
                	new_name_path = backup_dir;
                	strcat(real_path, ntemp);
        	}
        	else if(strcmp(argv[6], "P1") == 0 || strcmp(argv[6], current_dir) == 0){
                	new_name_path = current_dir;
       		}

        	else{ //파일명
                	char atemp[4096];
                	strcpy(atemp, current_dir);
               		strcat(atemp, "/");
                	strcat(atemp, argv[6]);
                	new_name_path = atemp;
        	}
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

        strcpy(dst_dir, home_dir);
        char *ptr = strchr(real_path, '/');

        for(int i = 0; i < 1; i++){
                ptr = strchr(ptr + 1, '/');
        }
        strcat(dst_dir, ptr);
        char *last_slash = strrchr(dst_dir, '/');
        if(last_slash != NULL){
                *last_slash = '\0';
        }

	if(strcmp(real_path, new_name_path) == 0)
		exit(1);

        if(access(real_path, R_OK|W_OK) != 0){
                fprintf(stderr, "Usage2 : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
                exit(1);
        }

        if(strlen(real_path) > 4096){
                fprintf(stderr, "<FILENAME> should be shorter than 4096 bytes\n");
                exit(1);
        }

        if(stat(real_path, &file_info) != 0){
                fprintf(stderr, "Usage3 : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
                exit(1);
        }

        if(S_ISREG(file_info.st_mode) != 1 && S_ISDIR(file_info.st_mode) != 1){
                fprintf(stderr, "<FILENAME> should be either regular file or directory\n");
                exit(1);
        }

        if(S_ISREG(file_info.st_mode) == 1 && argv[5] != NULL){
                fprintf(stderr, "regular file doesn't require any option\n");
                exit(1);
        }

        if(S_ISDIR(file_info.st_mode) == 1){
                if(argv[5] == NULL){
                        fprintf(stderr, "\"%s\" is a directory file\n", real_path);
                        exit(1);
                }
                else
                        if(strcmp(argv[5], "-d") != 0){
                                fprintf(stderr, "\"%s\" is a directory file\n", real_path);
                                exit(1);
                        }
        }

        //백업 코드
        //디렉토리
        if(S_ISDIR(file_info.st_mode) == 1){
               recover_directory(real_path, dst_dir);
        }
        //파일
        else{
                char recover_filename[4096];
                get_recover_filename(recover_filename, real_path, dst_dir);
                mkdir(dst_dir, 0777);
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
        	else{ //파일명
                	char btemp[4096];
                	strcpy(btemp, current_dir);
                	strcat(btemp, "/");
                	strcat(btemp, argv[4]);
                	real_path = btemp;
        	}
                recover_file1(real_path, recover_filename, dst_dir);
        }
        exit(0);
}

void get_recover_filename(char *backup_filename, char *original_filename, char *dst_file_path) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%y%m%d%H%M%S", &tm);
    char original_str[4096];
    strcpy(original_str, original_filename);
    char *filename = strrchr(original_str, '/') + 1;
    sprintf(backup_filename, "%s/%s_%s", dst_file_path, filename, time_str);
}

// 파일 복사
void recover_file1(char *src_path, char *dst_path, char *backup_dir) {
    int src_fd, dst_fd, n;
    char buf[1024];
    if(hash_check(src_path, backup_dir) == 1){
                        fprintf(stderr, "\"%s\" already recovered\n", dst_path);
                        return;
                }

    src_fd = open(src_path, O_RDONLY);
    dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
                write(dst_fd, buf, n);
    }
    printf("\"%s\" backuped\n", dst_path);
    close(src_fd);
    close(dst_fd);
}

// file recover
void recover_file2(char *src_path, char *dst_path, char *real_path) {
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

        while((entry = readdir(dir)) != NULL){
                if(entry->d_type == DT_REG){ //file
                        snprintf(filePath, sizeof(filePath), "%s/%s", dst_path, entry->d_name);
                        if(stat(filePath, &fileStat) == -1){
                                perror("error getting file status");
                                continue;
                        }

                        if(strncmp(filePath, src_path, strlen(src_path)) == 0){
                                fileinfo[nfiles].filelist = strdup(filePath);
                                fileinfo[nfiles].filesizelist = fileStat.st_size;
                                nfiles++;
                        }
                }
                else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){//directory
                        snprintf(filePath, sizeof(filePath), "%s/%s", dst_path, entry->d_name);
                        recover_file2(src_path, filePath, real_path);
                }
        }

        if(nfiles > 1){
                printf("backup file list of \"%s\"\n", real_path);
                printf("0. exit\n");
                for(int i = 0; i < nfiles; i++){
                        char *ptr;
                        ptr = strrchr(fileinfo[i].filelist, '_') + 1;

                        //file size
                        int len;
                        int j;
                        int a = 0;

                        char buffer[50];
                        sprintf(buffer, "%lld", fileinfo[i].filesizelist);
                        char *digits;

                        buffer[49] = 0;

                        len = strlen(buffer);
                        digits = buffer;

                        printf("%d. %s          ", i+1, ptr);
                        for(j = len; j > 0;){
                                printf("%c", *digits++);
                                j--;
                                if(j > 0 && (j % 3) == 0)
                                        printf(",");
                        }
                        printf("bytes\n");
                }
                printf("Choose file to recover\n>> ");
                fgets(input_str, sizeof(input_str), stdin);
                sscanf(input_str, "%d", &input);
                if(input == 0)
                        exit(1);

                else{
			recover_file2(fileinfo[input-1].filelist, real_path, dst_path);
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


// 디렉토리 복사
void recover_directory(char *src_path, char *dst_path) {
    DIR *src_dir, *dst_dir;
    struct dirent *dir_entry;
    struct stat dir_stat;
    char src_file_path[4096], dst_file_path[4097], backup_filename[4096];

    // 디렉토리 생성
    mkdir(dst_path, 0777);

    // 소스 디렉토리 열기
    src_dir = opendir(src_path);

    // 디렉토리 안의 파일 복사
    while ((dir_entry = readdir(src_dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(src_file_path, sizeof(src_file_path), "%s/%s", src_path, dir_entry->d_name);
        snprintf(dst_file_path, sizeof(dst_file_path), "%s/%s", dst_path, dir_entry->d_name);
        if (stat(src_file_path, &dir_stat) == -1) {
            perror("stat");
            continue;
        }
        if (S_ISDIR(dir_stat.st_mode)){
                recover_directory(src_file_path, dst_file_path);
        } else {
                get_recover_filename(backup_filename, src_file_path, dst_path);
                recover_file1(src_file_path, dst_file_path, dst_path);
        }
    }

    // 디렉토리 닫기
    closedir(src_dir);
}

int hash_check(char *file_path, char *backup_dir)
{
        //해시값 찾기
        FILE *fp;
        char md5_buffer[4096];
        char sha1_buffer[4096];
        unsigned char md5_hash[MD5_DIGEST_LENGTH+1];
        unsigned char sha1_hash[SHA_DIGEST_LENGTH+1];
        fp = fopen(file_path, "rb");
        if (fp == NULL) {
                printf("Error: could not open file\n");
                exit(1);
        }
       MD5((unsigned char *)md5_buffer, strlen(md5_buffer), md5_hash);
        SHA1((unsigned char *)sha1_buffer, strlen(sha1_buffer), sha1_hash);

        fclose(fp);

        //해시 검사
        DIR *dir = opendir(backup_dir);
        struct dirent *entry;
        char filepath[4092];
        char mdc_buffer[4096];
        char shac_buffer[4096];
        unsigned char mdc[MD5_DIGEST_LENGTH+1];
        unsigned char shac[SHA_DIGEST_LENGTH+1];
        int match = 0;

        if (dir == NULL) {
                perror("opendir");
                exit(1);
        }

        while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_REG) { // 파일인 경우에만 검사
                        sprintf(filepath, "%s/%s", backup_dir, entry->d_name);

                FILE *fp = fopen(filepath, "rb");
                if (fp == NULL) {
                        perror("fopen");
                        exit(1);
                }

                MD5((unsigned char *)mdc_buffer, strlen(mdc_buffer), mdc);
                SHA1((unsigned char *)shac_buffer, strlen(shac_buffer), shac);

                if (strcmp(md5_hash, mdc) == 0) {
                        match = 1;
                        return match;
                }

                if (strcmp(sha1_hash, shac) == 0) {
                        match = 1;
                        return match;
                }

                fclose(fp);
                }
        }

        closedir(dir);

        return 0;
}
