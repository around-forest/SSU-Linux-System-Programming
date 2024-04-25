#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
		sigset_t old_set;
		sigset_t sig_set;

		sigemptyset(&sig_set); // empty sig set
		sigaddset(&sig_set, SIGINT); // add SIGINT into sig_set
		sigprocmask(SIG_BLOCK, &sig_set, &old_set); // block sig set (SIGINT) and save it in old_set
		sigsuspend(&old_set); // suspended until SIGINT
		exit(0);
}
