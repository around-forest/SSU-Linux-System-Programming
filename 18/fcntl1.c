#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
		int testfd;
		int fd;

		fd = open("test.txt", O_CREAT); // create new txt file

		testfd = fcntl(fd, F_DUPFD, 5); // copy file descriptor to 5
		printf("testfd : %d\n", testfd);
		testfd = fcntl(fd, F_DUPFD, 5); // copy descriptor to 5, but already copied
		printf("testfd : %d\n", testfd); // thus, it should be copied to 6

		getchar(); // fresh buffer
}
