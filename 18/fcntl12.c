#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NAMESIZE 100

struct employee{
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
		char ans[5];

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

				position = recnum * sizeof(record); // offset of the record the user entered
				lock.l_type = F_RDLCK; // read lock -> write from other locked
				lock.l_whence = 0;
				lock.l_start = position; // offset
				lock.l_len = sizeof(record); // one record

				if(fcntl(fd, F_SETLKW, &lock) == -1){ // set read lock, wait if it is already locked
						perror(argv[1]);
						exit(2);
				}
				lseek(fd, position, 0); // move offset to the position
				if(read(fd, (char *)&record, sizeof(record)) == 0){ // read one record
						printf("record %d not found\n", recnum);
						lock.l_type = F_UNLCK; // unlock flag
						fcntl(fd, F_SETLK, &lock); // set unlock
						continue;
				}
				printf("Employee: %s, salary: %d\n", record.name, record.salary);
				printf("Do you want to update salary (y or n)? ");
				scanf("%s", ans);

				if(ans[0] != 'y'){ // if not, unlock the record
						lock.l_type = F_UNLCK;
						fcntl(fd, F_SETLK, &lock); // set unlock
						continue;
				}
				lock.l_type = F_WRLCK; // write lock flag -> read from other is locked
				if(fcntl(fd, F_SETLKW, &lock) == -1){ //set write lock, wait if it is already locked
						perror(argv[1]);
						exit(3);
				}
				record.pid = pid;
				printf("Enter new salary: ");
				scanf("%d", &record.salary); // get new salary

				lseek(fd, position, 0); // go back to the beginning of the record
				write(fd, (char *)&record, sizeof(record)); // write the new salary

				lock.l_type = F_UNLCK; // unlock flag
				fcntl(fd, F_SETLK, &lock); // set the flag
		}
		close(fd);
}
