#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void timestamp(char *str);

int main()
{
		unsigned int ret;

		timestamp("before sleep()");
		ret = sleep(10); // return of sleep(10)
		timestamp("after sleep()"); // 10 sec after sleep()
		printf("sleep() returned %d\n", ret); // 0 when sleep is returned correctly
		exit(0);
}

void timestamp(char *str){
		time_t time_val;

		time(&time_val);
		printf("%s the time is %s\n", str, ctime(&time_val)); // print current time
}
