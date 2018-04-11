#include <stdlib.h>
#include <string.h>
#include <gmodule.h>

#include "directories.h"

GHashTable *directory_index = NULL;

void write_directory(S_DIRECTORY *dir)
{
    /* write directory metadata into disk */
}

S_DIRECTORY *mk_root()
{
    int i = get_free_inode();

    /* TODO: add root_inode to inode-cache */
    INODE *root_inode = inode_alloc(i, DIRECTORY);

    S_DIRECTORY *dir = calloc(1, sizeof(S_DIRECTORY));
    strncpy((dir->dir_entry[0]).filename, CUR_DIR, MAX_FILE_NAME);
    strncpy((dir->dir_entry[1]).filename, PARENT_DIR, MAX_FILE_NAME);

    (dir->dir_entry[0]).inode_number = i;
    (dir->dir_entry[1]).inode_number = i;

    /* write_directory(dir); */
    /* writing into cache as of now */
    g_hash_table_insert(directory_index, GINT_TO_POINTER(i), dir);
    return dir;
}

void add_entry_to_parent(S_DIRECTORY *parent, char *c_name, int c_inode)
{
    for (int i=0; i<MAX_ENTRIES; i++)
    {
        if (parent->dir_entry[i].inode_number == 0)
        {
            parent->dir_entry[i].inode_number = c_inode;
            strncpy(parent->dir_entry[i].filename, c_name, MAX_FILE_NAME);
            break;
        }
    }
}

S_DIRECTORY *get_directory(INODE *in, int parent_inode)
{
    S_DIRECTORY *dir = g_hash_table_lookup(directory_index, GINT_TO_POINTER(in->inode_number));
    if (dir)
        return dir;

    dir = calloc(1, sizeof(S_DIRECTORY));
    strncpy((dir->dir_entry[0]).filename, CUR_DIR, MAX_FILE_NAME);
    strncpy((dir->dir_entry[1]).filename, PARENT_DIR, MAX_FILE_NAME);

    (dir->dir_entry[0]).inode_number = in->inode_number;
    (dir->dir_entry[1]).inode_number = parent_inode;

    /* write_directory(dir); */
    g_hash_table_insert(directory_index, GINT_TO_POINTER(in->inode_number), dir);

    return dir;
}
