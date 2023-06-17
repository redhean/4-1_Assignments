#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    char *cmd = NULL;
    size_t size = 0;
    char path[100];

    while (1) {
        printf("$ ");
        /* get input */
        if ((getline(&cmd, &size, stdin)) < 0) {
            perror("getline failed\n");
            exit(1);
        }

        /* if "quit", exit */
        if (strncmp(cmd, "quit", 4) == 0) {
            free(cmd);
            exit(1);
        }

        /* separate command name and options */
	cmd[strlen(cmd) - 1] = '\0';
	char** command = (char**)malloc(sizeof(char*) * 16);
	for(int i = 0; i < 10000; i++){
		command[i] = (char*)malloc(sizeof(char)*100);
	}

	int i = 0;
	char *ptr = strtok(cmd, " ");

	while(ptr != NULL){
		command[i++] = ptr;
		ptr = strtok(NULL, " ");

	}
	command[i] = NULL;
	sprintf(path, "/bin/%s", command[0]);

        /* execute command using fork()
	pid_t pid = fork();

	if(pid == 0) {
		execl(path, path, command[1], command[2], command[3], command[4], command[5], command[6],command[7], command[8], command[9], command[10],command[11], command[12], command[13],command[14],command[15],NULL);
		
	}
	wait(NULL);
	

    }
    return 0;
}
