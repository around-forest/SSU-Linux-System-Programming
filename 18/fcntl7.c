#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
		char *filename = "ssu_test.txt";
		int fd1, fd2;
		int flag;

		if ((fd1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0) { // set O_RDWR, O_APPEND when open
				fprintf(stderr, "open error for %s\n", filename);
				exit(1);
		}
		if(fcntl(fd1, F_SETFD, FD_CLOEXEC) == -1){ // set FD_CLOEXEC flag
				fprintf(stderr, "fcntl F_SETFL error\n");
				exit(1);
		}
		if((flag = fcntl(fd1, F_GETFL, 0)) == -1){ // get file status flag
				fprintf(stderr, "fcntl F_GETFL error\n");
				exit(1);
		}
		if(flag & O_APPEND) // check whether O_APPEND is set
				printf("fd1 : O_APPEND flag is set.\n");
		else
				printf("fd1 : O_APPEND flag is NOT set.\n");

		if((flag = fcntl(fd1, F_GETFD, 0)) == -1){ // get descriptor flag
				fprintf(stderr, "fcntl F_GETFD error\n");
				exit(1);
		}

		if(flag & FD_CLOEXEC) // check whether FD_CLOEXEC is set
				printf("fd1 : FD_CLOEXEC flag is set.\n");
		else
				printf("fd1 : FD_CLOEXEC flag is not set.\n");

		if((fd2 = fcntl(fd1, F_DUPFD, 0)) == -1){ // copy fd1 to fd2, but FD_CLOEXEC doesn't copied
				fprintf(stderr, "fcntl F_DUPFD error\n");
				exit(1);
		}
		if((flag = fcntl(fd2, F_GETFL, 0)) == -1){ // get the flag of fd2 copied from fd1
				fprintf(stderr, "fcntl F_GETFL error\n");
				exit(1);
		}

		if(flag & O_APPEND) // check flag from fd2
				printf("fd2 : O_APPEND flag is set.\n"); // this should be printed
		else
				printf("fd2 : O_APPEND flag is not set.\n");

		if((flag = fcntl(fd2, F_GETFD, 0)) == -1){ // get flag to check close-on-exec
				fprintf(stderr, "fcntl F_GETFD error\n");
				exit(1);
		}

		if(flag & FD_CLOEXEC) // check the close-on-exec bit
				printf("fd2 : FD_CLOEXEC flag is set.\n");
		else
				printf("fd2 : FD_CLOEXEC flag is not set.\n");

		exit(0);
}
