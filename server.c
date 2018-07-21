#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "fs.h"
#include "inode.h"
#include "commands.h"
#include "constants.h"
#include "superblock.h"
#include "directories.h"

#define MAX_LEN 100
#define OUTPUT_LEN 1000

SUPERBLOCK *superblock = NULL;
S_DIRECTORY *root_dir = NULL;
int fd = -1;


/********* commands *******/

void edit_file(char *path, char *content)
{
    char buffer[BLOCK_SIZE];
    int offset = 0;
    INODE *inode;

    /* get inode from loping */

    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        
        inode = get_inode(i);
    }
    else
    {
        printf("Invalid path -- root\n");
        return;
    }


    int size = strlen(content);
    int t = 0;
    /*
    while (1)
    {
        if (size == 0)
            break;

        t = MIN(size, BLOCK_SIZE);
        write_buffer(inode, content+offset, t, offset);
        offset += t;
        size -= t;
    }
    */
    
    write_buffer(inode, content, size, 0);
    write_inode(inode);
    root_dir = get_directory(saved_pos);
}

void __nano(char *path, int client_sock)
{
    int length = 0;
    recv(client_sock, &length, sizeof(length), 0);  // assuming there is no error
    int offset = 0;

    char *buffer = calloc(length+1, sizeof(char));
    recv(client_sock, buffer, length, 0);  // assuming there is no error
    /*
    while (length)
    {
        int t = recv(client_sock, buffer+offset, length, 0);  // assuming there is no error
        length -= t;
        offset += t;
    }
    */
    edit_file(path, buffer);
}


/*************************/






void print_dir(S_DIRECTORY *dir)
{
    // INODE *in = get_inode(dir->inode);
    printf("inode: %d\n", dir->inode);
    printf("name: %s\n", dir->name);
    for (int i=0; i<MAX_ENTRY; i++)
    {
        if (dir->dir_entry[i].inode_number)
            printf(" %s\t%d\n", dir->dir_entry[i].filename, dir->dir_entry[i].inode_number);
    }
}

/*
 * status values:
 *      0 ==> exit
 *      1 ==> read next command
 *     -1 ==> continue. print enpty string
 */
int status = 1;
char *shell(char *buffer, char *output, int client_sock)
{
    status = 1;
    char *cmd = strtok(buffer, " ");
    char *dir = strtok(NULL, " ");

    printf("command: %s %s\n", cmd, dir);

    if ((strncmp(cmd, "e", 1) == 0) || (strncmp(cmd, "exit", 4) == 0))
    {
        status = 0;
        return NULL;
    }
    else if (strcmp(cmd, "\n") == 0)
    {
        status = -1;
        return NULL;
    }

    if (dir != NULL)
        dir[strlen(dir)-1] = 0;
    else
    {
        cmd[strlen(cmd)-1] = 0;
        dir = ".";
    }

    
    memset(output, 0, OUTPUT_LEN);
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, output);


    if (strcmp(cmd, "ls") == 0)
        listdir(dir);
    else if (strcmp(cmd, "mk") == 0)
        makedir(dir);
    else if (strcmp(cmd, "cd") == 0)
        changedir(dir);
    else if (strcmp(cmd, "rm") == 0)
        removedir(dir);
    else if (strcmp(cmd, "touch") == 0)
        touch(dir);
    else if (strcmp(cmd, "nano") == 0)
        __nano(dir, client_sock);
        // printf("nano %s\n", dir);
        //nano(dir);
    else if (strcmp(cmd, "cat") == 0)
        cat(dir);
    else
        printf("Not implemented");

    printf("\n");

    freopen("/dev/tty", "a", stdout);

    return output;
}

int is_created(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
        return 0;
    return 1;
}

int init_fs(char *name)
{
    int flag = is_created(name);
    fd = open(name, O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
    superblock = calloc(1, sizeof(SUPERBLOCK));

    if (flag == 1)
    {
        read(fd, superblock, sizeof(SUPERBLOCK));
        root_dir = get_directory(ROOT_INODE);
    }
    else
    {
        strcpy(superblock->fs_name, name);
        memset(superblock->free_block_list, 0, sizeof(superblock->free_block_list));
        superblock->free_blk_index = 1;
        memset(superblock->free_inodes_list, 0, sizeof(superblock->free_inodes_list));
        superblock->free_inode_index = 1;

        root_dir = make_root();
    }

    return flag;
}

void uninit_fs()
{
    lseek(fd, 0, SEEK_SET);
    write(fd, superblock, sizeof(SUPERBLOCK));
    close(fd);
}

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
    char *buffer = calloc(MAX_LEN, sizeof(char));
    int length = sizeof(struct sockaddr_in);

    init_fs("network.fs");

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    listen(sock, 5);
    while (1)
    {
        memset(buffer, 0, MAX_LEN);
        memset(&client, 0, sizeof(client));

        printf("till here\n");
        length = sizeof(struct sockaddr_in);
        int client_sock =  accept(sock, (struct sockaddr *)&client, &length);

        int count = 0;
        while (1)
        {
            char *output = calloc(OUTPUT_LEN, sizeof(char));
            memset(buffer, 0, MAX_LEN);
            if (recv(client_sock, buffer, MAX_LEN-1, 0) == -1) //, (struct sockaddr *)&client, &length) == -1)
                printf("Error in recvfrom()\n");
            else
            {
                buffer[strlen(buffer)] = '\n';
                // printf("SERVER: %s", buffer);
                // fflush(stdout);

                shell(buffer, output, client_sock);
                if (status == 0) //(strcmp(buffer, "exit") == 0)
                    break;
                // memset(buffer, 0, MAX_LEN);
                sprintf(output, "%s%s", output, " ");
                send(client_sock, output, strlen(output), 0);
                // printf("end of command\n");
            }
        }

        close(client_sock);
    }

    uninit_fs();

    return 0;
}
