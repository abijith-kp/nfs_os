#ifndef __DIRECTORIES_H
  #define __DIRECTORIES_H

#include "inode.h"

#define MAX_FILE_NAME 12
#define MAX_ENTRY 64

#define CUR_DIR "."
#define PARENT_DIR ".."

typedef struct
{
    char filename[MAX_FILE_NAME];
    int inode_number;
} DIR_ENTRY;

typedef struct
{
    DIR_ENTRY dir_entry[MAX_ENTRY];
} S_DIRECTORY;

S_DIRECTORY *mk_root();
S_DIRECTORY *get_directory(INODE *in, int parent_inode);
void add_entry_to_parent(S_DIRECTORY *parent, char *c_name, int c_inode);

#endif
