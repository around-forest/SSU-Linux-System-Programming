#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 5

void *printhello(void *arg);

int main()
{
		pthread_t tid[THREAD_NUM];
		int i;

		for (i = 0; i < THREAD_NUM; i++) {
				printf("In main: creating thread %d\n", i);

				if(pthread_create(&tid[i], NULL, printhello, (void *)&i) != 0) {
						fprintf(stderr, "pthread_create error\n");
						exit(1);
				} // create new thread printhello func
		}
		pthread_exit(NULL); // exit thread
		exit(0);

}

void *printhello(void *arg) {
		int thread_index;

		thread_index = *((int *)arg);
		printf("Hello World! It;s me, thread #%d!\n", thread_index);
		pthread_exit(NULL); // exit thread
		return NULL;
}
