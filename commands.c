#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"
#include "constants.h"
#include "directories.h"

extern S_DIRECTORY *root_dir;

void makedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;

            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                cur = NULL;
                break;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        if (i != -1)
        {
            printf("Directory alread exists\n");
            root_dir = get_directory(saved_pos);
            return;
        }

        make_directory(cur, root->inode);
    }
    else
    {
        printf("Incorrect path\n");
    }
    
    root_dir = get_directory(saved_pos);
}

void listdir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    if (strcmp(CUR_DIR, path) == 0)
    {
        print_dir(root);
        root_dir = get_directory(saved_pos);
        return;
    }

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                root_dir = get_directory(saved_pos);
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            root_dir = get_directory(saved_pos);
            return;
        }
        
        INODE *in = get_inode(i);
        if (in->filetype != DIRECTORY)
        {
            printf("%s\t%d\n", cur, i);
            root_dir = get_directory(saved_pos);
            return;
        }

        root = get_directory(i);
    }

    print_dir(root);
    root_dir = get_directory(saved_pos);
}

void changedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        INODE *in = get_inode(i);
        if (in->filetype != DIRECTORY)
        {
            printf("Not a directory\n");
            return;
        }

        root_dir = get_directory(i);
    }
    else
        root_dir = get_directory(ROOT_INODE);
}

void removedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        
        S_DIRECTORY *dir = get_directory(i);
        if (!is_empty(dir))
            printf("Directory not empty\n");
        else
        {
            rm_entry_from_parent(root->inode, i);
            add_inode_to_free_list(i);
        }
    }
    else
        printf("Root cannot be deleted\n");

    root_dir = get_directory(saved_pos);
}

void touch(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;

            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                cur = NULL;
                break;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        if (i != -1)
        {
            printf("Directory alread exists\n");
            root_dir = get_directory(saved_pos);
            return;
        }

        INODE *inode = alloc_inode(REGULAR);
        add_entry_to_parent(root->inode, cur, inode->inode_number);
    }
    else
    {
        printf("Incorrect path\n");
    }
    
    root_dir = get_directory(saved_pos);
}

void nano(char *path)
{
    char buffer[BLOCK_SIZE];
    int offset = 0;
    INODE *inode;

    /* get inode from loping */

    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        
        inode = get_inode(i);
    }
    else
    {
        printf("Invalid path -- root\n");
        return;
    }

    while (1)
    {
        memset(buffer, 0, BLOCK_SIZE);
        int t = read(1, buffer, BLOCK_SIZE);
        if (strcmp(buffer, "\n") == 0)
            break;

        write_buffer(inode, buffer, t, offset);
        // offset += BLOCK_SIZE;
        offset += strlen(buffer);
    }

    write_inode(inode);
    root_dir = get_directory(saved_pos);
}

void cat(char *path)
{
    char *buffer;
    int offset = 0;
    INODE *inode;

    /* get inode from loping */

    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        if (tmp)
        {
            prev = cur;
            cur = tmp;
            int i = get_inode_dir(root, prev);
            if (i == -1)
            {
                printf("File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        
        inode = get_inode(i);
    }
    else
    {
        printf("Invalid path -- root\n");
        return;
    }

    offset = inode->size;

    int off = 0;
    while (1)
    {
        if (offset <= 0)
            break;

        buffer = read_buffer(inode, BLOCK_SIZE, off);
        // write(1, buffer, BLOCK_SIZE);
        printf("%s", buffer);
        offset -= BLOCK_SIZE;
        off += BLOCK_SIZE;
    }

    root_dir = get_directory(saved_pos);
}
