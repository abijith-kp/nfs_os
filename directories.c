#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fs.h"
#include "constants.h"
#include "superblock.h"
#include "directories.h"

extern int fd;
extern SUPERBLOCK *superblock;
extern S_DIRECTORY *root_dir;

void write_directory(S_DIRECTORY *dir)
{
    INODE *inode = get_inode(dir->inode);
    int block = inode->entries[0];

    int ssize = sizeof(SUPERBLOCK);
    int offset = ssize + ((MAX_INODES + 1) * sizeof(INODE)) + ((block - 1) * BLOCK_SIZE);
    lseek(fd, offset, SEEK_SET);
    write(fd, dir, sizeof(S_DIRECTORY));
}


S_DIRECTORY *get_directory(int inode)
{
    INODE *c_inode = get_inode(inode);
    S_DIRECTORY *dir = calloc(1, sizeof(S_DIRECTORY));
    int ssize = sizeof(SUPERBLOCK);

    int block = c_inode->entries[0];
    int offset = ssize + ((MAX_INODES + 1) * sizeof(INODE)) + ((block - 1) * BLOCK_SIZE);
    lseek(fd, offset, SEEK_SET);
    read(fd, dir, sizeof(S_DIRECTORY));

    return dir;
}

void rm_entry_from_parent(int parent, int child)
{
    S_DIRECTORY *p_directory = get_directory(parent);
    DIR_ENTRY *dentry = p_directory->dir_entry;

    for (int i=2; i<MAX_ENTRIES; i++)
    {
        if (dentry[i].inode_number == child)
        {
            dentry[i].inode_number = 0;
            p_directory->count--;
            write_directory(p_directory);
            break;
        }
    }
}

void add_entry_to_parent(int parent, char *name, int inode)
{
    S_DIRECTORY *p_directory = get_directory(parent);
    DIR_ENTRY *dentry = p_directory->dir_entry;

    printf(">> %d %s %d\n", parent, name, inode);
    for (int i=2; i<MAX_ENTRIES; i++)
    {
        printf("$$ %d %d %s\n", i, dentry[i].inode_number, dentry[i].filename);
        if (dentry[i].inode_number == 0)
        {
            strcpy(dentry[i].filename, name);
            dentry[i].inode_number = inode;
            p_directory->count++;
            write_directory(p_directory);
            return;
        }
    }
}

int get_inode_dir(S_DIRECTORY *p_directory, char *d)
{
    DIR_ENTRY *dentry = p_directory->dir_entry;

    for (int i=2; i<MAX_ENTRIES; i++)
    {
        if ((strcmp(d, dentry[i].filename) == 0) && (dentry[i].inode_number != 0))
            return dentry[i].inode_number;
    }

    return -1;
}


S_DIRECTORY *make_root()
{
    S_DIRECTORY *root = _make_directory("/", ROOT_INODE, 1);
    return root;
}


S_DIRECTORY *_make_directory(char *name, int parent, int is_root)
{
    INODE *c_inode = alloc_inode(DIRECTORY);
    int inode = c_inode->inode_number;

    /*
    int start = 0;
    if (BLOCK_SIZE % sizeof(INODE) == 0)
        start = BLOCK_SIZE / sizeof(INODE);
    else
        start = (BLOCK_SIZE / sizeof(INODE)) + 1;
    */

    int block = get_free_block();
    printf("block: (%d) %d %d\n", c_inode->inode_number, block, superblock->free_blk_index);
    S_DIRECTORY *dir = calloc(1, sizeof(S_DIRECTORY));
    strcpy(dir->name, name);
    dir->inode = inode;
    dir->count = 2;

    DIR_ENTRY *dentry = dir->dir_entry;
    strcpy(dentry[0].filename, CUR_DIR);
    dentry[0].inode_number = inode;
    strcpy(dentry[1].filename, PARENT_DIR);
    dentry[1].inode_number = parent;

    c_inode->used_entries++;
    c_inode->entries[0] = block;

    if (!is_root)
        add_entry_to_parent(parent, dir->name, dir->inode);

    write_inode(c_inode);
    write_directory(dir);

    return dir;
}

int is_empty(S_DIRECTORY *dir)
{
    int c = 0;
    for (int i=0; i<MAX_ENTRIES; i++)
    {
        if (dir->dir_entry[i].inode_number != 0)
            c++;
        if (c == 3)
            return 0;
    }

    return 1;
}
