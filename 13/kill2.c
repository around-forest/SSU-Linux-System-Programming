#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
		while(1) { // SIGKILL 받을 떄 까지 반복
				printf("\n[NOAHACTUALLYA]");
				sleep(5);
		}

		exit(0);
}
