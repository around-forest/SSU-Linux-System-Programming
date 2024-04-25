#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include <errno.h>

static void ssu_alarm(int signo);
static void func(int signo);
void mask(const char *str);

static volatile sig_atomic_t can_jump;
static sigjmp_buf jump_buf;

int main()
{
		if(signal(SIGUSR1, func) == SIG_ERR){ // set signal
				fprintf(stderr, "SIGUSR1 error\n");
				exit(1);
		}

		if(signal(SIGALRM, ssu_alarm) == SIG_ERR){ // set signal alarm
				fprintf(stderr, "SIGALRM error\n");
				exit(1);
		}

		mask("starting main: ");

		if(sigsetjmp(jump_buf, 1)){ // set jump here
				mask("ending main: ");
				exit(0);
		}

		can_jump = 1;

		while(1)
				pause();

		exit(0);
}

void mask(const char *str){
		sigset_t sig_set;
		int err_num;

		err_num = errno;

		if(sigprocmask(0,NULL,&sig_set) < 0){ // set procmask
				printf("sigprocmask() error");
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

static void func(int signo){
		time_t start_time;

		if(can_jump == 0)
				return;

		mask("starting func: ");
		alarm(3);
		start_time = time(NULL); // current time

		while(1)
				if(time(NULL) > start_time + 5)
						break;
		mask("ending func: ");
		can_jump = 0;
		siglongjmp(jump_buf, 1); // move back to setjmp
}

static void ssu_alarm(int signo){
		mask("in alarm: ");
}
