#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#include <errno.h>

#define NAMESIZE 100
#define MAXTRIES 5

struct employee{
		char name[NAMESIZE];
		int salary;
		int pid;
};

int main(int argc, char *argv[])
{
		struct flock lock;
		struct employee record;
		int fd, sum = 0, try = 0;

		sleep(10);

		if((fd = open(argv[1], O_RDONLY)) == -1) {
				perror(argv[1]);
				exit(1);
		}

		lock.l_type = F_RDLCK; // set read lock -> write is locked
		lock.l_whence = 0; // SEEK_SET
		lock.l_start = 0L; // offset
		lock.l_len == 0L; // lock to EOF

		while(fcntl(fd, F_SETLK, &lock) == -1){ // set lock
				if(errno == EACCES){ // EACCES = authority error -> still locked
						if(try++ < MAXTRIES){
								sleep(1);
								continue;
						}
						printf("%s busy -- try later\n", argv[1]);
						exit(2);
				}
				perror(argv[1]);
				exit(3);
		}
		sum = 0;
		while(read(fd, (char *)&record, sizeof(record)) > 0){
				printf("Employee: %s, Salary: %d\n", record.name, record.salary);
				sum += record.salary;
		}
		printf("\nTotal Salary: %d\n", sum);

		lock.l_type = F_UNLCK; // set unlock flag
		fcntl(fd, F_SETLK, &lock); // set the flag
		close(fd);
}

