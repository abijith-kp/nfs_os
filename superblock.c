#include <stdlib.h>
#include <stdio.h>

#include "directories.h"
#include "superblock.h"

SUPERBLOCK *superblock = NULL;
S_DIRECTORY *root_dir = NULL;

void init_superblock()
{
    if (NULL == superblock)
        superblock = calloc(1, sizeof(SUPERBLOCK));

    superblock->dirty = 1;
    superblock->free_inode_index = 1;
}
