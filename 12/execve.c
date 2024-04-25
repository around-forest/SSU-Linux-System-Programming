#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
		char *argv[] = {"ssu_execl_test_1", "param1", "param2", (char *)0};
		char *env[] = {
				"NAME = value",
				"nextname=nextvalue",
				"HOME=/home/noahactuallya",
				(char *)0};

		printf("this is the original program\n");
		execve("./ssu_execl_test_1", argv, env);
		printf("%s\n", "this line should never get printed\n");
		exit(0);
}

