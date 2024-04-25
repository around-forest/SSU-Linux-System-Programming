#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
		sigset_t set; // sigset_t

		sigemptyset(&set); // set sigset_t empty
		sigaddset(&set, SIGINT); // add SIGINT into set (change 0 to 1)

		switch(sigismember(&set, SIGINT))
		{
				case 1:
						printf("SIGINT is included. \n");
						break;
				case 0:
						printf("SIGINT is not included. \n");
						break;
				default :
						printf("failed to call sigismember() \n");
		}

		switch(sigismember(&set, SIGSYS)) // which should be 0 due to sigemptyset
		{
				case 1:
						printf("SIGSYS is included. \n");
						break;
				case 0:
						printf("SIGSYS is not included. \n");
						break;
				default : 
						printf("failed to call sigismember() \n");
		}

		exit(0);
}
