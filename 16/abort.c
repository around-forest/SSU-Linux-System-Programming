#include <stdio.h>
#include <stdlib.h>

int main()
{
		printf("abort terminate this program\n"); // before abort
		abort(); // abort called, program terminated
		printf("this line is never reached\n"); // not reached due to abort
		exit(0);
}

