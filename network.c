#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

void get_server(struct sockaddr_in *server, char *addr,
                int port, int *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == *sock)
    {
        printf("Socket creation error\n");
        exit(0);
    }

    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    inet_aton(addr, &(server->sin_addr));
    server->sin_port = htons(port);
}

int send_msg(int sock, char *msg, int len)
{
    return write(sock, msg, len);
}

int read_msg(int sock, char *msg, int len)
{
    return read(sock, msg, len);
}
