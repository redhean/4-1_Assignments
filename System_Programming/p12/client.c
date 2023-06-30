#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 80

int main (int argc, char *argv[]) {
    ssize_t num_bytes;
    int connected_fd;
    struct hostent *host_entry;
    struct sockaddr_in socket_address;
    char buffer[MAXLINE];

    if (argc < 3) {
        printf("Received %d arguments. Please enter host address and port number!\n", argc - 1);
        exit(1);
    }

    char* host = argv[1];
    uint16_t port = (uint16_t)strtol(argv[2], NULL, 10);

    if ((connected_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("socket() failed.\n"); 
        exit(2);
    }

    if ((host_entry = gethostbyname(host)) == NULL) {
        printf("invalid hostname %s\n", host); 
        exit(3);
    }

    memset((char *)&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    memcpy((char *)&socket_address.sin_addr.s_addr, (char *)host_entry->h_addr, (size_t) host_entry->h_length);
    socket_address.sin_port = htons(port);

    if (connect(connected_fd,(struct sockaddr *)&socket_address,sizeof(socket_address)) < 0) {
        printf("connect() failed.\n");
        exit(4);
    }

    printf("Insert your name : ");
    fflush(stdout);
    num_bytes = read(STDIN_FILENO, buffer, MAXLINE);
    buffer[num_bytes - 1] = '\0';
    write(connected_fd, buffer, (size_t) num_bytes);

    fd_set read_set;
    int fd_max = (connected_fd > STDIN_FILENO) ? connected_fd : STDIN_FILENO;

    while (1) {
        FD_ZERO(&read_set);
        FD_SET(STDIN_FILENO, &read_set);
        FD_SET(connected_fd, &read_set);

        if (select(fd_max + 1, &read_set, NULL, NULL, NULL) < 0) {
            printf("select() failed.\n");
            exit(5);
        }

        // 키보드 입력 처리
        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            num_bytes = read(STDIN_FILENO, buffer, MAXLINE);
            if (num_bytes <= 0) {
                break;
            }

            write(connected_fd, buffer, (size_t) num_bytes);

            if (strncmp(buffer, "quit", 4) == 0) {
                break;
            }
        }

        // 소켓 입력 처리
        if (FD_ISSET(connected_fd, &read_set)) {
            num_bytes = read(connected_fd, buffer, MAXLINE);
            if (num_bytes <= 0) {
                break;
            }
            write(STDOUT_FILENO, buffer, (size_t) num_bytes);
        }
    }
    close(connected_fd);

    return 0;
}
