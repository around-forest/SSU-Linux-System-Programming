#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
		int flag;

		if((flag = fcntl(STDOUT_FILENO, F_DUPFD)) == -1){ // not enough argument for F_DUPFD
				fprintf(stderr, "Error : Checking CLOSE_ON_EXEC\n");
				exit(1);
		}

		printf("CLOSE_ON_EXEC flag is = %d\n", flag);
		exit(0);
}
