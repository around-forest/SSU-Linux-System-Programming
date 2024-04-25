#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

#define BUFLEN 1024
#define PATHLEN 1024
#define OPTIONS 5

// tree structure
typedef struct treeNode {
	struct treeNode *prev;
	char *path;
	mode_t mode;
	time_t mtime;
	bool isEnd;
	struct treeNode *parent;
	struct treeNode *child;
	struct treeNode *next;
} tree;

// global var
FILE *log_fp;
char *ID = "20212228";
char *monitor_list = "monitor_list.txt";
char monitor_list_path[PATHLEN];
char log_path[PATHLEN];
time_t prev_modified_time = 0;
volatile sig_atomic_t signal_received = 0;

// function prototype
void ssu_prompt(void);
void execute_command(int argc, char *argv[]);
void execute_add(int argc, char *argv[]);
void execute_delete(int argc, char *argv[]);
void execute_tree(int argc, char *argv[]);
void execute_help(int argc, char *argv[]);
void execute_exit(int argc, char *argv[]);
void init_daemon(char *dirpath, time_t mn_time);
pid_t make_daemon(void);
void monitor_directory(char *dirpath, time_t mn_time);
void log_event(tree *file_tree, char *event);
tree *create_node(char *path, mode_t mode, time_t mtime);
void make_tree(tree *dir, char *path);
void compare_tree(tree *old, tree *new);
void print_tree(tree *node, int depth);
void free_tree(tree *cur);
char *get_name(char *path);
void signal_handler(int signum);
int scandir_filter(const struct dirent *file);

// main
void main(void) {
	while(1)
		ssu_prompt();
	return;
}
// prompt
void ssu_prompt(void) {
	char input[BUFLEN];
	char *temp;
	char *argv[OPTIONS];
	int argc = 0;

	printf("%s> ", ID);
	fgets(input, BUFLEN, stdin);
	
	input[strcspn(input, "\n")] = '\0'; // remove '\n' from input array
	
	if(strcmp(input, "") == 0) // '\n' enter
		return;
	temp = strtok(input, " "); // get arguments for options
	while(temp != NULL) {
		argv[argc++] = temp;
		temp = strtok(NULL, " ");
	}
	// get monitor_list.txt path
	strcpy(monitor_list_path, getcwd(temp, PATHLEN));
	strcat(monitor_list_path, "/");
	strcat(monitor_list_path, monitor_list);

	// call func to distinguish arguments
	execute_command(argc, argv);
}
// arguments check func
void execute_command(int argc, char *argv[]) {
	if (!strcmp(argv[0], "add")) {
		execute_add(argc, argv);
	} else if (!strcmp(argv[0], "delete")) {
		execute_delete(argc, argv);	
	} else if (!strcmp(argv[0], "tree")) {
		execute_tree(argc, argv);
	} else if (!strcmp(argv[0], "help")) {
		execute_help(argc, argv);	
	} else if (!strcmp(argv[0], "exit")) {
		execute_exit(argc, argv);
		return;
	} else {
		fprintf(stderr, "wrong command in prompt\n");
		execute_help(argc, argv);
	}
	return;
}
// add menu -> init_daemon -> make_daemon
void execute_add(int argc, char *argv[]) {
	char dir_path[PATHLEN];
	char temp[PATHLEN];
	struct stat path_stat;
	int option_check = 0;
	int mn_time = 1;
	FILE *fp;
	// when <DIRPATH> is not given
	if(argc < 2 || argc == 3) {
		fprintf(stderr, "Usage : add <DIRPATH> [OPTION] <TIME>\n");
		return;
	}
	// get realpath
	realpath(argv[1], dir_path);
	// check existance of the path, and whether the path is directory or not
	if(stat(dir_path, &path_stat) == 0) {
		if(!(S_ISDIR(path_stat.st_mode))) {
			fprintf(stderr, "Usage : <DIRPATH> should be a directory\n");
			return;
		}
	} else {
		fprintf(stderr, "Usage : <DIRPATH> doesn't exist\n");
		return;
	}
	// [option] checking
	if(argc > 2 && !strcmp(argv[2], "-t"))
		option_check = 1;
	else if(argc > 2 && strcmp(argv[2], "-t") != 0) {
		fprintf(stderr, "Usage : [OPTION] should be \'-t\'\n");
		return;
	}
	// <TIME> checking
	if(option_check) {
		mn_time = atoi(argv[3]);
		if(mn_time == 0){
			fprintf(stderr, "Usage : <TIME> should be an integer\n");
			return;
		}
	}
	// check 'monitor_list.txt' in pwd. if exist, open. else create
	if((fp = fopen(monitor_list_path, "a+")) < 0) {
			fprintf(stderr, "fopen error for %s\n", monitor_list_path);
			return;
	}
	// check whether the given directory is already monitored -> use txt file
	while(fread(temp, 1, PATHLEN, fp) > 0) {
		if(strstr(temp, dir_path) != NULL){
			fprintf(stderr, "%s is already monitored\n", dir_path);
			fclose(fp);
			return;
		}
	}
	// start daemon process for the directory
	init_daemon(dir_path, mn_time);
	// save <DIRPATH> and daemon_pid in txt file
	printf("monitoring started (%s)\n", dir_path);
	return;
}

