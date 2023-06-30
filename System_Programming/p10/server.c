#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAXLINE 200000

int main (int argc, char* argv[]) {
	int listening_fd, connecting_fd, connecting_address_length;
	//struct hostent* host_entry;
	struct sockaddr_in socket_address, connecting_address;
	char buffer[MAXLINE], buffer_data[MAXLINE];
	int num_bytes;

	if (argc < 2) {
		printf("Received %d arguments. Please enter port number!\n", argc - 1);
		exit(1);
	}

	uint32_t port = (uint32_t)strtol(argv[1], NULL, 10); // contains port number 

	/* Create listen socket */
	if((listening_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("socket() failed.\n");
		exit(2);
	}

	memset((char*) &socket_address, 0, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_address.sin_port = htons(port);

	/* Bind sockaddr (IP, etc.) to listen socket */
	if(bind(listening_fd, (struct sockaddr *) &socket_address, sizeof(socket_address)) < 0){
		printf("bind() failed.\n");
		exit(3);
	}

	/* Listen to listen socket */
	if(listen(listening_fd, 5) < 0){
		printf("listen() failed.\n");
		exit(4);
	}

	/* Accept connection request from clients */
	while (1) {
		connecting_address_length = sizeof(connecting_address);

		if((connecting_fd = accept(listening_fd, (struct sockaddr *)&connecting_address, (socklen_t *)&connecting_address_length)) < 0){
			printf("accept() failed.\n");
			continue;
		}

		/* Receive filename & data */
		while(1){
			num_bytes = read(connecting_fd, buffer, MAXLINE);
			if(strncmp(buffer, "quit", 4) == 0){
				write(connecting_fd, buffer, num_bytes);
				break;
			}
			
			char file_name[MAXLINE];
			buffer[num_bytes - 1] = '\0';
			sprintf(file_name, "%s_copy.txt", buffer);

			int fd = open(file_name, O_WRONLY | O_CREAT, 0644);
			if (fd == -1) {
        		perror("Failed to open the file");
        		return 1;
    		}	

			num_bytes = read(connecting_fd, buffer_data, MAXLINE);
			printf("got %d bytes from client.\n", num_bytes);
			write(fd, buffer_data, num_bytes);

			close(fd);
		}

		if (strncmp(buffer, "quit", 4) == 0) {
        	break; 
    	}
	}
	close(connecting_fd);
	return 0;
}


