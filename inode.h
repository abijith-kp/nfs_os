#ifndef __INODE_H
  #define __INODE_H

#define MAX_ENTRIES 13

#define DIRECTORY 0
#define REGULAR   1

typedef struct
{
    int inode_number;

    /**
     * Initial 10 are direct entries.
     * 11th entry is single indirect.
     * 12th entry is double indirect.
     * 13th entry is triple indirect.
     **/
    int entries[MAX_ENTRIES];
    int used_entries;

    /**
     * DIRECTORY -> directory
     * REGULAR -> regular file
     **/
    int filetype;

    int reference_count;
    int size;

} INODE;

void get_inode_location(int i, int *location);
INODE *inode_alloc(int i, int filetype);
INODE *get_inode(int i);
int get_free_inode();

#endif