void execute_delete(int argc, char *argv[]) {
	FILE *fp;
	char temp[BUFLEN];
	char dirpath[BUFLEN];

	if(argc != 2) {
		fprintf(stderr, "Usage : delete <DAEMON_PID>\n");
		return;
	}
	
	// open 'monitor_list.txt' in pwd. if exist, open.
  if((fp = fopen(monitor_list_path, "r+")) < 0) {
    fprintf(stderr, "fopen error for %s\n", monitor_list_path);
    return;
	}
	// check whether the given daemon process pid is in 'monitor_list.txt'
  while(fgets(temp, BUFLEN, fp) != NULL) {
    if(strstr(temp, argv[1]) != NULL){
			strcpy(dirpath, temp);
			dirpath[strlen(temp) - strlen(argv[1]) - 2] = '\0';
			// send SIGUSR1 signal to the daemon process
			pid_t dpid = atoi(argv[1]);
			if(kill(dpid, SIGUSR1) == -1) {
				fprintf(stderr, "kil error to send SIGUSR1 to the daemon");
				fclose(fp);
				return;
			}
			// remove daemon process entry from 'monitor_list.txt'
			FILE *temp_fp = fopen("temp.txt", "w");
			if(temp_fp == NULL) {
				fprintf(stderr, "fopen error\n");
				fclose(fp);
				return;
			}
			// create new file to update 'monitor_list.txt'
			rewind(fp);
			while(fgets(temp, BUFLEN, fp) != NULL) {
				if(!strstr(temp, argv[1])) {
					fputs(temp, temp_fp);
				}
			}
			fclose(temp_fp);
			fclose(fp);
			// replace 'monitor_list.txt' with the updated temp file
			if(rename("temp.txt", monitor_list) == -1) {
				fprintf(stderr, "rename error for %s\n", monitor_list);
				return;
			}
			printf("monitoring ended (%s)\n", dirpath);
			return;
		}
	}
	fprintf(stderr, "daemon process with pid %s is not running\n", argv[1]);
	fclose(fp);
}	

void execute_tree(int argc, char *argv[]) {
	FILE *fp;
	char dirpath[PATHLEN];
	char temp[BUFLEN];

	if(argc != 2) {
		fprintf(stderr, "Usage : tree <DIRPATH>\n");
		return;
	}

	// open 'monitor_list.txt' in the current working directory
  if((fp = fopen(monitor_list_path, "r")) < 0) {
    fprintf(stderr, "fopen error for %s\n", monitor_list_path);
    return;
  }

	// get realpath
  if(realpath(argv[1], dirpath) == NULL) {
		fprintf(stderr, "damon process monitoring %s is not running\n", argv[1]);
		fclose(fp);
		return;
	}

	// check whether the given directory path is in 'monitor_list.txt'
  while(fgets(temp, BUFLEN, fp) != NULL) {
		if(strstr(temp, dirpath) != NULL) {
			// create the tree structure of the directory
			tree *root = create_node(dirpath, 0, 0); // create root directory
			make_tree(root, dirpath); // populate the tree

			// print the directory structure in tree format
			print_tree(root, 0);

			// free the allocated memory
			fclose(fp);
			return;
		}
	}
	fprintf(stderr, "damon process monitoring %s is not running\n", argv[1]);
	fclose(fp);
}

