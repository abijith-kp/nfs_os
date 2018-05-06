#ifndef __DIRECTORIES_H
  #define __DIRECTORIES_H

#include "inode.h"

#define MAX_FILE_NAME 12
#define MAX_ENTRY 64

#define CUR_DIR "."
#define PARENT_DIR ".."

#define NEWDIR 1
#define CREATD 2

#define ROOT_INODE 1

typedef struct
{
    char filename[MAX_FILE_NAME];
    int inode_number;
} DIR_ENTRY;

typedef struct
{
    char name[50];
    int count;
    int inode;
    DIR_ENTRY dir_entry[MAX_ENTRY];
} S_DIRECTORY;

#define make_directory(name, parent) _make_directory(name, parent, 0)

S_DIRECTORY *mk_root();
S_DIRECTORY *get_directory(int inode);
S_DIRECTORY *_make_directory(char *name, int parent, int is_root);
void add_entry_to_parent(int parent, char *name, int inode);
void write_dir(INODE *inode, S_DIRECTORY *dir);
void read_dir(INODE *inode, S_DIRECTORY *dir);
S_DIRECTORY *make_root();
int get_inode_dir(S_DIRECTORY *p_directory, char *d);
void rm_entry_from_parent(int parent, int child);
int is_empty(S_DIRECTORY *dir);
void write_directory(S_DIRECTORY *dir);

#endif
