#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int daemon_init(void);

int main()
{
		pid_t pid;

		pid = getpid();
		printf("parent process : %d\n", pid);
		printf("daemon process initialization\n");

		if(daemon_init() < 0){ // call daemon process
				fprintf(stderr, "ssu_daemon_init failed\n");
				exit(1);
		}
		exit(0);
}

int daemon_init(void){
		pid_t pid;
		int fd, maxfd;

		if((pid = fork()) < 0){ // fork child process which is daemon process
				fprintf(stderr, "fork error\n");
				exit(1);
		}
		else if(pid != 0)
				exit(0); // exit when process is not child?

		pid = getpid(); // daemon process pid
		printf("process %d running as daemon\n", pid);
		setsid(); // set session leader
		signal(SIGTTIN, SIG_IGN); // let IO signals ignored
		signal(SIGTTOU, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		maxfd = getdtablesize(); // get number of fds

		for(fd = 0; fd < maxfd; fd++)
				close(fd); // close all fd
		
		umask(0); // set mask 0
		chdir("/"); // move wd to root directory
		fd = open("/dev/null", O_RDWR); // set fd 0, 1 ,2 to /dev/null
		dup(0);
		dup(0);
		return 0;
}
