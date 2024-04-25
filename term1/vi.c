#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *args[] = {NULL};

	if(execv("/bin/vi", args) < 0)
		fprintf(stderr, "execv vi error\n");
}