void execute_help(int argc, char *argv[]) {
	printf("Usage : ssu_monitor\n");
	printf("Options : \n");
	printf(" add <DIRPATH> [OPTION] <TIME>\n");
	printf(" delete <DAEMON_PID>\n");
	printf(" tree <DIRPATH>\n");
	printf(" help\n");
	printf(" exit\n");
}

void execute_exit(int argc, char *argv[]) {
	exit(0);
}
// start new daemon process
void init_daemon(char *dirpath, time_t mn_time) {
	int pid, dpid;
	FILE *fp1;
	char temp[BUFLEN];
	
	// make child process
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		return;
	}
	else if (pid == 0) { // child made
		if ((dpid = (make_daemon())) < 0) {
			fprintf(stderr, "make_daemon error\n");
			exit(0);
		}
		// when new daemon process started, write info of the daemon on 'monitor_list.txt'
		if((fp1 = fopen(monitor_list_path, "a")) == NULL) {
			fprintf(stderr, "fopen error for %s\n", monitor_list_path);
			exit(0);
		}
		sprintf(temp, "%s %d\n", dirpath, dpid);
		fputs(temp, fp1);
		fclose(fp1);
		// set log.txt file path
		sprintf(log_path, "%s/%s", dirpath, "log.txt");
		log_fp = fopen(log_path, "a");
		if(log_fp == NULL) {
			fprintf(stderr, "fopen error for %s\n", log_path);
			exit(0);
		}
		fclose(log_fp);
		// start monitoring
		while(!signal_received) {
			monitor_directory(dirpath, mn_time);
			sleep(mn_time);
		}
	
		printf("monitoring ended (%s)\n", dirpath);
		exit(0);
	}
	else
		return;
}
// make new daemon process
pid_t make_daemon(void) {
	pid_t pid;
	int fd, maxfd;
	// make child process
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid != 0)
		exit(0);
	// make new session
	setsid();
	// reset IO
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	maxfd = getdtablesize();
	for (fd = 0; fd < maxfd; fd++) //for debug , fd=3
		close(fd);
	// set authority
	umask(0);
	//	chdir("/");
	fd = open("/dev/null", O_RDWR);
	dup(0);
	dup(0);
	// return pid of daemon process
	return getpid();
}
// monitoring func
void monitor_directory(char *dirpath, time_t mn_time) {
	tree *root = create_node(dirpath, 0, 0);
	make_tree(root, dirpath);

	while(!signal_received) {
		tree *new_root = create_node(dirpath, 0, 0);
    make_tree(new_root, dirpath);
    compare_tree(root, new_root);
		free_tree(root);
    root = new_root;
    sleep(mn_time);
  }

  free_tree(root);
}
// log event
void log_event(tree *file_tree, char *event) {
	time_t modified_time = file_tree->mtime;
	struct tm *time_info = localtime(&modified_time);
	char timestamp[20];
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);

	log_fp = fopen(log_path, "a");
	if(log_fp == NULL) {
		fprintf(stderr, "fopen error for %s\n", log_path);
		return;
	}
	if(strstr(file_tree->path, ".swp") != NULL)
		return;
	fprintf(log_fp, "[%s][%s][%s]\n", timestamp, event, file_tree->path);
	fclose(log_fp);
}

tree *create_node(char *path, mode_t mode, time_t mtime) {
	tree *new;

	new = (tree *)malloc(sizeof(tree));
	new->path = strdup(path);
	new->isEnd = false;
	new->mode = mode;
	new->mtime = mtime;
	new->next = NULL;
	new->prev = NULL;
	new->child = NULL;
	new->parent = NULL;

	return new;
}

