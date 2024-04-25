#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signo);

int count = 0;

int main()
{
		signal(SIGALRM, signal_handler); // alarm -> handler
		alarm(1); //set alarm

		while(1);

		exit(0);
}

void signal_handler(int signo) {
		printf("alarm %d\n", count++);
		alarm(1);
}
