#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static void handler1(int signo);
static void handler2(int signo);

int main()
{
		struct sigaction act_int, act_quit;

		act_int.sa_handler = handler1; // set handler1
		sigemptyset(&act_int.sa_mask); // empty act_int
		sigaddset(&act_int.sa_mask, SIGQUIT); // SIGQUIT -> 1
		act_quit.sa_flags = 0;

		if (sigaction(SIGINT, &act_int, NULL) < 0){
				fprintf(stderr, "sigaction(SIGINT) error\n");
				exit(1);
		}

		act_quit.sa_handler = handler2; // set handler2
		sigemptyset(&act_quit.sa_mask); // empty act_quit
		sigaddset(&act_quit.sa_mask, SIGINT); // SIGINT -> 1
		act_int.sa_flags = 0;

		if(sigaction(SIGQUIT, &act_quit, NULL) < 0){
				fprintf(stderr, "sigaction(SIGQUIT) error\n");
				exit(1);
		}

		pause();
		exit(0);
}

static void handler1(int signo){
		printf("Signal handler of SIGINT : %d\n", signo);
		printf("SIGQUIT signal is blocked : %d\n", signo);
		printf("sleeping 3 sec\n");
		sleep(3);
		printf("Signal handler of SIGINT ended\n");
}

static void handler2(int signo){
		printf("Signal handler of SIGQUIT : %d\n", signo);
    printf("SIGINT signal is blocked : %d\n", signo);
    printf("sleeping 3 sec\n");
    sleep(3);
    printf("Signal handler of SIGQUIT ended\n");
}

