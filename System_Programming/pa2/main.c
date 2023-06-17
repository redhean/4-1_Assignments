#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARG 300
#define MAX_PATH_LEN 300

void make_tokens(char* cmd, char* arg[], char* arg2[], char* target){
	char* left, *right, *ptr;
	int num = 0;

	left = strtok(cmd, target);
	strcat(left, "\0");

	right = strtok(NULL, target);
	strcat(right, "\0");

	ptr = strtok(left, " ");
	while(ptr != NULL){
		arg[num++] = ptr;
		ptr = strtok(NULL, " ");
	}
	arg[num] = NULL;

	num = 0;
	ptr = strtok(right, " ");
	while(ptr != NULL){
		arg2[num++] = ptr;
		ptr = strtok(NULL, " ");
	}
	arg2[num] = NULL;

	return;
}

void sigint_handler(int signum){
	
}

void sigtstp_handler(int signum){

} 

char* inst[15] = {"ls", "man", "grep", "sort" ,"awk", "bc", "head", "tail", "cat", "cp", "mv" ,"rm", "pwd", "cd" ,"exit"};

int main(){
	size_t size;
	char* cmd, *ptr;
	char* arg[MAX_ARG], *arg2[MAX_ARG];
	char path[MAX_PATH_LEN], path2[MAX_PATH_LEN];
	int child_status;
	int fd[2];
	int fdr;

	const char* error_message_1 = "mini: command not found\n";
	const char* error_message_2 = "mini: No such file or directory\n";

	while(1){
		
		// signal
		signal(SIGINT, sigint_handler);
		signal(SIGTSTP, sigtstp_handler);

		int num = 0;
		cmd = NULL;
		printf("$ ");

		getline(&cmd, &size, stdin);
		cmd[strlen(cmd) - 1] = '\0';	


		// Terminate program
		if(strstr(cmd, "exit") != NULL){
			write(2, "exit\n", strlen("exit\n"));
			char* temp;
			int num;
			
			temp = strtok(cmd, " ");
			temp = strtok(NULL, " ");

			if(temp != NULL)
				num = atoi(temp);
			
			return num;
		}

		// ./
		if(strstr(cmd, "./") != NULL){

			ptr = strtok(cmd, "/");
			ptr = strtok(NULL, " ");

                        // if NULL(no command)
                        if(ptr == NULL)
                                continue;

                        while(ptr != NULL){
                                arg[num++] = ptr;
                                ptr = strtok(NULL, " ");
                        }
                        arg[num] = NULL;

			sprintf(path, "./%s", arg[0]);
			
			if(fork() == 0){
				if(execv(path, arg) == -1){
					exit(1);
				}
				exit(1);
			}
			else{
				wait(&child_status);
			}

		}
		// Pipe and (>, >>) and (<)
		else if((strchr(cmd, '|') != NULL && strchr(cmd, '>') != NULL && strchr(cmd, '<') != NULL) || (strchr(cmd, '|') != NULL && strstr(cmd, ">>") != NULL && strchr(cmd, '<') != NULL)){
			
			int num_pipes = 0;
			int num_args = 0;
			int prev_read = 0;
			char* tokens[100][100];
			memset(tokens, 0, sizeof(tokens));
			char* redirect_input;
			char* redirect_output;
			char* redirect_token;

			char* token = strtok(cmd, "|");

			while(token != NULL){
				if(token[0] == ' ')
					token++;
				arg[num_pipes++] = token;
				token = strtok(NULL, "|");
			}

			for(int i = 0; i < num_pipes; i++){
				if(strstr(arg[i] , ">>") != NULL){
                                        arg[i] = strtok(arg[i] , ">>");
                                        redirect_token = ">>";
                                        strtok(NULL, " ");
                                        redirect_output = strtok(NULL, " ");
                                }
                                else if(strchr(arg[i], '>') != NULL){
                                        arg[i] = strtok(arg[i] , ">");
                                        redirect_token = ">";
                                        redirect_output = strtok(NULL, " ");
                                }
				else if(strchr(arg[i] , '<') != NULL){
                                        arg[i] = strtok(arg[i] , "<");
                                        redirect_input = strtok(NULL, " ");
                                }
			}

			for(int i = 0; i < num_pipes; i++){
                                char* temp = strtok(arg[i], " ");
                                while(temp != NULL){
                                        tokens[i][num_args++] = temp;
                                        temp = strtok(NULL, " ");
                                }
                                num_args = 0;
                        }
			
			// check

                        int chk1 = 0;
                        for(int i = 0; i < num_pipes; i++){
                                int chk2 = 0;
                                for(int j = 0; j < 15; j++){
                                        if(strcmp(tokens[i][0], inst[j]) == 0)
                                                chk2 = 1;
                                }
                                if(chk2 == 0){
                                        chk1 = 1;
                                        break;
                                }
                        }

                        if(chk1 == 1){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }
			
			for(int i = 0; i <= num_pipes; i++){
				if(pipe(fd) == -1){
					exit(1);
				}	

				pid_t pid = fork();
				if(pid == -1){
					exit(1);
				}
				else if(pid == 0){
					if(i != num_pipes){

						if(i == 0){
							int file_fd = open(redirect_input, O_RDONLY);
							if(file_fd == -1){
								write(2, error_message_2, strlen(error_message_2));
								exit(1);
							}
							dup2(file_fd, 0);
							close(file_fd);
							dup2(fd[1], 1);
							close(fd[0]);
							close(fd[1]);
						}
						else{
							dup2(prev_read, 0);
							close(prev_read);
							dup2(fd[1], 1);
							close(fd[0]);
							close(fd[1]);
						}
					}
					else if(i == num_pipes){
						close(fd[0]);
						dup2(prev_read, 0);
						close(prev_read);

						int file_fd2;
						if(strcmp(redirect_token, ">") == 0){
                                                        file_fd2 = open(redirect_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                                        if(file_fd2 == -1){
                                                                write(2, error_message_2, strlen(error_message_2));
                                                                exit(1);
                                                        }
                                                        dup2(file_fd2, 1);
                                                        close(file_fd2);
                                                }
                                                else if(strcmp(redirect_token, ">>") == 0){
                                                        file_fd2 = open(redirect_output, O_WRONLY | O_CREAT | O_APPEND, 0644);
                                                        if(file_fd2 == -1){
                                                                write(2, error_message_2, strlen(error_message_2));
                                                                exit(1);
                                                        }
                                                        dup2(file_fd2, 1);
                                                        close(file_fd2);
                                                }
					}

					if(i != num_pipes){
                                                if(strcmp(tokens[i][0], "head") == 0 || strcmp(tokens[i][0] , "tail") == 0 || strcmp(tokens[i][0], "bc") == 0
                                                || strcmp(tokens[i][0], "awk") == 0 || strcmp(tokens[i][0], "sort") == 0 || strcmp(tokens[i][0] ,"man") == 0){
                                                        sprintf(path, "/usr/bin/%s", tokens[i][0]);
                                                }
                                                else{
                                                        sprintf(path, "/bin/%s", tokens[i][0]);
                                                }

                                                if(execvp(path, tokens[i]) == -1){
                                                        exit(1);
                                                }
                                        }
                                        else{
                                                if(strcmp(tokens[i - 1][0], "head") == 0 || strcmp(tokens[i - 1][0] , "tail") == 0 || strcmp(tokens[i - 1][0], "bc") == 0
                                                || strcmp(tokens[i - 1][0], "awk") == 0 || strcmp(tokens[i - 1][0], "sort") == 0 || strcmp(tokens[i - 1][0] ,"man") == 0){
                                                        sprintf(path, "/usr/bin/%s", tokens[i - 1][0]);
                                                }
                                                else{
                                                        sprintf(path, "/bin/%s", tokens[i - 1][0]);
                                                }
                                                if(execvp(path, tokens[i - 1]) == -1){
                                                        exit(1);
                                                }

                                        }
				}
				else{
					close(fd[1]);
					prev_read = fd[0];
				}
			}
			while(wait(&child_status) > 0);
			
		}
		// Pipe and (>, >>)
		else if((strchr(cmd, '|') != NULL && strchr(cmd, '>') != NULL) || (strchr(cmd, '|') != NULL && strstr(cmd, ">>") != NULL)){
			
			int num_pipes = 0;
			int num_args = 0;
			int prev_read = 0;
			char* tokens[100][100];
			memset(tokens, 0, sizeof(tokens));
			char* redirect_file;
			char* redirect_token;
			
			char* token = strtok(cmd, "|");

			while(token != NULL){
                        	if(token[0] == ' ')
                                        token++;
                                arg[num_pipes++] = token;
                                token = strtok(NULL, "|");
                        }

			for(int i = 0; i < num_pipes; i++){
				if(strstr(arg[i] , ">>") != NULL){
					arg[i] = strtok(arg[i] , ">>");
					redirect_token = ">>";
					strtok(NULL, " ");
					redirect_file = strtok(NULL, " ");
				}
				else if(strchr(arg[i], '>') != NULL){
					arg[i] = strtok(arg[i] , ">");
					redirect_token = ">";
					redirect_file = strtok(NULL, " ");
				}
			}

                        for(int i = 0; i < num_pipes; i++){
                                char* temp = strtok(arg[i], " ");
                                while(temp != NULL){
                                        tokens[i][num_args++] = temp;
                                        temp = strtok(NULL, " ");
                                }
                                num_args = 0;
                        }
		
			
			// check
			
			int chk1 = 0;
			for(int i = 0; i < num_pipes; i++){
				int chk2 = 0;
				for(int j = 0; j < 15; j++){
					if(strcmp(tokens[i][0], inst[j]) == 0)
						chk2 = 1;
				}
				if(chk2 == 0){
					chk1 = 1;
					break;
				}
			}

			if(chk1 == 1){
				write(2, error_message_1, strlen(error_message_1));
				continue;
			}


			for(int i = 0; i <= num_pipes; i++){
				if(pipe(fd) == -1)
					exit(1);

				pid_t pid = fork();
				if(pid == -1){
					exit(1);
				}
				else if(pid == 0){
					if(i != num_pipes){
						dup2(prev_read, 0);
						close(prev_read);
						dup2(fd[1], 1);
						close(fd[0]);
						close(fd[1]);
                                    	
					}
					else if(i == num_pipes){
						close(fd[0]);
                                                dup2(prev_read, 0);
                                                close(prev_read);

                                                int file_fd;
                                                 if(strcmp(redirect_token, ">") == 0){
                                                        file_fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                                        if(file_fd == -1){
                                                                write(2, error_message_2, strlen(error_message_2));
                                                                exit(1);
                                                        }
                                                        dup2(file_fd, 1);
                                                        close(file_fd);
                                                }
                                                else if(strcmp(redirect_token, ">>") == 0){
                                                        file_fd = open(redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                                                        if(file_fd == -1){
                                                                write(2, error_message_2, strlen(error_message_2));
                                                                exit(1);
                                                        }
                                                        dup2(file_fd, 1);
                                                        close(file_fd);
                                                }	
					}
					
					if(i != num_pipes){
						if(strcmp(tokens[i][0], "head") == 0 || strcmp(tokens[i][0] , "tail") == 0 || strcmp(tokens[i][0], "bc") == 0
                                        	|| strcmp(tokens[i][0], "awk") == 0 || strcmp(tokens[i][0], "sort") == 0 || strcmp(tokens[i][0] ,"man") == 0){
                                                	sprintf(path, "/usr/bin/%s", tokens[i][0]);
                                        	}
                                        	else{
                                                	sprintf(path, "/bin/%s", tokens[i][0]);
                                        	}
				
						if(execvp(path, tokens[i]) == -1){
							exit(1);
						}
					}
					else{
						if(strcmp(tokens[i - 1][0], "head") == 0 || strcmp(tokens[i - 1][0] , "tail") == 0 || strcmp(tokens[i - 1][0], "bc") == 0
                                        	|| strcmp(tokens[i - 1][0], "awk") == 0 || strcmp(tokens[i - 1][0], "sort") == 0 || strcmp(tokens[i - 1][0] ,"man") == 0){
                                                	sprintf(path, "/usr/bin/%s", tokens[i - 1][0]);
                                        	}
                                        	else{
                                                	sprintf(path, "/bin/%s", tokens[i - 1][0]);
                                       		}

                                        	if(execvp(path, tokens[i - 1]) == -1){
                                                	exit(1);
                                        	}

					}
				}
				else{	
					close(fd[1]);
					prev_read = fd[0];
				}
			}
			while(wait(&child_status) > 0);
		}
		// Pipe and <
		else if(strchr(cmd, '|') != NULL && strchr(cmd, '<') != NULL){
			int num_pipes = 0;
                        int num_args = 0;
                        int prev_read = 0;
                        char* tokens[100][100];
                        memset(tokens, 0, sizeof(tokens));
                        char* redirect_file;

                        char* token = strtok(cmd, "|");

                        while(token != NULL){
                                if(token[0] == ' ')
                                        token++;
                                arg[num_pipes++] = token;
                                token = strtok(NULL, "|");
                        }

                        for(int i = 0; i < num_pipes; i++){
                                if(strchr(arg[i] , '<') != NULL){
                                        arg[i] = strtok(arg[i] , "<");
                                        redirect_file = strtok(NULL, " ");
                                }
                        }

                        for(int i = 0; i < num_pipes; i++){
                                char* temp = strtok(arg[i], " ");
                                while(temp != NULL){
                                        tokens[i][num_args++] = temp;
                                        temp = strtok(NULL, " ");
                                }
                                num_args = 0;
                        }

			int chk1 = 0;
                        for(int i = 0; i < num_pipes; i++){
                                int chk2 = 0;
                                for(int j = 0; j < 15; j++){
                                        if(strcmp(tokens[i][0], inst[j]) == 0)
                                                chk2 = 1;
                                }
                                if(chk2 == 0){
                                        chk1 = 1;
                                        break;
                                }
                        }

                        if(chk1 == 1){
                                write(2, error_message_1, strlen(error_message_1));
				continue;
                        }

			for(int i = 0; i < num_pipes; i++){
				
				if(pipe(fd) == -1){
					exit(1);
				}

				pid_t pid = fork();
				if(pid == -1){
					exit(1);
				}
				else if(pid == 0){
					if(i == num_pipes - 1){
                                        	dup2(prev_read, 0);
						close(prev_read);
                                        }
					else{
						if(i == 0){
							int file_fd  = open(redirect_file, O_RDONLY);
							if(file_fd == -1){
								write(2, error_message_2, strlen(error_message_2));
								exit(1);
							}
							dup2(file_fd, 0);
							close(file_fd);
							dup2(fd[1], 1);
							close(fd[0]);
							close(fd[1]);
						}
						
						else{
							dup2(prev_read, 0);
							close(prev_read);
							dup2(fd[1], 1);
							close(fd[0]);
							close(fd[1]);
						}
					                       
                                        }

                                        if(strcmp(tokens[i][0], "head") == 0 || strcmp(tokens[i][0] , "tail") == 0 || strcmp(tokens[i][0], "bc") == 0
                                        || strcmp(tokens[i][0], "awk") == 0 || strcmp(tokens[i][0], "sort") == 0 || strcmp(tokens[i][0] ,"man") == 0){

                                                sprintf(path, "/usr/bin/%s", tokens[i][0]);
                                        }
                                        else{
                                                sprintf(path, "/bin/%s", tokens[i][0]);
                                        }

                                        if(execvp(path, tokens[i]) == -1){
						exit(1);
					}
                                }
				else{
					close(fd[1]);
					prev_read = fd[0];

				}
				
			}
			while(wait(&child_status) > 0);
		}	
		// Pipe
		else if(strchr(cmd, '|') != NULL){
			
			int prev_read = 0;
			int num_pipes = 0;
			char* token = strtok(cmd, "|");

			while(token != NULL){
				if(token[0] == ' ')
					token++;
				arg[num_pipes++] = token;
				token = strtok(NULL, "|");
			}
			
			int num_args = 0;
			char* tokens[100][100];
			memset(tokens, 0, sizeof(tokens));

			for(int i = 0; i < num_pipes; i++){
				char* temp = strtok(arg[i], " ");
				while(temp != NULL){
					tokens[i][num_args++] = temp;
					temp = strtok(NULL, " "); 
				}
				num_args = 0;
			}	
			
			// check
			int chk1 = 0;
                        for(int i = 0; i < num_pipes; i++){
                                int chk2 = 0;
                                for(int j = 0; j < 15; j++){
                                        if(strcmp(tokens[i][0], inst[j]) == 0)
                                                chk2 = 1;
                                }
                                if(chk2 == 0){
                                        chk1 = 1;
                                        break;
                                }
                        }

                        if(chk1 == 1){
                                write(2, error_message_1, strlen(error_message_1));
				continue;
                        }


			for(int i = 0; i < num_pipes; i++){
				if(pipe(fd) == -1)
					exit(1);

				pid_t pid = fork();
				
				if(pid == -1){
					exit(1);
				}
				else if(pid == 0){
					close(fd[0]);

					if(i == num_pipes - 1){
						dup2(prev_read, 0);
						close(prev_read);
					}
					else{
						dup2(prev_read, 0);
						dup2(fd[1], 1);
						close(prev_read);
						close(fd[1]);
					}

					if(strcmp(tokens[i][0], "head") == 0 || strcmp(tokens[i][0] , "tail") == 0 || strcmp(tokens[i][0], "bc") == 0
                                        || strcmp(tokens[i][0], "awk") == 0 || strcmp(tokens[i][0], "sort") == 0 || strcmp(tokens[i][0] ,"man") == 0){

                                                sprintf(path, "/usr/bin/%s", tokens[i][0]);
                                        }
                                        else{
                                                sprintf(path, "/bin/%s", tokens[i][0]);
                                        }

					if(execvp(path, tokens[i]) == -1){
						exit(1);
					}

				}
				else{
					close(fd[1]);
					prev_read = fd[0];
				}
						
			}
			
			while(wait(&child_status) > 0);
                           
		}
		// < & >> Redirection
		else if(strchr(cmd, '<') != NULL && strstr(cmd, ">>") != NULL){
			make_tokens(cmd, arg, arg2, "<");
			
			// check
			int chk1 = 0;
			
                        for(int j = 0; j < 15; j++){
                        	if(strcmp(arg[0], inst[j]) == 0)
                                	chk1 = 1;
                        }
                        

                        if(chk1 == 0){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }

			sprintf(path, "/bin/%s", arg[0]);

			if(fork() == 0){
				fd[0] = open(arg2[0] , O_RDONLY);
				if(fd[0] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[0], 0);
				close(fd[0]);

				fd[1] = open(arg2[2], O_CREAT | O_WRONLY | O_APPEND, 0644);
				if(fd[1] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[1], 1);
				close(fd[1]);

				if(execvp(path, arg) == -1)
					exit(1);
				exit(1);
			}
			else{
				wait(&child_status);
			}
		}
		// < & > Redirection
		else if(strchr(cmd, '<') != NULL && strchr(cmd, '>') != NULL){
			make_tokens(cmd, arg, arg2, "<");

			// check
                        int chk1 = 0;

                        for(int j = 0; j < 15; j++){
                                if(strcmp(arg[0], inst[j]) == 0)
                                        chk1 = 1;
                        }


                        if(chk1 == 0){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }
			
			sprintf(path, "/bin/%s", arg[0]);

			if(fork() == 0){
				fd[0] = open(arg2[0], O_RDONLY);
				if(fd[0] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[0], 0);
				close(fd[0]);

				fd[1] = open(arg2[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
				if(fd[1] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[1], 1);
				close(fd[1]);
				
				if(execvp(path, arg) == -1)
					exit(1);
				exit(1);
			}
			else{
				wait(&child_status);
			}
		}

		// >> Redirection
		else if(strstr(cmd, ">>") != NULL){
			make_tokens(cmd, arg, arg2, ">>");
			
			// check
                        int chk1 = 0;

                        for(int j = 0; j < 15; j++){
                                if(strcmp(arg[0], inst[j]) == 0)
                                        chk1 = 1;
                        }


                        if(chk1 == 0){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }

			sprintf(path, "/bin/%s", arg[0]);

			if(fork() == 0){
				fd[0] = open(arg2[0], O_CREAT | O_WRONLY | O_APPEND, 0644);
				
				if(fd[0] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[0], 1);
				if(execvp(path, arg) == -1)
					exit(1);
				close(fd[0]);
				exit(1);
			}
			else{
				wait(&child_status);
			}	
		}

		// > Redirection
		else if(strchr(cmd, '>') != NULL){
			make_tokens(cmd, arg, arg2, ">");
				
			// check
                        int chk1 = 0;

                        for(int j = 0; j < 15; j++){
                                if(strcmp(arg[0], inst[j]) == 0)
                                        chk1 = 1;
                        }


                        if(chk1 == 0){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }

			sprintf(path, "/bin/%s", arg[0]);

			if(fork() == 0){
				fd[0] = open(arg2[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
				
				if(fd[0] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[0], 1);
				if(execvp(path, arg) == -1)
					exit(1);
				close(fd[0]);
				exit(1);
			}
			else{
				wait(&child_status);
			}
		}
		// < Redirection
		else if(strchr(cmd, '<') != NULL){
			make_tokens(cmd, arg, arg2, "<");

			// check
                        int chk1 = 0;

                        for(int j = 0; j < 15; j++){
                                if(strcmp(arg[0], inst[j]) == 0)
                                        chk1 = 1;
                        }


                        if(chk1 == 0){
                                write(2, error_message_1, strlen(error_message_1));
                                continue;
                        }

			sprintf(path, "/bin/%s", arg[0]);

			if(fork() == 0){
				fd[0] = open(arg2[0], O_RDONLY);
				if(fd[0] == -1){
					write(2, error_message_2, strlen(error_message_2));
					exit(1);
				}
				dup2(fd[0], 0);
				close(fd[0]);
				if(execvp(path, arg) == -1)
					exit(1);
				exit(1);
			}
			else{
				wait(&child_status);
			}
		}
		// Only single command
		else{
			char error[1000];
			ptr = strtok(cmd, " ");
			
			// if NULL(no command)
			if(ptr == NULL)
				continue;

			while(ptr != NULL){
				arg[num++] = ptr;
				ptr = strtok(NULL, " ");
			}
			arg[num] = NULL;

			if(strcmp(arg[0], "cd") == 0){
				if(chdir(arg[1]) == -1){
					sprintf(error, "cd: %s\n", strerror(errno));
					write(2, error, strlen(error));
				}
			}
			else if(strcmp(arg[0], "head") == 0 || strcmp(arg[0] , "tail") == 0 || strcmp(arg[0], "bc") == 0
				|| strcmp(arg[0], "awk") == 0 || strcmp(arg[0], "sort") == 0 || strcmp(arg[0] ,"man") == 0){

				sprintf(path, "/usr/bin/%s", arg[0]);

				if(fork() == 0){
					if((execv(path, arg) == -1 && arg[0] != NULL)){
						if(strcmp(arg[0], "head") == 0){
							sprintf(error, "head: %s\n", strerror(errno));
							write(2, error, strlen(error));
						}
						else if(strcmp(arg[0], "tail") == 0){
							sprintf(error, "tail: %s\n", strerror(errno));
							write(2, error, strlen(error));
						}
						exit(1);
					}
					exit(1);	
				}
				else{
					wait(&child_status);
				}

			}
			else{
				sprintf(path, "/bin/%s", arg[0]);

				if(fork() == 0){
					if((execv(path, arg) == -1) && arg[0] != NULL){
						if(strcmp(arg[0], "cat") == 0){
                                                        sprintf(error, "cat: %s\n", strerror(errno));
                                                        write(2, error, strlen(error));
                                                }
                                                else if(strcmp(arg[0], "cp") == 0){
                                                        sprintf(error, "cp: %s\n", strerror(errno));
                                                        write(2, error, strlen(error));
                                                }
						else if(strcmp(arg[0], "mv") == 0){
                                                        sprintf(error, "mv: %s\n", strerror(errno));
                                                        write(2, error, strlen(error));
                                                }
						else if(strcmp(arg[0], "rm") == 0){
                                                        sprintf(error, "rm: %s\n", strerror(errno));
                                                        write(2, error, strlen(error));
                                                }
						else
							write(2, error_message_1, strlen(error_message_1));
						exit(1);
					}
					exit(1);
				}
				else{
					wait(&child_status);
				}
			}
		}
		
	}
	return 0;
}
