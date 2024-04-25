#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread(void *arg);

int main()
{
		pthread_t tid;

		if(pthread_create(&tid, NULL, thread, NULL) != 0) {
				fprintf(stderr, "pthread_create error\n");
				exit(1);
		} // create new thread, thread func

		sleep(1);
		printf("쓰레드가 완료되기전 main 함수가 먼저 종료되면 실행중 쓰레드 소멸\n");
		printf("메인 종료\n");
		exit(0);
}

void *thread(void *arg) {
		printf("쓰레드 시작\n");
		sleep(5);
		printf("쓰레드 수행 완료\n");
		pthread_exit(NULL); // exit thread
		return NULL;
}
