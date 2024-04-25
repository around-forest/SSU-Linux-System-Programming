#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *ssu_thread(void *arg);

int main()
{
		pthread_t tid;
		pid_t pid;

		if(pthread_create(&tid, NULL, ssu_thread, NULL) != 0) { // create new thread -> ssu_thread
				fprintf(stderr, "create error\n");
				exit(1);
		}

		pid = getpid(); // pid
		tid = pthread_self(); // thread pid

		printf("Main thread : pid %u tid %u \n", (unsigned int)pid, (unsigned int) tid);
		sleep(1); // same pid, different tid
		exit(0);
}

void *ssu_thread(void *arg) {
		pthread_t tid;
		pid_t pid;

		pid = getpid();
		tid = pthread_self();
		printf("New thread : pid %d tid %u \n", (int) pid, (unsigned int) tid);
		return NULL;
}
