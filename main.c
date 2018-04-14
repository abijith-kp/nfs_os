#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "avl.h"
#include "inode.h"
#include "directories.h"
#include "superblock.h"

extern SUPERBLOCK *superblock;
extern S_DIRECTORY *root_dir;

NODE *inode_index = NULL;
extern NODE *directory_index;

int fd = -1;

void init_fs(char *fs_name)
{
    // inode_index = g_hash_table_new(g_direct_hash, g_direct_equal);
    // directory_index = g_hash_table_new(g_direct_hash, g_direct_equal);

    init_superblock();

    /* we'll create a root directory also */
    root_dir = mk_root();

    fd = open(fs_name, O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
}

void _listdir(S_DIRECTORY *dir)
{
    DIR_ENTRY *dir_entry = dir->dir_entry;
    for (int j=0; j< MAX_ENTRIES; j++)
    {
        if (dir_entry[j].inode_number == 0)
            continue;
        printf("%d\t%s\n", dir_entry[j].inode_number,
                           dir_entry[j].filename);
    }
}

/* always absolute path should be given */
void listdir(char *dir)
{
    S_DIRECTORY *root = root_dir;
    char *d = NULL;
    int flag = 0;

    if (strcmp(dir, "/") == 0)
    {
        _listdir(root);
        return;
    }

    char *dd = strdup(dir);
    d = strtok(dd, "/");
    while (d)
    {
        flag = -1;
        DIR_ENTRY *dir_entry = root->dir_entry;
        for (int j=0; j< MAX_ENTRIES; j++)
        {
            if (strcmp(d, dir_entry[j].filename) == 0)
            {
                if (dir_entry[j].inode_number != root->dir_entry[1].inode_number)
                {
                    INODE *di = get_inode(dir_entry[j].inode_number);
                    S_DIRECTORY *new_root = get_directory(di, root->dir_entry[1].inode_number);
                    if (di->filetype == DIRECTORY)
                        root = new_root;
                    else if (di->filetype == REGULAR)
                    {
                        flag = -1;
                        break;
                    }
                }
                flag = 1;
                break;
            }
        }

        if ((flag == -1) || (flag == 0))
            break;
        d = strtok(NULL, "/");
    }

    if (flag == -1)
        printf("Not directory: %s ==> %s\n", d, dir);
    if ((!d) && flag)
    {
        _listdir(root);
        return;
    }

    printf("Not directory: %s ==> %s\n", d, dir);
}

/* always absolute path should be given */
void makedir(char *dirname)
{
    char *dd = strdup(dirname);
    S_DIRECTORY *root = root_dir;

    int flag = 0;

    char *pos;
    char *d = strtok_r(dd, "/", &pos);

    while (d)
    {
        flag = -1;
        DIR_ENTRY *dir_entry = root->dir_entry;
        for (int j=0; j< MAX_ENTRIES; j++)
        {
            if (dir_entry[j].inode_number == 0)
                continue;
            printf(">%s %s\n", d, dir_entry[j].filename);
            if (strcmp(d, dir_entry[j].filename) == 0)
            {
                if (dir_entry[j].inode_number != root->dir_entry[1].inode_number)
                {
                    printf("<<<\n");
                    INODE *di = get_inode(dir_entry[j].inode_number);
                    S_DIRECTORY *new_root = get_directory(di, root->dir_entry[1].inode_number);
                    if (di->filetype == DIRECTORY)
                    {
                        root = new_root;
                    }
                    else if (di->filetype == REGULAR)
                    {
                        break;
                    }
                }
                flag = 1;
                break;
            }
        }

        if (flag == -1)
            break;
        d = strtok_r(NULL, "/", &pos);
    }

    if (flag == -1)
    {
        if (strlen(pos) == 0)
        {
            // printf("make new directory: %d %s %ld\n", root->dir_entry[0].inode_number, pos, strlen(pos));

            printf("alloc\n");
            int i = get_free_inode();
            INODE *di = inode_alloc(i, DIRECTORY);

            /* add dir variable to cache */
            S_DIRECTORY *dir = get_directory(di, root->dir_entry[0].inode_number);
            add_entry_to_parent(root, d, i);
        }
        else
            printf("Wrong directory path: %s ==> %s\n", d, dirname);
        return;
    }

    printf("Directory already existing: %s ==> %s\n", d, dirname);
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

        dir[strlen(dir)-1] = 0;

        if (strcmp(cmd, "ls") == 0)
            listdir(dir);
        else if (strcmp(cmd, "mk") == 0)
            makedir(dir);
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

    #ifdef TEST
    run_test();
    #else
    shell();
    #endif

    close(fd);
    return 0;
}
