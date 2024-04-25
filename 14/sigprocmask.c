#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
		sigset_t sig_set;
		int count;

		sigemptyset(&sig_set); // empty all set
		sigaddset(&sig_set, SIGINT); // add SIGINT into set (change 0 to 1);
		sigprocmask(SIG_BLOCK, &sig_set, NULL); // block what is 1 in the set

		for (count = 3; 0 < count; count--){
				printf("count %d\n", count);
				sleep(1);
		}

		printf("Ctrl-C에 대한 블록을 해제\n"); // CTRL-C won't work till here
		sigprocmask(SIG_UNBLOCK, &sig_set, NULL); // unblock what is blocked
		printf("count 중 CTRL-C 입력하면 이 문장은 출력 되지 않음.\n");

		while(1);

		exit(0);
}
