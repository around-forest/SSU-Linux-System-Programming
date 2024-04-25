#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int signo) {
		printf("handler control\n");
}

int main()
{
		struct sigaction sig_act;
		sigset_t sig_set;

		sigemptyset(&sig_act.sa_mask); // empty set
		sig_act.sa_flags = 0; // flag set 0
		sig_act.sa_handler = handler;
		sigaction(SIGUSR1, &sig_act, NULL); // action change
		printf("before first kill()\n");
		kill(getpid(), SIGUSR1);
		sigemptyset(&sig_set); // empty sig set
		sigaddset(&sig_set, SIGUSR1); // SIGUSR1 1
		sigprocmask(SIG_SETMASK, &sig_set, NULL); // block set
		printf("before second kill()\n");
		kill(getpid(), SIGUSR1); // call handler
		printf("after second kill()\n");
		exit(0);
}