void make_tree(tree *dir, char *path) {
	int count, i;
	struct dirent **filelist;
	if ((count = scandir(path, &filelist, scandir_filter, alphasort)) < 0) {
		fprintf(stderr, "in function make_tree: scandir error for %s\n", path);
		return;
	}
	
	for (i = 0; i < count; i++) {
		if (strcmp(filelist[i]->d_name, ".") == 0 || strcmp(filelist[i]->d_name, "..") == 0) {
      free(filelist[i]);
      continue;
    }
        
    char file_path[PATH_MAX];
    snprintf(file_path, sizeof(file_path), "%s/%s", path, filelist[i]->d_name);

    struct stat file_stat;
    if (lstat(file_path, &file_stat) < 0) {
			fprintf(stderr, "lstat error for %s\n", file_path);
      free(filelist[i]);
      continue;
    }

    tree *new_node = create_node(file_path, file_stat.st_mode, file_stat.st_mtime);
    new_node->parent = dir;

    if (S_ISDIR(file_stat.st_mode)) {
			// recursive call to make_tree for subdirectories
			make_tree(new_node, file_path);
		}

		if (dir->child == NULL) {
      dir->child = new_node;
    } else {
      tree *child = dir->child;
      while (child->next != NULL) {
        child = child->next;
      }
      child->next = new_node;
      new_node->prev = child;
    }

    free(filelist[i]);
	}
	free(filelist);
}

void compare_tree(tree *old, tree *new) {
	if(old == NULL || new == NULL) {
		return;
	}

	tree *old_child = old->child;
  tree *new_child = new->child;
	
	while (old_child != NULL || new_child != NULL) {
    if (old_child == NULL) {
      log_event(new_child, "create");
			if(S_ISDIR(new_child->mode)) {
				compare_tree(NULL, new_child); // recursively compare subdirectories
			}
      new_child = new_child->next;
    } else if (new_child == NULL) {
      log_event(old_child, "remove");
      old_child = old_child->next;
    } else {
      int cmp = strcmp(old_child->path, new_child->path);
      if (cmp == 0) {
				if ((old_child->mtime != new_child->mtime) && !S_ISDIR(old_child->mode) && !S_ISDIR(new_child->mode)) {
          log_event(new_child, "modify");
        }
				if(S_ISDIR(old_child->mode) && S_ISDIR(new_child->mode)) {
					compare_tree(old_child, new_child); // recursively compare subdirectories
				}
        old_child = old_child->next;
        new_child = new_child->next;
      } else if (cmp < 0) {
        log_event(old_child, "remove");
        old_child = old_child->next;
      } else {
        log_event(new_child, "create");
				if (S_ISDIR(new_child->mode)) {
					compare_tree(NULL, new_child); // recursively compare subdirectories
				}
        new_child = new_child->next;
      }
    }
  }
}

void print_tree(tree *node, int depth) {
	if (node == NULL) return;
	// get the file or directory name
	char *name = get_name(node->path);
	for(int i = 0; i < depth; i++) {
		printf("---");
	}
	printf("%s\n", name);
	free(name);	

	print_tree(node->child, depth + 1);
	print_tree(node->next, depth);
}

void free_tree(tree *cur) {
	if (cur == NULL) {
		return;
	}

	free_tree(cur->child);
	free_tree(cur->next);
	free(cur->path);
	free(cur);
}

char *get_name(char *path) {
	char *name;
	char *last_slash = strrchr(path, '/');

	if(last_slash != NULL) {
		name = strdup(last_slash + 1); // extract name
	} else {
		name = strdup(path);
	}

	return name;
}
// SIGUSR1 signal received, stop monitoring
void signal_handler(int signum) {
	if(signum == SIGUSR1) {
		signal_received = 1;
	}
}

int scandir_filter(const struct dirent *file) {
	if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")
			|| !strcmp(file->d_name, "log.txt")
			|| !strcmp(file->d_name, monitor_list)) {
		return 0;
	}
	else
		return 1;
}
