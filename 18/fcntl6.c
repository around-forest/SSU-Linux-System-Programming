#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
		int fd;
		int val;

		fd = open("exec_copy.txt", O_CREAT); // create new txt file
		execl("/home/noahactuallya/LSP/18/loop", "./loop", NULL);
		exit(0);
}
