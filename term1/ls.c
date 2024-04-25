#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	char *current_dir = (char *)malloc(sizeof(char) * 1024);
	DIR *dir = NULL;
	struct dirent *entry = NULL;

	getcwd(current_dir,1024);

	if((dir = opendir(current_dir)) == NULL){
		printf("Current directory error\n");
		exit(1);
	}

	while((entry = readdir(dir)) != NULL)
	{
		printf("%s ", entry -> d_name);
	}

	printf("\n");

	closedir(dir);

	return 0;
}
