#include "superblock.h"
#include "inode.h"

#define BLOCK_SIZE 1024

int num_entries_needed(int len)
{
    if ((len / BLOCK_SIZE) < 10)
        return len / BLOCK_SIZE;
    else ((len - (10 * BLOCK_SIZE)) / (BLOCK_SIZE * BLOCK_SIZE / sizeof()))
}

int read(char *filename, char *buffer, int l, int offset)
{
    int inode = get_inode(filename);
    int entries = num_entries_needed(offset + l);
}
