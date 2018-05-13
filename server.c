#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include "directories.h"
#include "superblock.h"
#include "commands.h"
#include "network.h"

#define MAX_LEN 100
#define MAX_USERS 5

int fd = -1;
SUPERBLOCK *superblock = NULL;
S_DIRECTORY *root_dir = NULL;

int is_created(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
        return 0;
    return 1;
}

/*
void print_dir(S_DIRECTORY *dir)
{
    INODE *in = get_inode(dir->inode);
    printf("=========\n");
    printf("inode: %d\n", dir->inode);
    printf("block: %d\n", in->entries[0]);
    printf("name: %s\n", dir->name);
    for (int i=0; i<dir->count; i++)
        printf("== %s %d ==\n", dir->dir_entry[i].filename, dir->dir_entry[i].inode_number);
    printf("=========\n");
}
*/

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

V_RETURN *shell_user(char *buffer)
{
    char *cmd = strtok(buffer, " ");
    char *dir = strtok(NULL, " ");
    V_RETURN *v_ret = NULL;

    if (strncmp(cmd, "e", 1) == 0)
        return calloc(1, sizeof(V_RETURN));
    else if (strcmp(cmd, "\n") == 0)
        return calloc(1, sizeof(V_RETURN));

    if (dir != NULL)
        dir[strlen(dir)-1] = 0;
    else
    {
        cmd[strlen(cmd)-1] = 0;
        dir = ".";
    }

    if (strcmp(cmd, "ls") == 0)
        v_ret = listdir(dir);
    else if (strcmp(cmd, "mk") == 0)
        v_ret = makedir(dir);
    else if (strcmp(cmd, "cd") == 0)
        v_ret = changedir(dir);
    else if (strcmp(cmd, "rm") == 0)
        v_ret = removedir(dir);
    else if (strcmp(cmd, "touch") == 0)
        v_ret = touch(dir);
    else if (strcmp(cmd, "nano") == 0)
        nano(dir);
    else if (strcmp(cmd, "cat") == 0)
        v_ret = cat_new(dir);
    else
    {
        v_ret = calloc(1, sizeof(V_RETURN));
        printf("Not implemented");
        v_ret->d = strdup("Not implemented");
        v_ret->r = 1;
    }

    if (!v_ret)
    {
        v_ret = calloc(1, sizeof(V_RETURN));
        v_ret->r = 1;
    }

    printf(">>>>>>>\n");
    return v_ret;
}

void *process_client(void *b)
{
    int sock = *(int *)b;
    int i = 1;

    char buffer[MAX_LEN];
    while (1)
    {
        memset(buffer, 0, MAX_LEN);
        int t = read_msg(sock, buffer, MAX_LEN-1);
        printf("buffer: %d %s %d\n", sock, buffer, t);
        if (t > 0)
        {
            V_RETURN *v_ret = shell_user(buffer);
            printf("<<<%s>><<%d>>\n", v_ret->d, v_ret->r);
            if (v_ret == NULL)
                break;
            send_msg(sock, v_ret->d, strlen(v_ret->d));
            if ((v_ret->r == 0) || (strcmp(buffer, "exit") == 0))
                break;
        }
        else
            break;
        i++;
    }
    
    close(sock);
    return NULL;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;
    char buffer[MAX_LEN];
    int length = sizeof(struct sockaddr_in);
    int sock, count = 0, socks[MAX_USERS];
    pthread_t users[MAX_USERS];

    if (argc != 4)
    {
        printf("Usage: %s server_ip server_port filesystem\n", argv[0]);
        exit(0);
    }

    init_fs(argv[3]);

    get_server(&server, argv[1], atoi(argv[2]), &sock);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Bind error\n");
        exit(0);
    }

    listen(sock, MAX_USERS);
    
    while (1)
    {
        if (count == MAX_USERS)
            break;
        int csock = accept(sock, (struct sockaddr *)&client, &length);
        if (-1 == csock)
        {
            printf("Accept error\n");
            continue;
        }

        socks[count] = csock;
        pthread_create(&(users[count]), NULL, &process_client, &(socks[count]));
        count++;
    }

    printf("Waiting for clients to terminate...\n");

    for (int i=0; i<MAX_USERS; i++)
    {
        pthread_join(users[i], NULL);
        close(socks[i]);
    }

    close(sock);
    uninit_fs();

    /*
    int csock = accept(sock, (struct sockaddr *)&client, &length);
    
    int i = 1;
    while (1)
    {
        memset(buffer, 0, MAX_LEN);
        int t = read_msg(csock, buffer, MAX_LEN-1);
        if (t > 0)
        {
            char *out = process_client(buffer);
            send_msg(csock, out, strlen(out));
        }
        i++;
    }
    */

    return 0;
}
