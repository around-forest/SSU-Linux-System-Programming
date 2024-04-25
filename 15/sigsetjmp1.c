#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void handler(int signo);

jmp_buf jump_buffer;

int main()
{
	signal(SIGINT, handler);

	while(1) {
			if(setjmp(jump_buffer) == 0) {// set jump here
					printf("Hit Ctrl-c at anytime ... \n");
					pause();
			}
	}
	exit(0);
}

void handler(int signo){
		char character;

		signal(signo, SIG_IGN); // set SIG_IGN
		printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
		character = getchar();

		if(character == 'y' || character == 'Y')
				exit(0);
		else{
				signal(SIGINT, handler);
				longjmp(jump_buffer, 1); // move to set jump
		}
}
