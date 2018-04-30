#ifndef __SUPERBLOCK_H
  #define __SUPERBLOCK_H

#define MAX_BLOCKS (512 * sizeof(char)) /* in bits */
#define MAX_INODES (512 * sizeof(char)) /* in bits */

typedef struct
{
    char fs_name[250];
    int fs_size;
    int num_free_blocks;
    char free_block_list[MAX_BLOCKS + 1]; /* max free blocks is 512 * 8 */
    int free_blk_index;
    int inode_size;
    int num_free_inodes;
    int free_inodes_list[MAX_INODES + 1];
    int free_inode_index;
    int lock;
    int dirty;
} SUPERBLOCK;

void init_superblock(char *fs_name, int flag);

#endif
