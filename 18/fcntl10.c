#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
		struct flock lock;
		int fd;
		char command[100];

		if((fd = open(argv[1], O_RDWR)) == -1){
				perror(argv[1]);
				exit(1);
		}

		lock.l_type = F_WRLCK; // write lock -> read locked
		lock.l_whence = 0;
		lock.l_start = 0L; // offset
		lock.l_len = 0L; // lock to EOF

		if(fcntl(fd, F_SETLK, &lock) == -1){ // set lock flag
				if(errno == EACCES){ // authority error
						printf("%s busy -- try later\n", argv[1]); // occur when the file is locked
						exit(2);
				}
				perror(argv[1]);
				exit(3);
		}
		sprintf(command, "vim %s\n", argv[1]); // string for system()
		system(command);
		lock.l_type = F_UNLCK; // unlock flag
		fcntl(fd, F_SETLK, &lock); // set unlock
		close(fd);

		return 0;
}
