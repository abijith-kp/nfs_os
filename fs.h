#ifndef __FS_H
  #define __FS_H

#include "inode.h"

#define MIN(x, y) (((x) > (y))? (y) : (x))

char *read_buffer(INODE *inode, int size, int offset);
void write_buffer(INODE *in, char *buffer, int size, int offset);
int get_free_block();
void read_block(int block, char *buffer, int size, int offset);
void write_block(int block, char *buffer, int size, int offset);

#endif
