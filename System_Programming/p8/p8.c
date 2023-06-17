#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG 128
#define MAX_PATH_LEN 128

void make_tokens(char* cmd, char* arg[], char* arg2[], char* target) {
    char *left, *right, *ptr;
    int num = 0;

    left = strtok(cmd, target);
    strcat(left, "\0");
    right = strtok(NULL, target);
    strcat(right, "\0");

    ptr = strtok(left, " ");
    while (ptr != NULL) {
        arg[num++] = ptr;
        ptr = strtok(NULL, " ");
    }
    arg[num] = NULL;

    num = 0;
    ptr = strtok(right, " ");
    while (ptr != NULL) {
        arg2[num++] = ptr;
        ptr = strtok(NULL, " ");
    }
    arg2[num] = NULL;

    return;
}

int main () {
    size_t size;
    char *cmd, *ptr;
    char *arg[MAX_ARG], *arg2[MAX_ARG];
    char path[MAX_PATH_LEN], path2[MAX_PATH_LEN];
    int child_status;
    int fd[2];
    int fdr;
    
    while(1) {
        int num = 0;
        cmd = NULL;
		printf("> ");
        getline(&cmd, &size, stdin);
        cmd[strlen(cmd)-1] = '\0';

	// Terminate program
        if(strcmp(cmd, "quit") == 0) {
            break;
        }

	// Pipe
        if(strchr(cmd, '|') != NULL) {
            make_tokens(cmd, arg, arg2, "|");
            
            sprintf(path, "/bin/%s", arg[0]);
            sprintf(path2, "/bin/%s", arg2[0]);

            if(pipe(fd) == -1)
                exit(1);

            if(fork() == 0) {
                // Place your code in this if block!

		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
		if(execvp(path, arg) == -1) exit(1);
            }
	    else{
		if(fork() == 0){	
			close(fd[1]);
			dup2(fd[0], 0);
			close(fd[0]);
			if(execvp(path2, arg2) == -1) exit(1);
		}else{
			close(fd[0]);
			close(fd[1]);
			wait(&child_status);
			wait(&child_status);
		}
	    }
        } 
	// > Redirection
        else if(strchr(cmd, '>') != NULL) {
            make_tokens(cmd, arg, arg2, ">");
            sprintf(path, "/bin/%s", arg[0]);

            if(fork() == 0) {
                // Place your code in this if block!
		
		fd[0] = open(arg2[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if(fd[0] == -1){
			perror("open");
			exit(1);
		}
		dup2(fd[0], 1);
		execvp(path, arg);
		close(fd[0]);
		exit(1);

            }
            else wait(&child_status);

        }
	// < REDIRECTION
        else if(strchr(cmd, '<') != NULL) {
            make_tokens(cmd, arg, arg2, "<");
            sprintf(path, "/bin/%s", arg[0]);

            if(fork() == 0) {
                // Place your code in this if block!
		
		fd[0] = open(arg2[0], O_RDONLY);
		if(fd[0] == -1){
			perror("open");
			exit(1);
		}
		dup2(fd[0], 0);
		close(fd[0]);
		execvp(path, arg);	
		exit(1);

            }
            else
                wait(&child_status);

        }
	// ONLY SINGLE COMMAND
        else {
            ptr = strtok(cmd, " ");
            while (ptr != NULL) {
                arg[num++] = ptr;
                ptr = strtok(NULL, " ");
            }
            arg[num] = NULL;

            sprintf(path, "/bin/%s", arg[0]);
        
            if (fork() == 0) {
                execv(path, arg);
                exit(1);
            }
            else
                wait(&child_status);            
        }
    }
    return 0;
}



