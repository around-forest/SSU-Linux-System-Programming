#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	char input[512];
	char *tok[10];
	char *cmd;
	char current_buffer[512] = {0, };
	char *current_dir = NULL;
	char backup_buffer[512] = {0, };
	char *backup_dir = NULL;
	char *home_dir = NULL;
	pid_t pid;

	if(argc != 2){
		fprintf(stderr, "Usage: ssu_backup <md5 | sha1>\n");
		exit(1);
	}

	if(strcmp(argv[1], "md5") != 0 && strcmp(argv[1], "sha1") != 0){
		fprintf(stderr, "Usage: ssu_backup <md5 | sha1>\n");
		exit(1);
	}

	else{
		home_dir = getenv("HOME");
		current_dir = getcwd(current_buffer, 512);
// backup directory create
		chdir(home_dir);
		if(mkdir("backup", 0776) == 0)
			printf("Backup Directory created\n");
		else
			printf("Backup Directory already exist\n");
		chdir("backup");
		backup_dir = getcwd(backup_buffer, 512);
		chdir(current_dir);


		while(1){ 
//args[0] -> hash, args[1] -> home_dir, args[2] -> backup_dir, args[3] -> current_dir, [4] -> filename, [5] -> option
			printf("\n20212228> ");
			fgets(input, 512, stdin);

			if(strcmp(input, "\n") == 0)
				continue;

			input[strlen(input)-1] = '\0';

			tok[0] = strtok(input, " ");
			tok[1] = strtok(NULL, " "); //filename
			tok[2] = strtok(NULL, " "); //opion1
			tok[3] = strtok(NULL, " "); //option2 or newname

			cmd = tok[0];

			//add
			if(strstr(cmd, "add") != 0){
				if((pid = fork()) == 0){
					char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

					if(execv("./cmd_add", args) < 0){
						fprintf(stderr, "execv add error\n");
						continue;
					}
				}
				
				if(wait(NULL) == pid){
					printf("add process exited\n");
				}
				else{
					fprintf(stderr, "wait error\n");
				}
			}

			//remove
			else if(strstr(cmd, "remove") != 0){
                                if((pid = fork()) == 0){
                                        char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], tok[3], NULL};

                                        if(execv("./cmd_remove", args) < 0){
                                                fprintf(stderr, "execv remove error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("remove process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }

			//recover
			else if(strstr(cmd, "recover") != 0){
                                if((pid = fork()) == 0){
                                        char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], tok[3], NULL};

                                        if(execv("./cmd_recover", args) < 0){
                                                fprintf(stderr, "execv recover error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("recover process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }

			//ls
			else if(strstr(cmd, "ls") != 0){
                                if((pid = fork()) == 0){
                                        char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

                                        if(execv("./cmd_ls", args) < 0){
                                                fprintf(stderr, "execv ls error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("ls process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }

			//vim
			else if(strstr(cmd, "vim") != 0){
                                if((pid = fork()) == 0){
                                        char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

                                        if(execv("./cmd_vim", args) < 0){
                                                fprintf(stderr, "execv vim error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("vim process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }

			//vi
			else if(strstr(cmd, "vi") != 0){
                                if((pid = fork()) == 0){
                                        char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

                                        if(execv("./cmd_vi", args) < 0){
                                                fprintf(stderr, "execv vi error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("vi process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }


			//help
			else if(strstr(cmd, "help") != 0){
                                if((pid = fork()) == 0){
					char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

                                        if(execv("./cmd_help", args) < 0){
                                                fprintf(stderr, "execv help error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("help process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }
			//exit
			else if(strstr(cmd, "exit") != 0){
				exit(0);
                        }

			else{
                                if((pid = fork()) == 0){
					char *args[] = {argv[1], home_dir, backup_dir, current_dir, tok[1], tok[2], NULL};

                                        if(execv("./cmd_help", args) < 0){
                                                fprintf(stderr, "execv help error\n");
                                                continue;
                                        }
                                }

                                if(wait(NULL) == pid){
                                        printf("help process exited\n");
                                }
                                else{
                                        fprintf(stderr, "wait error\n");
                                }
                        }
		}
	}
}
