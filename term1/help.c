#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("Usage:\n   > add <FILENAME> [OPTION]\n     -d : add directory recursive\n   > remove <FILENAME> [OPTION]\n     -a : remove all file(recursive)\n     -c : clear backup directory\n   > recover <FILENAME> [OPTION]\n     -d : recover directory recursive\n     -n <NEWNAME> : recover file with new name\n   > ls\n   > vi\n   > vim\n   > help\n   > exit\n\n");

	exit(0);
}


