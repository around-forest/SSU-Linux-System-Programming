#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signo);

void (*func)(int);

int main()
{
		func = signal(SIGINT, signal_handler); // SIGINT에 대한 시그널 핸들러 등록

		while(1) {
				printf("process running...\n");
				sleep(1);
		} // SIGINT 발생 시 출력 실행

		exit(0);
}

void signal_handler(int signo) { // 리턴값 func에 저장
		printf("SIGINT 시그널 발생.\n");
		printf("SIGINT를 SIG_DFL로 재설정 함.\n");
		signal(SIGINT, func);
}
