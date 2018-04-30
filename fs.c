#include <stdio.h>

#include "fs.h"
#include "superblock.h"

extern SUPERBLOCK *superblock;

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
            return i;
        }
        
        i = (i + 1) % MAX_BLOCKS;
        if (i == 0)                 /* to ignore index 0 */
            i = (i + 1) % MAX_BLOCKS;
    }

    printf("ERROR: No blocks free\n");
    return -1;
}
