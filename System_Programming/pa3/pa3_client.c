#include <arpa/inet.h>	// htons, tcp, ipv4
#include <sys/types.h>	// ssize_t
#include <sys/socket.h>	// socket
#include <netdb.h>	// hostent
#include <stdio.h>	// printf
#include <unistd.h>	// write, read
#include <stdlib.h>	// atoi, exit, malloc
#include <string.h>	// memset, memcpy
#include <pthread.h>
#include <fcntl.h>

#define MAXLINE 256

void itoa(int num, char *buf){
        if(num == 0){
                buf[0] = '0';
                buf[1] = '\0';
                return;
        }

        int len = 0;
        int temp = num;
        while(temp != 0){
                len++;
                temp /= 10;
        }

        for(int i = 0; i < len; i++){
                buf[len - 1 -i] = '0' + num % 10;
                num /= 10;
        }
        buf[len] = '\0';
}

typedef struct _query {
    int user;
    int action;
    int seat;
} query;


int main(int argc, char *argv[]){
	
	//ssize_t num_bytes;
	int num_bytes, socket_fd;
	struct hostent *host_entry;
	struct sockaddr_in socket_address;
	char buffer[MAXLINE];
	int input_chk = 0;
	query queries[1000];
	int query_cnt = 0;

	if(argc ==  3){
		input_chk = 0;
	}else if (argc == 4) {
		input_chk = 1;
		FILE* file = fopen(argv[3], "r");
		if(file == NULL){
			exit(1);
		}

		while(fgets(buffer, sizeof(buffer), file) != NULL){
			char* temp_user = strtok(buffer, " ");
			char* temp_action = strtok(NULL, " ");
			char* temp_seat = strtok(NULL, " ");
			
			if(temp_user == NULL)
				temp_user = "-1";
			queries[query_cnt].user = atoi(temp_user);

			if(temp_action == NULL)
				temp_action = "-1";
			queries[query_cnt].action = atoi(temp_action);

			if(temp_seat == NULL)
				temp_seat = "-1";
			queries[query_cnt].seat = atoi(temp_seat);
			query_cnt++; 

		}

		fclose(file);


    	} else {
        	printf("Follow the input rule below\n");
        	printf("==================================================================\n");
        	printf("argv[1]: server address, argv[2]: port number\n");
        	printf("or\n");
        	printf("argv[1]: server address, argv[2]: port number, argv[3]: input file\n");
        	printf("==================================================================\n");
        	exit(1);
    	}

	char *host = argv[1]; // hostname
	uint32_t port = (uint32_t)strtol(argv[2], NULL, 10); // port number

	// create a socket for TCP connections
	// AF_INETL IPv4, SOCK_STREAM : stream socket, IPPROTO_TCP: TCP
	if((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("socket() failed.\n");
		exit(2);
	}

	// Gets the host entry given the hostname
	if((host_entry = gethostbyname(host)) == NULL){
		printf("invalid hostname %s\n", host);
		exit(3);
	}


	// initializes socket_address fields to zero
	memset((char *)&socket_address, 0, sizeof(socket_address));
	// sets address family to IPv4
	socket_address.sin_family = AF_INET;
	// copies ip address
	memcpy((char *)&socket_address.sin_addr.s_addr, (char *)host_entry->h_addr, host_entry->h_length);
	// sets port after converting it to network byte order
	socket_address.sin_port = htons(port);

	// connects the socket to the given IP address
	if(connect(socket_fd,(struct sockaddr *)&socket_address, sizeof(socket_address)) < 0){
		printf("connect() failed.\n");
		exit(4);
	}

	// reads user input
	pid_t pid = getpid();

	if(input_chk == 0){
		while((num_bytes = read(STDIN_FILENO, buffer, MAXLINE)) > 0){
			// send input to server
			
			if(num_bytes == 0 || strspn(buffer, " \n") == num_bytes){
				num_bytes = sprintf(buffer, "-1 -1 -1\n");
			}

			if (buffer[num_bytes - 1] == '\n') {
           			buffer[num_bytes - 1] = '\0';
            			num_bytes--;
        		}

			snprintf(buffer + num_bytes, MAXLINE - num_bytes, " %d\n", pid);
			write(socket_fd, buffer, strlen(buffer));
			
			// get reponse from server
			
			char *temp_user = strtok(buffer, " ");
                        char *temp_action = strtok(NULL, " ");
                        char *temp_seat = strtok(NULL, " ");
			char *temp_pid2 = strtok(NULL, " ");
			char *overflow_test = strtok(NULL, " ");
			int temp_pid = -1;
			if(temp_pid2 != NULL && overflow_test == NULL)
				temp_pid = atoi(temp_pid2);

                        if(temp_user == NULL)
                                temp_user = "-1";
                        if(temp_action == NULL)
                                temp_user = "-1";
                        if(temp_seat == NULL)
                                temp_seat = "-1";
			
			int result = 0;
			int result_arr[1000];

			if(atoi(temp_action) == 3)
				num_bytes = read(socket_fd, result_arr, sizeof(result_arr));
			else
				num_bytes = read(socket_fd, &result, sizeof(result));
		
			// prints response to server
			if(result == -1){
				if(atoi(temp_user) == -1 || atoi(temp_action) == -1 || atoi(temp_seat) == -1 || temp_pid == -1)
					printf("Invalid query\n");
				else if(atoi(temp_action) == 1)
				       	printf("Login failed\n");
				else if(atoi(temp_action) == 2)
					printf("Reservation failed\n");
				else if(atoi(temp_action) == 4)
					printf("Cancel failed\n");
				else if(atoi(temp_action) == 5)
					printf("Logout failed\n");
				else
					printf("Invalid query\n");
			}
			else if(result == 1){
				if(atoi(temp_action) == 1)
					printf("Login success\n");
				else if(atoi(temp_action) == 5)
					printf("Logout success\n");
			}
			else{
				if(atoi(temp_action) == 2)
					printf("Seat %d is reserved\n", result);

				else if(atoi(temp_action) == 3){
					char temp[1024];
					int length = sprintf(temp, "Reservation: ");
					int cnt = 0;
					for(int i = 0; i < num_bytes / 4; i++){
						if(result_arr[i] == 1)
							cnt++;
					}
					int cnt2 = 0;
					for(int i = 0; i < num_bytes/4; i++){
						if(result_arr[i] == 1 && cnt2 == cnt - 1){
							length += sprintf(temp + length, "%d", i);
						}

						else if(result_arr[i] == 1){
							length += sprintf(temp + length, "%d, ", i);
							cnt2++;
						}
					}
					
					if(temp_pid == -1)
						printf("Invalid query\n");
					else if(cnt == 0)
						printf("Reservation check failed\n");
					else
						printf("%s\n", temp);
				}
				else if(atoi(temp_action) == 4)
					printf("Seat %d is canceled\n", result);
				else{
					// 256
					close(socket_fd);
					return 0;
				}
			}
		}
	}
	else if(input_chk == 1){
		for(int i = 0; i < query_cnt; i++){
			int length = snprintf(NULL, 0, "%d %d %d\n", queries[i].user, queries[i].action, queries[i].seat);
    			snprintf(buffer, length + 1, "%d %d %d\n", queries[i].user, queries[i].action, queries[i].seat);
			if (buffer[length - 1] == '\n') {
                                buffer[length - 1] = '\0';
                                length--;
                        }
                        snprintf(buffer + length, MAXLINE - length, " %d\n", pid);	
			write(socket_fd, buffer, strlen(buffer));

			char* temp_user = strtok(buffer, " ");
                        char* temp_action = strtok(NULL, " ");
                        char* temp_seat = strtok(NULL, " ");
			char *temp_pid2 = strtok(NULL, " ");
			char* overflow_test = strtok(NULL, " ");
			int temp_pid = -1;
			if(temp_pid2 != NULL && overflow_test == NULL)
				temp_pid = atoi(temp_pid2);

                        if(temp_user == NULL)
                                temp_user = "-1";
                        if(temp_action == NULL)
                                temp_user = "-1";
                        if(temp_seat == NULL)
                                temp_seat = "-1";

                        int result = 0;
                        int result_arr[1000];

			if(atoi(temp_action) == 3)
                                num_bytes = read(socket_fd, result_arr, sizeof(result_arr));
                        else
                                num_bytes = read(socket_fd, &result, sizeof(result));

			if(result == -1){
                                if(atoi(temp_user) == -1 || atoi(temp_action) == -1 || atoi(temp_seat) == -1 || temp_pid == -1)
                                        printf("Invalid query\n");
                                else if(atoi(temp_action) == 1)
                                        printf("Login failed\n");
                                else if(atoi(temp_action) == 2)
                                        printf("Reservation failed\n");
                                else if(atoi(temp_action) == 4)
                                        printf("Cancel failed\n");
                                else if(atoi(temp_action) == 5)
                                        printf("Logout failed\n");
				else
					printf("Invalid query\n");
                        }
                        else if(result == 1){
                                if(atoi(temp_action) == 1)
                                        printf("Login success\n");
                                else if(atoi(temp_action) == 5)
                                        printf("Logout success\n");
                        }
                        else{
                                if(atoi(temp_action) == 2)
                                        printf("Seat %d is reserved\n", result);

                                else if(atoi(temp_action) == 3){
                                        char temp[1024];
                                        int length = sprintf(temp, "Reservation: ");
                                        int cnt = 0;
                                        for(int i = 0; i < num_bytes / 4; i++){
                                                if(result_arr[i] == 1)
                                                        cnt++;
                                        }
                                        int cnt2 = 0;
                                        for(int i = 0; i < num_bytes/4; i++){
                                                if(result_arr[i] == 1 && cnt2 == cnt - 1){
                                                        length += sprintf(temp + length, "%d", i);
                                                }

                                                else if(result_arr[i] == 1){
                                                        length += sprintf(temp + length, "%d, ", i);
                                                        cnt2++;
                                                }
                                        }
					
					if(temp_pid == -1)
                                                printf("Invalid query\n");
					else if(cnt == 0)
                                                printf("Reservation check failed\n");
                                        else
                                                printf("%s\n", temp);
                                }
                                else if(atoi(temp_action) == 4)
                                        printf("Seat %d is canceled\n", result);
                        }
		}
		
		int length = snprintf(buffer, MAXLINE, "0 0 0 %d\n", pid);
		write(socket_fd, buffer, length);
		int result = 0;
		num_bytes = read(socket_fd, &result, sizeof(result));
		if(result == 256){
			close(socket_fd);
			return 0;
		}


	}
        close(socket_fd);
	return 0;
}
