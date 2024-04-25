#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

static void handler1(int signo);
static void handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main()
{
		struct sigaction act_sig1;
		struct sigaction act_sig2;
		int i, ret;

		printf("My PID is %d\n", getpid());
		ret = sigsetjmp(jmp_buf1, 1); // set jmp 1

		if(ret == 0){
				act_sig1.sa_handler = handler1; // set handler 1
				sigaction(SIGINT, &act_sig1, NULL);
		}
		else if (ret == 3)
				printf("---------------\n");

		printf("Starting\n");
		sigsetjmp(jmp_buf2, 2); // set jmp 2
		act_sig2.sa_handler = handler2; // set handler 2
		sigaction(SIGUSR1, &act_sig2, NULL);

		for (int i = 0; i < 20; i ++){
				printf("i = %d\n", i);
				sleep(1);
		}
		exit(0);
}

static void handler1(int signo){
		fprintf(stderr, "\nInterrupted\n");
		siglongjmp(jmp_buf1, 3); // back to setjmp 1
}

static void handler2(int signo){
		fprintf(stderr, "\nSIGUSR1\n");
		siglongjmp(jmp_buf2, 2); //back to setjmp 2
}
