#ifndef __COMMANDS_H
  #define __COMMANDS_H

typedef struct
{
    int r;
    char *d;
} V_RETURN;

V_RETURN *listdir(char *path);
V_RETURN *makedir(char *path);
V_RETURN *changedir(char *path);
V_RETURN *removedir(char *path);
V_RETURN *touch(char *path);
void nano(char *path);
void cat(char *path);
V_RETURN *cat_new(char *path);

#endif
