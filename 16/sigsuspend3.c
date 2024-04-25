#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void handler(int signo);
void timestamp(char *str);

int main()
{
		struct sigaction sig_act;
		sigset_t blk_set;

		sigfillset(&blk_set); // fill blk set
		sigdelset(&blk_set, SIGALRM); // delete SIGALRM from blk set
		sigemptyset(&sig_act.sa_mask); // empty sig act
		sig_act.sa_flags = 0;
		sig_act.sa_handler = handler; // set sig act handler as handler func
		sigaction(SIGALRM, &sig_act, NULL); // SIGALRM, sa_handler is called
		timestamp("before sigsuspend()");
		alarm(5); // SIGALRM after 5
		sigsuspend(&blk_set); // pending until signal in blk set is called -> SIGALRM
		timestamp("after sigsuspend()");
		exit(0);
}

void handler(int signo){
		printf("in ssu_signal_handler() function\n");
}

void timestamp(char *str){
		time_t time_val;

		time(&time_val);
		printf("%s the time is %s\n", str, ctime(&time_val)); // print current time
}
