#include "directories.h"
#include "superblock.h"

#define MAX_USERS 10
#define MAX_OPEN_INODE 10

typedef struct
{
    int sockfd;
    S_DIRECTORY *cur_dir;
} USER_LIST;

SUPERBLOCK *superblock = NULL;
INODE *inode_list[MAX_OPEN_INODE];
USER_LIST user_list[MAX_USERS];
int user_count = 0;

int add_user(int sockfd)
{
    if (MAX_USERS < user_count)
    {
        printf("No new user can be added\n");
        return ll
    }

    INODE *new_inode = calloc(1, sizeof(INODE));
    memcpy(new_inode, get_root_inode(), sizeof(INODE));
    
}
