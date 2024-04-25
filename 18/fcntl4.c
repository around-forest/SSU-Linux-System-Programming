#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd, int flags);
void clr_flags(int fd, int flags);

char buf[500000];

int main()
{
		int ntowrite, nwrite;
		char *ptr;

		ntowrite = read(STDIN_FILENO, buf, sizeof(buf)); // read STDIN
		fprintf(stderr, "reading %d bytes\n", ntowrite); // 500000

		set_flags(STDOUT_FILENO, O_NONBLOCK); // set nonblocking for STDOUT

		ptr = buf;
		while(ntowrite > 0) {
				errno = 0;
				nwrite = write(STDOUT_FILENO, ptr, ntowrite); // print what is in buf
				fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

				if(nwrite > 0){
						ptr += nwrite;
						ntowrite -= nwrite;
						}
		}
		clr_flags(STDOUT_FILENO, O_NONBLOCK); // stop nonblocking
		exit(0);
}

void set_flags(int fd, int flags)
{
		int val;

		if((val = fcntl(fd, F_GETFL, 0)) < 0){ // get file status flag
				fprintf(stderr, "fcntl F_GETFL failed\n");
				exit(1);
		}

		val |= flags; // set on the bit of nonblocking

		if(fcntl(fd, F_SETFL, val) < 0){ // start file status flag nonblocking
				fprintf(stderr, "fcntl F_SETFL failed\n");
				exit(1);
		}
}

void clr_flags(int fd, int flags)
{
		int val;

		if((val = fcntl(fd, F_GETFL, 0)) < 0){ // get file status flag
				fprintf(stderr, "fcntl F_GETFL failed\n");
				exit(1);
		}

		val &= ~flags; // turn off bit of the noblocking bit

		if(fcntl(fd, F_SETFL, val) < 0){ // reset the flag
				fprintf(stderr, "fcntl F_SETFL failed\n");
				exit(1);
		}
}
