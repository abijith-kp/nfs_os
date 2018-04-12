#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <unistd.h>

#include "inode.h"
#include "superblock.h"
#include "constants.h"

extern SUPERBLOCK *superblock;
extern GHashTable *inode_index;
extern int fd;

void write_inode(INODE *in)
{
    int l[2];
    get_inode_location(in->inode_number, l);

    printf("write inode: %d %ld\n", in->inode_number, sizeof(INODE));

    lseek(fd, (l[0]*BLOCK_SIZE) + l[1], SEEK_SET);
    write(fd, in, sizeof(INODE));
}

INODE *read_inode(int i)
{
    int l[2];
    get_inode_location(i, l);

    printf("read inode: %d %ld\n", i, sizeof(INODE));

    INODE *inode = calloc(1, sizeof(INODE));
    lseek(fd, (l[0]*BLOCK_SIZE) + l[1], SEEK_SET);
    read(fd, inode, sizeof(INODE));
    return inode;
}

/* inode starts from 1 */
void get_inode_location(int i, int *location)
{
    int sb_size = (sizeof(SUPERBLOCK) / BLOCK_SIZE) + 1;
    int ni_block = BLOCK_SIZE / sizeof(INODE);

    location[0] = ((i - 1) / ni_block) + sb_size; /* block no. */
    location[1] = ((i - 1) % ni_block) * sizeof(INODE);
}


/* TODO: read from cache or disk */
/* Temp we create  new inode struct */
INODE *get_inode(int i)
{
    /* cache lookup */
    INODE *inode = g_hash_table_lookup(inode_index, GINT_TO_POINTER(i));
    if (inode)
        return inode;

    printf("no cache\n");
    return read_inode(i);
}

/* create new inode */
INODE *inode_alloc(int i, int filetype)
{
    INODE *inode = calloc(1, sizeof(INODE));
    inode->inode_number = i;
    inode->reference_count = 1;
    inode->used_entries = 1;
    inode->filetype = filetype;

    g_hash_table_insert(inode_index, GINT_TO_POINTER(i), inode);

    /* if needed write to disk */
    write_inode(inode);
    return inode;
}

/* acts like iget() */
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
            superblock->free_inodes_list[i] = 1;
            superblock->free_inode_index = i + 1;
            return i;
        }
        
        i = (i + 1) % MAX_INODES;
        if (i == 0)                 /* to ignore index 0 */
            i = (i + 1) % MAX_INODES;
    }

    printf("ERROR: No inodes free\n");
    return -1;
}