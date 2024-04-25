#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NAMESIZE 100

struct employee {
		char name[NAMESIZE];
		int salary;
		int pid;
};

int main(int argc, char *argv[])
{
		struct flock lock;
		struct employee record;
		int fd, recnum, pid;
		long position;

		if((fd = open(argv[1], O_RDWR)) == -1){
				perror(argv[1]);
				exit(1);
		}

		pid = getpid();
		for(;;){
				printf("\nEnter record number: ");
				scanf("%d", &recnum);
				if(recnum < 0)
						break;
				position = recnum * sizeof(record); // offset of the record the user is looking for
				lock.l_type = F_WRLCK; // write lock flag -> read from other program is locked
				lock.l_whence = 0;
				lock.l_start = position; // offset
				lock.l_len = sizeof(record); // lock one record that the user wants
				if(fcntl(fd, F_SETLKW, &lock) == -1){ // set record lock
						perror(argv[1]);
						exit(2);
				}
				lseek(fd, position, 0); // move offset to the position
				if(read(fd, (char *)&record, sizeof(record)) == 0){ // read one record start from position
						printf("record %d not found\n", recnum);
						lock.l_type = F_UNLCK; // unlock when the data is not found
						fcntl(fd, F_SETLK, &lock); // set unlock
						continue;
				}
				printf("Employee: %s, salary: %d\n", record.name, record.salary);
				record.pid = pid;
				printf("Enter new salary: ");
				scanf("%d", &record.salary);
				lseek(fd, position, 0); // move back to the beginning of the record
				write(fd, (char *)&record, sizeof(record)); // change the salary

				lock.l_type = F_UNLCK; // unlock flag
				fcntl(fd, F_SETLK, &lock);
		}
		close(fd);
}
