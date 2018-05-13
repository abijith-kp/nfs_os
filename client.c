#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "network.h"

#define PROMPT "> "

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    int sock;
    
    if (argc != 3)
    {
        printf("Usage: %s server_ip server_port\n", argv[0]);
        exit(0);
    }

    get_server(&server, argv[1], atoi(argv[2]), &sock);

    if (-1 == connect(sock, (struct sockaddr *)&server, sizeof(server)))
    {
        printf("Connect error\n");
        exit(0);
    }

    char buffer[500];
    char input[100];

    while (1)
    {
        write(1, PROMPT, strlen(PROMPT));
        memset(input, 0, 100);
        read(0, input, 99);
        if (input[0] == '\n')
            continue;
        send_msg(sock, input, strlen(input)+1);
        memset(buffer, 0, 500);
        read_msg(sock, buffer, 99);
        input[strlen(input)-1] = 0;
        printf("[[%s]]\n", buffer);
        if (strcmp(input, "exit") == 0)
            break;
        else if (strncmp(input, "nano", 4) == 0)
        {
        }
    }

    close(sock);
    return 0;
}
