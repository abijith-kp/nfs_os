#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"

#define CCAT(x) #x
#define CONCAT(x) CCAT(x)
//(x - 1)

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
    
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sock)
    {
        printf("Socket creation error\n");
        exit(0);
    }

    get_server(&server, argv[1], atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    while (1)
    {
        char command[100];
        memset(command, 0, 100);
        fflush(stdout);
        fflush(stdin);

        printf("> "); //1, "> ", 2);
        scanf("%99[^\n]%*c", command);

        if (send(sock, command, strlen(command), 0) == -1) // , (struct sockaddr *)&server, sizeof(server)) == -1)
            printf("Error in sending data\n");

        //printf("here i am!!\n");

        if (strcmp(command, "exit") == 0)
            break;
        else if (strncmp(command, "nano", 4) == 0)
        {
            char *buffer = NULL;
            int len = 0;

            while (1)
            {
                char tmp[BLOCK_SIZE];
                memset(tmp, 0, BLOCK_SIZE);
                scanf("%" CONCAT(BLOCK_SIZE) "[^\n]%*c", tmp);
                if (strlen(tmp) == 0)
                {
                    getchar();
                    break;
                }

                len = len + strlen(tmp);
                buffer = realloc(buffer, len+3);
                strcat(buffer, tmp);
                strcat(buffer, "\n");
                // buffer[len+2] = 0;
            }
            
            len = strlen(buffer) + 1;
            send(sock, &len, sizeof(len), 0);         // send length
            send(sock, buffer, strlen(buffer), 0);    // send content
        }

        char buf[1000];
        memset(buf, 0, 1000);
        recv(sock, buf, 999, 0);
        printf("%s\n", buf);
        // printf(">>>[%s]\n", buf);
    }

    close(sock);
    return 0;
}
