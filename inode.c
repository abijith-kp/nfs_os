#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "inode.h"
#include "superblock.h"

extern SUPERBLOCK *superblock;
extern int fd;

void write_inode(INODE *n_inode)
{
    int i = n_inode->inode_number;
    int ssize = sizeof(SUPERBLOCK);
    int offset = ssize + ((i - 1) * sizeof(INODE));
    lseek(fd, offset, SEEK_SET);
    write(fd, n_inode, sizeof(INODE));
}

INODE *get_inode(int inode)
{
    int ssize = sizeof(SUPERBLOCK);
    int offset = ssize + ((inode - 1) * sizeof(INODE));
    INODE *c_inode = calloc(1, sizeof(INODE));
    lseek(fd, offset, SEEK_SET);
    read(fd, c_inode, sizeof(INODE));
    return c_inode;
}

INODE *alloc_inode(int filetype)
{
    int i = get_free_inode();
    INODE *n_inode = calloc(1, sizeof(INODE));

    n_inode->inode_number = i;
    n_inode->filetype = filetype;
    n_inode->used_entries = 0;
    n_inode->size = 0;

    int ssize = sizeof(SUPERBLOCK);
    int offset = ssize + ((i - 1) * sizeof(INODE));
    lseek(fd, offset, SEEK_SET);
    write(fd, n_inode, sizeof(INODE));
    return n_inode;
}

int get_free_inode()
{
    int i = superblock->free_inode_index;
    while (i != superblock->free_inode_index - 1)
    {
        int t = i / 8;
        int j = i % 8;
        unsigned char c = 1 << j;

        if (0 == (superblock->free_inodes_list[t] & c))
        {
            superblock->free_inodes_list[t] = superblock->free_inodes_list[t] | c;
            superblock->free_inode_index = (i + 1) % MAX_INODES;
            return i;
        }
        
        i = (i + 1) % MAX_INODES;
        if (i == 0)                 /* to ignore index 0 */
            i = (i + 1) % MAX_INODES;
    }

    printf("ERROR: No inodes free\n");
    return -1;
}

void add_inode_to_free_list(int i)
{
    int t = i / 8;
    int j = i % 8;
    unsigned char c = 1 << j;
    superblock->free_inodes_list[t] = superblock->free_inodes_list[t] | c;
}
