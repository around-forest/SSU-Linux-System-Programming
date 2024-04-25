#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
		int fd;
		int val;

		if((fd = open("exec_copy.txt", O_CREAT)) < 0){
				fprintf(stderr, "open error for %s\n", "exec_copy.txt");
				exit(1);
		}
		val = fcntl(fd, F_GETFD, 0); // get fd of the open file

		if(val & FD_CLOEXEC) // check whether the bit is already on or not
				printf("close-on-exec bit on\n");
		else
				printf("close-on-exec bit off\n");

		val |= FD_CLOEXEC; // turn on the close-on-exec flag

		if(val & FD_CLOEXEC) // check the bit after turning on
				printf("close-on-exec bit on\n");
		else
				printf("close-on-exec bit off\n");

		fcntl(fd, F_SETFD, val); // set the close-on-exec bit
		execl("/home/noahavtuallya/LSP/18/loop", "./loop", NULL);
}
