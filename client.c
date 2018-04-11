#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

void get_server(struct sockaddr_in *server, char *addr, int port)
{
    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    inet_aton(addr, &(server->sin_addr));
    server->sin_port = htons(port);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock)
    {
        printf("Socket creation error\n");
        exit(0);
    }

    get_server(&server, argv[1], atoi(argv[2]));

    if (sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&server, sizeof(server)) == -1)
        printf("Error in sending data\n");

    return 0;
}
