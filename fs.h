#ifndef __FS_H
  #define __FS_H

#include "inode.h"

int read_buffer(INODE *in, char *buffer, int len, int offset);
int write_buffer(INODE *in, char *buffer, int len, int offset);
int get_free_block();

#endif
