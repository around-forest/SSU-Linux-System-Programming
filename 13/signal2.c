#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void signal_handler(int signo);

int main()
{
		if (signal(SIGINT, signal_handler) == SIG_ERR) {
				fprintf(stderr, "cannot handle SIGINT\n");
				exit(EXIT_FAILURE);
		} // SIGINT handler 등록

		if (signal(SIGTERM, signal_handler) == SIG_ERR) {
				fprintf(stderr, "cannot handle SIGTERM\n");
				exit(EXIT_FAILURE);
		} // SIGTERM handler 등록

		if (signal(SIGPROF, SIG_DFL) == SIG_ERR) {
				fprintf(stderr, "cannot reset SIGPROF\n");
				exit(EXIT_FAILURE);
		} // SIGPROF를SIG_DFL로 등록 

		if(signal(SIGHUP, SIG_IGN) == SIG_ERR) {
				fprintf(stderr, "cannot ignore SIGHUP\n");
				exit(EXIT_FAILURE);
		} // SIGHUP를 SIG_IGN로 등록

		while(1)
				pause();

		exit(0);
}

static void signal_handler(int signo) {
		if (signo == SIGINT) // SIGINT 발생 시 출력
				printf("caught SIGINT\n");
		else if (signo == SIGTERM) // SIGTERM 발생 시 출력
				printf("caught SIGTERM\n");
		else {
				fprintf(stderr, "unexpected signal\n");
				exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
}
