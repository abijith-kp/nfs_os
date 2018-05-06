#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "directories.h"
#include "superblock.h"

SUPERBLOCK *superblock = NULL;
S_DIRECTORY *root_dir = NULL;

extern int fd;

void init_superblock(char *fs_name, int flag)
{
    printf("flag: %d\n", flag);
    superblock = calloc(1, sizeof(SUPERBLOCK));
    if (flag == 0)
    {
        superblock->dirty = 1;
        superblock->free_inode_index = 1;
        superblock->free_blk_index = 1;
        strcpy(superblock->fs_name, fs_name);
    }
    else
    {
            printf(";;;; %d\n", superblock->free_inode_index);
            // int t = open(fs_name, O_RDONLY);
            read(fd, superblock, sizeof(SUPERBLOCK));
            // close(t);
            printf(";;;; %d\n", superblock->free_inode_index);
    }

    printf("superblock: %d\n", superblock->free_inode_index);
}
