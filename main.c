#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fs.h"
#include "inode.h"
#include "commands.h"
#include "constants.h"
#include "superblock.h"
#include "directories.h"

SUPERBLOCK *superblock = NULL;
S_DIRECTORY *root_dir = NULL;
int fd = -1;

int is_created(char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
        return 0;
    return 1;
}

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

void shell()
{
    while (1)
    {
        char buffer[20];
        memset(buffer, 0, 20);
        write(1, "> ", 2);
        read(0, buffer, 19);
        char *cmd = strtok(buffer, " ");
        char *dir = strtok(NULL, " ");

        if (strncmp(cmd, "e", 1) == 0)
            break;
        else if (strcmp(cmd, "\n") == 0)
            continue;

        if (dir != NULL)
            dir[strlen(dir)-1] = 0;
        else
        {
            cmd[strlen(cmd)-1] = 0;
            dir = ".";
        }

        printf("||| %s %s\n", cmd, dir);
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
            nano(dir);
        else if (strcmp(cmd, "cat") == 0)
            cat(dir);
        else
            printf("Not implemented");

        printf("\n");
    }
}

int length(char *argv[])
{
    int l = 0;
    while (*argv++)
        l++;
    return l;
}

void run_test()
{
    char *cmd[][10] = {
                        { "ls", "/", NULL },
                        { "mk", "/d", NULL },
                        { "ls", "/", NULL },
                        { "e", NULL },
                        { NULL }
                      };

    int i=0;
    printf("------------\n");
    while (cmd[i][0])
    {
        for (int j=0; j<length(cmd[i]); j++)
            printf("%s ", cmd[i][j]);

        printf("\n\n");

        if (strcmp(cmd[i][0], "ls") == 0)
            listdir(cmd[i][1]);
        else if (strcmp(cmd[i][0], "mk") == 0)
            makedir(cmd[i][1]);
        else if (strcmp(cmd[i][0], "e") == 0)
            break;

        i++;
        printf("------------\n");
    }
}


int main(int argc, char *argv[])
{
    init_fs("network.fs");

#ifndef TEST
    shell();
#else
    INODE *in = get_inode(5);
    printf("[[[[inode: %d %d\n", in->inode_number, in->entries[0]);
    char b[500], *out;
    memset(b, 0, 500);
    out = read_buffer(in, 10, 0);
    printf("[%s]\n", out);
#endif

    uninit_fs();

    return 0;
}
