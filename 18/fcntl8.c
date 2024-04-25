#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF 100
#define DUMMY 0

struct ssu_employee {
		char name[BUF];
		int salary;
		int pid;
};

int main(int argc, char *argv[])
{
		struct ssu_employee record;
		int fd;
		int flags;
		int length;
		int pid;

		if(argc<2){
				fprintf(stderr, "Usage : %s file\n", argv[0]);
				exit(1);
		}

		if((fd = open(argv[1], O_RDWR)) < 0){
				fprintf(stderr, "open error for %s\n", argv[1]);
				exit(1);
		}

		if((flags = fcntl(fd, F_GETFL, DUMMY)) == -1){ // get flags of fd
				fprintf(stderr, "fcntl F_GETFL error\n");
				exit(1);
		}

		flags |= O_APPEND; // set O_APPEND flag

		if(fcntl(fd, F_SETFL, flags) == -1){ // set O_APPEND flag
				fprintf(stderr, "fcntl F_SETFL error\n");
				exit(1);
		}

		pid = getpid(); // get pid of working process

		while(1){
				printf("Enter employee name : ");
				scanf("%s", record.name);

				if(record.name[0] == '.')
						break;

				printf("Enter employee salary : ");
				scanf("%d", &record.salary);
				record.pid = pid;
				length = sizeof(record);

				if(write(fd, (char *)&record, length) != length){ // write record to fd
						fprintf(stderr, "record write error\n");
						exit(1);
				}
		}

		close(fd);
		exit(0);
}
