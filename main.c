#include <stdio.h>
#include <string.h>
#include <gmodule.h>

#include "inode.h"
#include "directories.h"
#include "superblock.h"

extern SUPERBLOCK *superblock;
extern S_DIRECTORY *root_dir;

GHashTable *inode_index = NULL;
extern GHashTable *directory_index;

void init_fs()
{
    inode_index = g_hash_table_new(g_direct_hash, g_direct_equal);
    directory_index = g_hash_table_new(g_direct_hash, g_direct_equal);
    init_superblock();

    /* we'll create a root directory also */
    root_dir = mk_root();
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
void mkdir(char *dirname)
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
            if (strcmp(d, dir_entry[j].filename) == 0)
            {
                if (dir_entry[j].inode_number != root->dir_entry[1].inode_number)
                {
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

int main(int argc, char *argv[])
{
    printf("%p %p\n", superblock, root_dir);
    init_fs();
    printf("%p %p\n", superblock, root_dir);

    int l[2];
    get_inode_location(1, l);
    printf("|| %d %d\n", l[0], l[1]);
    int i = 0;
    // printf("inode: %d\n", (i = get_free_inode()));

    // INODE *inode = inode_alloc(i, REGULAR);
    // printf(">> %d %d\n", inode->inode_number, inode->filetype);

    printf("=============\n");
    listdir("/");
    printf("=============\n");
    mkdir(argv[1]);
    printf("=============\n");
    listdir("/");
    printf("=============\n");
    mkdir(argv[2]);
    printf("=============\n");
    listdir("/d");
    printf("=============\n");
    return 0;
}
