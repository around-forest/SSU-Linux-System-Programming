#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void check_pending(int signo, char *signame);
void handler(int signo);

int main()
{
		struct sigaction sig_act;
		sigset_t sig_set;

		sigemptyset(&sig_act.sa_mask);
		sig_act.sa_flags = 0;
		sig_act.sa_handler = handler;

		if(sigaction(SIGUSR1, &sig_act, NULL) != 0){
				fprintf(stderr, "sigaction() error\n");
				exit(1);
		}
		else {
				sigemptyset(&sig_set); // empty set
				sigaddset(&sig_set, SIGUSR1); // add SIGUSR1

				if(sigprocmask(SIG_SETMASK, &sig_set, NULL) != 0){ // block set
						fprintf(stderr, "sigprocmask() error\n");
						exit(1);
				}
				else{
								printf("SIGUSR1 signals are now blocked\n");
								kill(getpid(), SIGUSR1);
								printf("after kill()\n");
								check_pending(SIGUSR1, "SIGUSR1"); // check blocked signal
								sigemptyset(&sig_set); // empty set
								sigprocmask(SIG_SETMASK, &sig_set, NULL); // release blocked set
								printf("SIGUSR1 signals are no longer blocked\n");
								check_pending(SIGUSR1, "SIGUSR1");
				}
		}
		exit(0);
}

void check_pending(int signo, char *signame){
		sigset_t sig_set;

		if(sigpending(&sig_set) != 0)
				printf("sigpending() error\n");
		else if(sigismember(&sig_set, signo))
				printf("a %s signal is pending\n", signame);
		else
				printf("%s signals are not pending\n", signame);
}

void handler(int signo){
		printf("in handler function\n");
}
