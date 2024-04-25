#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static void func(int signo);
void print_mask(const char *str);

int main()
{
		sigset_t new_mask, old_mask, wait_mask;

		print_mask("program start: "); // call print_mask

		if(signal(SIGINT, func) == SIG_ERR) { // set handler
				fprintf(stderr, "siganl(SIGINT) error\n");
				exit(1);
		}

		sigemptyset(&wait_mask); // empty wait mask
		sigaddset(&wait_mask, SIGUSR1); // add SIGUSR1 to wait mask
		sigemptyset(&new_mask); // empty new mask
		sigaddset(&new_mask, SIGINT); // add SIGINT ot new mask

		if(sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0){ // block new mask and save it in old mask
				fprintf(stderr, "SIG_BLOCK() error\n");
				exit(1);
		}

		print_mask("in critical region: ");

		if(sigsuspend(&wait_mask) != -1){ // wait until signal in wait mask called
				fprintf(stderr, "sigsuspend() error\n");
				exit(1);
		}

		print_mask("after return from sigsuspend: ");

		if(sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0){ // unblock blocked mask
				fprintf(stderr, "SIG_SETMASK() error\n");
				exit(1);
		}

		print_mask("program exit: ");
		exit(0);
}

void print_mask(const char *str){
		sigset_t sig_set;
		int err_num;

		err_num = errno;

		if(sigprocmask(0, NULL, &sig_set) < 0){ // save current mask in sig_set
				fprintf(stderr, "sigprocmask() error\n");
				exit(1);
		}

		printf("%s", str);

		if(sigismember(&sig_set, SIGINT))
				printf("SIGINT ");

		if(sigismember(&sig_set, SIGQUIT))
				printf("SIGQUIT ");

		if(sigismember(&sig_set, SIGUSR1))
				printf("SIGUSR1 ");

		if(sigismember(&sig_set, SIGALRM))
				printf("SIGALRM ");

		printf("\n");
		errno = err_num;
}

static void func(int signo){ // if Ctrl + c, handler func is called, then wait mask will be saved in sig set
		print_mask("\nin ssu_func: ");
}
