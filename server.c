#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_LEN 100

void get_server(struct sockaddr_in *server, char *addr, int port)
{
    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    inet_aton(addr, &(server->sin_addr));
    server->sin_port = htons(port);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;
    char buffer[MAX_LEN];
    int length = sizeof(struct sockaddr_in);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock)
    {
        printf("Socket creation error\n");
        exit(0);
    }

    get_server(&server, argv[1], atoi(argv[2]));

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Bind error\n");
        exit(0);
    }

    while (1)
    {
        memset(buffer, 0, MAX_LEN);
        memset(&client, 0, sizeof(client));
        if (recvfrom(sock, buffer, MAX_LEN-1, 0, (struct sockaddr *)&client, (socklen_t *)&length) == -1)
            printf("Error in recvfrom()\n");
        else
            printf("SERVER: %s\n", buffer);
    }

    return 0;
}
