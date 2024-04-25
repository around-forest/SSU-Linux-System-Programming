#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main()
{
		printf("Alarm Setting\n");
		signal(SIGALRM, ssu_alarm); // set handler for SIGALRM
		alarm(2); // handler called

		while(1){
				printf("done\n");
				pause(); // pause until get alarm signal
				alarm(2);
		}

		exit(0);
}

void ssu_alarm(int signo){
		printf("alarm..!!!\n");
}
