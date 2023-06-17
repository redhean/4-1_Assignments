#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXLINE 100

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
    int data;
} query;

typedef struct _userdata{
	int login_chk;
	int password;
	int pid;
} userdata;

query queries[1000];
int query_cnt = 0;
userdata users[1024];
int seat_chk[256];
pthread_mutex_t seat_mutex[256];


void* handle_client(void* arg){
	int connected_fd = *(int*)arg;
	ssize_t num_bytes;
	char buffer[MAXLINE];

	while((num_bytes = read(connected_fd, buffer, MAXLINE)) > 0){
		char *temp_user = strtok(buffer, " ");
		char *temp_action = strtok(NULL, " ");
		char *temp_data = strtok(NULL, " ");
		char *temp_pid2 = strtok(NULL, " ");
		int temp_pid = -1;
		if(temp_pid2 != NULL)
			temp_pid = atoi(temp_pid2);

		if(temp_user == NULL)
                	temp_user = "-1";
                queries[query_cnt].user = atoi(temp_user);

                if(temp_action == NULL)
                	temp_action = "-1";
                queries[query_cnt].action = atoi(temp_action);

                if(temp_data == NULL)
                	temp_data = "-1";
                queries[query_cnt].data = atoi(temp_data);
               
		if((queries[query_cnt].action == 2 || queries[query_cnt].action == 4) && temp_pid != -1){
			pthread_mutex_unlock(&seat_mutex[queries[query_cnt].data]);
			if(pthread_mutex_lock(&seat_mutex[queries[query_cnt].data]) != 0){
				int result = -1;
        			write(connected_fd, &result, sizeof(result));
				continue;
			}
		}

		if(queries[query_cnt].user == -1 || queries[query_cnt].action == -1 || queries[query_cnt].data == -1 || temp_pid == -1){
			int result = -1;
			write(connected_fd, &result, sizeof(result));	
                }
		else if( (queries[query_cnt].user < 0 || queries[query_cnt].user > 1023) || (queries[query_cnt].action < 0 || queries[query_cnt].action > 5) ){
			int result = -1;
                        write(connected_fd, &result, sizeof(result));
		}
		else if(queries[query_cnt].action == 1){
			// registration
			if(users[queries[query_cnt].user].password == -1 && users[queries[query_cnt].user].login_chk == 0){
				if(queries[query_cnt].data < 0){
					int result = -1;
					write(connected_fd, &result, sizeof(result));
				}
				else{
					int cnt = 0;
                    for(int i = 0; i < 1024; i++){
                        if(users[i].login_chk == 1 && users[i].pid == temp_pid){
                            cnt++;
                            break;
                   	 	}
                	}

                    if(cnt != 0){
                    	int result = -1;
                        write(connected_fd, &result, sizeof(result));
                    }
					else{
						users[queries[query_cnt].user].password = queries[query_cnt].data;
						users[queries[query_cnt].user].login_chk = 1;
						users[queries[query_cnt].user].pid = temp_pid;
						int result = 1;
						write(connected_fd, &result, sizeof(result));
					}
				}
			}
			// login
			else if(users[queries[query_cnt].user].login_chk == 0){
				if(users[queries[query_cnt].user].password == queries[query_cnt].data){
					int cnt = 0;
                    for(int i = 0; i < 1024; i++){
                        if(users[i].login_chk == 1 && users[i].pid == temp_pid){
                            cnt++;
                            break;
                   	 	}
                	}

                    if(cnt != 0){
                    	int result = -1;
                        write(connected_fd, &result, sizeof(result));
                    }
					else{
						users[queries[query_cnt].user].login_chk = 1;
						users[queries[query_cnt].user].pid = temp_pid;
						int result = 1;
						write(connected_fd, &result, sizeof(result));
					}
				}
				else{
					int result = -1;
					write(connected_fd, &result, sizeof(result));
				}
			}
			else{
				int result = -1;
				write(connected_fd, &result, sizeof(result));
			}
		}
		else if(queries[query_cnt].action == 2){
			// already login or no seat
			if(users[queries[query_cnt].user].login_chk == 0 || queries[query_cnt].data < 0 || queries[query_cnt].data > 255 
					|| seat_chk[queries[query_cnt].data] != -1){
				int result = -1;
				write(connected_fd, &result, sizeof(result));
			}
			// reserve seat
			else{
				seat_chk[queries[query_cnt].data] = queries[query_cnt].user;
				int result = queries[query_cnt].data;
				write(connected_fd, &result, sizeof(result));
			}
			pthread_mutex_unlock(&seat_mutex[queries[query_cnt].data]);
		}
		else if(queries[query_cnt].action == 3){
			if(users[queries[query_cnt].user].login_chk == 0){
				int result = -1;
				write(connected_fd, &result, sizeof(result));
			}
			
			int reserve_cnt = 0;
			int *reserve_arr = malloc(sizeof(int) * 256);
			for(int i = 0; i < 256; i++){
				if(seat_chk[i] == queries[query_cnt].user){
					reserve_arr[reserve_cnt] = i;
					reserve_cnt++;
				}
			}

			if(reserve_cnt == 0 && users[queries[query_cnt].user].login_chk == 1){
				int result = -1;
				write(connected_fd, &result, sizeof(result));
			}
			else if(reserve_cnt > 0 && users[queries[query_cnt].user].login_chk == 1){ 
				int return_seat[256] = {0,};
				for(int i = 0; i < reserve_cnt; i++){
					return_seat[reserve_arr[i]] = 1;
				}
				write(connected_fd, return_seat, sizeof(return_seat));
			}
			free(reserve_arr);
		}
		else if(queries[query_cnt].action == 4){
			if(queries[query_cnt].data >= 256 || queries[query_cnt].data < 0){
				int result = -1;
				write(connected_fd, &result, sizeof(result));
			} 
			else if(users[queries[query_cnt].user].login_chk == 0 || seat_chk[queries[query_cnt].data] == 0 || seat_chk[queries[query_cnt].data] != queries[query_cnt].user){
                                int result = -1;
                                write(connected_fd, &result, sizeof(result));
                        }
			else{
				seat_chk[queries[query_cnt].data] = -1;
				int result = queries[query_cnt].data;
				write(connected_fd, &result, sizeof(result));
			}
			pthread_mutex_unlock(&seat_mutex[queries[query_cnt].data]);
		}
		else if(queries[query_cnt].action == 5){
			if(users[queries[query_cnt].user].login_chk == 0 || users[queries[query_cnt].user].pid != temp_pid){
				int result = -1;
				write(connected_fd, &result, sizeof(result));	
			}	
			else{	
				users[queries[query_cnt].user].login_chk = 0;
				int result = 1;
				write(connected_fd, &result, sizeof(result));
			}
		}
		else{
			if(queries[query_cnt].user == 0 && queries[query_cnt].action == 0 && queries[query_cnt].data == 0){
                        	for(int i = 0; i < 1024; i++){
					if(users[i].pid == temp_pid)
                                		users[i].login_chk = 0;
                        	}
                        	int result = 256;
                        	write(connected_fd, &result, sizeof(result));
                	}
		}
		
		query_cnt++;
	}

	//printf("connection terminated.\n");
	close(connected_fd);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]){
	ssize_t num_bytes;
	size_t connected_address_length;
	int listening_fd, connected_fd;
	struct sockaddr_in socket_address, connected_address;
	char buffer[MAXLINE];

	for (int i = 0; i < 1024; i++) {
        	users[i].login_chk = 0;
        	users[i].password = -1;
    	}

	for(int i = 0; i < 256; i++){
		seat_chk[i] = -1;
	}

	for (int i = 0; i < 256; ++i) {
        	if (pthread_mutex_init(&seat_mutex[i], NULL) != 0) {
            	perror("Mutex initialization failed");
            	exit(EXIT_FAILURE);
        	}
    	}

	if(argc < 2){
		exit(1);
	}

	uint32_t port = (uint32_t)strtol(argv[1], NULL, 10);

	// creates a socket for TCP connections
	// AF_INETL: IPv4, SOCK_STREAM: stream socket, IPPROTO_TCP: TCP
	if((listening_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("socket() failed.\n");
		exit(2);
	}

	// initialize socket_address fields to zero
	memset((char *)&socket_address, 0, sizeof(socket_address));
	// sets address family to IPv4
	socket_address.sin_family = AF_INET;
	//copies ip address
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// sets port after converting it to network byte order
	socket_address.sin_port = htons(port);

	// assigns socket address to the listening fd
	if(bind(listening_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0){
		//printf("bind() failed.\n");
		//exit(3);
		return 0;
	}

	// listens to 1024 connections at most
	if(listen(listening_fd, 1024) < 0){
		//printf("listen() failed.\n");
		//exit(4);
		return 0;
	}

	while(1){
		connected_address_length = sizeof(connected_address);
		query query_receive;

		// accepts connection
		if((connected_fd = accept(listening_fd, (struct sockaddr *)&connected_address, (socklen_t *)&connected_address_length)) <0){
			printf("accept() failed.\n");
			continue;
		}

		// receives data from client
		pthread_t thread;
		if(pthread_create(&thread, NULL, handle_client, &connected_fd) != 0){
			printf("pthread_create() failed.\n");
			close(connected_fd);
			continue;
		}
		
		pthread_detach(thread);
		pthread_join(thread, NULL);

	}
	
	close(listening_fd);
	return 0;
}
