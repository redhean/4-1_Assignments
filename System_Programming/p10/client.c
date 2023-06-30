#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXLINE 200000

int main (int argc, char *argv[]) {
	struct hostent* host_entry; // contains information related to the host like IP address
	struct sockaddr_in socket_address;
	char buffer[MAXLINE], buffer_data[MAXLINE];
	int num_bytes, socket_fd, fd;

	if (argc < 3){
		printf("Received %d arguments. Please enter host address and port number!\n", argc - 1);
		exit(1);
	}

	char *host = argv[1]; // contains hostname (e.g. "www.skku.edu", "localhost", etc.) 
	uint32_t port = (uint32_t)strtol(argv[2], NULL, 10); // contains port number 

	/* Create socket */
	if((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("socket() failed.\n");
		exit(2);
	}

	/* Get host information */
	if((host_entry = gethostbyname(host)) == NULL){
		printf("invalid hostname %s\n", host);
		exit(3);
	}

	memset((char*)&socket_address, 0, sizeof(socket_address));	// initializes socket_address fields to zero
	socket_address.sin_family = AF_INET;	// sets address family to IPv4
	memcpy((char*)&socket_address.sin_addr.s_addr, (char*)host_entry-> h_addr, host_entry-> h_length);	// copies ip address
	socket_address.sin_port = htons(port);	// sets port after converting it to "network byte order"

	/* Connect to host */
	if(connect(socket_fd, (struct sockaddr *) & socket_address, sizeof(socket_address)) < 0){
		printf("connect() failed.\n");
		exit(4);
	}
	
	int chk = 0;
	while (1) {
		/* Send filename & data */
		num_bytes = read(STDIN_FILENO, buffer, MAXLINE);
		if(strncmp(buffer, "quit", 4) == 0)
			chk = 1;

		write(socket_fd, buffer, num_bytes);
		
		if(chk == 0){
			char file_name[MAXLINE];
			buffer[num_bytes - 1] = '\0';
			sprintf(file_name, "%s.txt", buffer);
		
			fd = open(file_name, O_RDONLY);
			if (fd == -1) {
        		perror("Failed to open the file");
        		return 1;
    		}	
						
			num_bytes = read(fd, buffer_data, MAXLINE);
			write(socket_fd, buffer_data, num_bytes);
			printf("Send %d bytes to server.\n", num_bytes);
			close(fd);
		}
		else
			break;
	}
	close(socket_fd);

	return 0;
}


