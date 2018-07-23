#ifndef __COMMANDS_H
  #define __COMMANDS_H

void listdir(char *path);
void makedir(char *path);
void changedir(char *path);
void removedir(char *path);
void touch(char *path);
void nano(char *path);
void cat(char *path);
void copy(char* src_path, char* dest_path);
void move(char* src_path, char* dest_path);

#endif
