#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "fs.h"
#include "constants.h"
#include "superblock.h"

extern SUPERBLOCK *superblock;
extern int fd;

int get_free_block()
{
    int i = superblock->free_blk_index;
    while (i != superblock->free_blk_index - 1)
    {
        int t = i / 8;
        int j = i % 8;
        unsigned char c = 1 << j;

        if (0 == (superblock->free_block_list[t] & c))
        {
            superblock->free_block_list[t] = superblock->free_block_list[t] | c;
            superblock->free_blk_index = (i + 1) % MAX_BLOCKS;

            char zero[BLOCK_SIZE];
            memset(zero, 0, BLOCK_SIZE);
            write_block(i, zero, BLOCK_SIZE, 0);
            return i;
        }
        
        i = (i + 1) % MAX_BLOCKS;
        if (i == 0)                 /* to ignore index 0 */
            i = (i + 1) % MAX_BLOCKS;
    }

    printf("ERROR: No blocks free\n");
    return -1;
}

void read_block(int block, char *buffer, int size, int offset)
{
    int ssize = sizeof(SUPERBLOCK);
    int t_offset = ssize + ((MAX_INODES + 1) * sizeof(INODE)) + ((block - 1) * BLOCK_SIZE) + offset;
    lseek(fd, t_offset, SEEK_SET);
    
    int len = MIN((BLOCK_SIZE - offset), size);
    read(fd, buffer, len);
}

void write_block(int block, char *buffer, int size, int offset)
{
    int ssize = sizeof(SUPERBLOCK);
    int t_offset = ssize + ((MAX_INODES + 1) * sizeof(INODE)) + ((block - 1) * BLOCK_SIZE) + offset;
    lseek(fd, t_offset, SEEK_SET);
    
    int len = MIN((BLOCK_SIZE - offset), size);
    write(fd, buffer, len);
}

void write_buffer(INODE *in, char *buffer, int size, int offset)
{
    if ((offset + size) > (MAX_ENTRIES * BLOCK_SIZE))
    {
        printf("Size beyond the MAX size\n");
        return;
    }

    int offset_b = offset / BLOCK_SIZE;
    
    for (int i=0; i<offset_b; i++)
    {
        if (in->entries[i] == 0)
            in->entries[i] = get_free_block();
    }

    if (in->entries[offset_b] == 0)
        in->entries[offset_b] = get_free_block();

    int offset_rest = offset - (offset_b * BLOCK_SIZE);
    int t = MIN(size, (BLOCK_SIZE - offset_rest));
    printf("size: %d, offset: %d, buffer: %s, t: %d\n", size, offset, buffer, t);
    write_block(in->entries[offset_b], buffer, t, offset_rest);
    buffer += t; // (BLOCK_SIZE - offset_rest);
    size -= t; // (BLOCK_SIZE - offset_rest);

    printf("inode: %d %d\n", in->entries[0], in->inode_number);
    for (int i=offset_b+1; i<MAX_ENTRIES; i++)
    {
        if (size <= 0)
            break;
        else if (in->entries[i] == 0)
            in->entries[i] = get_free_block();

        printf("buffer: %d [%s] %ld\n", size, buffer, strlen(buffer));
        write_block(in->entries[i], buffer, MIN(strlen(buffer), BLOCK_SIZE), 0);
        buffer += BLOCK_SIZE;
        size -= BLOCK_SIZE;
    }

    if (in->size < offset + size)
        in->size = offset + size;
    write_inode(in);
}

char *read_buffer(INODE *in, int size, int offset)
{
    if ((offset + size) > (MAX_ENTRIES * BLOCK_SIZE))
    {
        printf("Size beyond the MAX size\n");
        return NULL;
    }

    char *buffer = calloc(size, sizeof(char));
    char *b = buffer;

    // INODE *in = get_inode(inode);
    int offset_b = offset / BLOCK_SIZE;
    
    for (int i=0; i<offset_b; i++)
    {
        if (in->entries[i] == 0)
            in->entries[i] = get_free_block();
    }

    if (in->entries[offset_b] == 0)
        in->entries[offset_b] = get_free_block();

printf("read [%d] %d\n", in->inode_number, in->entries[offset_b]);
    int offset_rest = offset - (offset_b * BLOCK_SIZE);
printf("read [%d] %d %d\n", in->inode_number, in->entries[offset_b], offset_rest);

    read_block(in->entries[offset_b], buffer, (BLOCK_SIZE - offset_rest), offset_rest);
printf("read [%d] %d\n", in->inode_number, in->entries[offset_b]);
    buffer += (BLOCK_SIZE - offset_rest);
    size -= (BLOCK_SIZE - offset_rest);

    printf("inode000: %d %d\n", in->inode_number, in->size);
    for (int i=offset_b+1; i<MAX_ENTRIES; i++)
    {
        if (size <= 0)
            break;
        else if (in->entries[i] == 0)
            in->entries[i] = get_free_block();

        printf("buffer: %d [%s] %ld\n", size, b, strlen(buffer));
        write_block(in->entries[i], buffer, MIN(strlen(buffer), BLOCK_SIZE), 0);
        buffer += BLOCK_SIZE;
        size -= BLOCK_SIZE;
    }

    if (in->size < offset + size)
        in->size = offset + size;
    write_inode(in);
    return b;
}
