#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
        char *args[] = {NULL};

        if(execv("/bin/vim", args) < 0)
                fprintf(stderr, "execv vim error\n");
}


