#include <stdio.h>
#include <string.h>

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
    printf("makedir: %s\n", path);

    while (tmp)
    {
        tmp = strtok(NULL, "/");
        printf("$$$$$ %s %s %s %s\n", prev, cur, root->name, tmp);
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

    printf(">> %s %s %s\n", prev, cur, root->name);
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
                printf("1File do not exist...\n");
                root_dir = get_directory(saved_pos);
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
        if (i == -1)
        {
            printf("File do not exist...\n");
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
                printf("1File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
        if (i == -1)
        {
            printf("File do not exist...\n");
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
                printf("1File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }

    if (cur != NULL)
    {
        int i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
        if (i == -1)
        {
            printf("File do not exist...\n");
            return;
        }
        
        INODE *inode = get_inode(i);
        if (inode->filetype != DIRECTORY)
            printf("Not a directory\n");
        else
        {
            S_DIRECTORY *dir = get_directory(i);
            if (!is_empty(dir))
                printf("Directory not empty\n");
            else
            {
                rm_entry_from_parent(root->inode, i);
                add_inode_to_free_list(i);
            }
        }
    }
    else
        printf("Root cannot be deleted\n");

    root_dir = get_directory(saved_pos);
}
