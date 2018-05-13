#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "fs.h"
#include "commands.h"
#include "constants.h"
#include "directories.h"

extern S_DIRECTORY *root_dir;

V_RETURN *makedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));

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
            v_ret->d = strdup("Directory alread exist\n");
            v_ret->r = 0;
            return v_ret;
        }

        make_directory(cur, root->inode);
    }
    else
    {
        printf("Incorrect path\n");
        v_ret->d = strdup("Incorrect path\n");
        v_ret->r = 0;
    }
    
    root_dir = get_directory(saved_pos);
    v_ret->d = strdup("\n");
    v_ret->r = 1;
    return v_ret;
}

void print_dir(S_DIRECTORY *dir, char *buffer)
{
    char b[100];

    INODE *in = get_inode(dir->inode);
    printf("=========\n");
    strcat(buffer, "=========\n");

    memset(b, 0, 100);
    sprintf(b, "inode: %d\n", dir->inode);
    printf("%s", b);
    strcat(buffer, b);

    memset(b, 0, 100);
    sprintf(b, "block: %d\n", in->entries[0]);
    printf(b); //"name: %s\n", dir->name);
    strcat(buffer, b);

    for (int i=0; i<dir->count; i++)
    {
        memset(b, 0, 100);
        sprintf(b, "== %s %d ==\n", dir->dir_entry[i].filename, dir->dir_entry[i].inode_number);
        printf("%s", b);
        strcat(buffer, b);
    }

    strcat(buffer, "=========\n");
    printf("=========\n");
}

V_RETURN *listdir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));
    char *buffer = calloc(500, sizeof(char));

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    S_DIRECTORY *root = root_dir;

    if (strcmp(CUR_DIR, path) == 0)
    {
        print_dir(root, buffer);
        root_dir = get_directory(saved_pos);
        v_ret->d = buffer;
        v_ret->r = 1;
        return v_ret;
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
                v_ret->d = strdup("File do not exist...\n");
                v_ret->r = 1;
                root_dir = get_directory(saved_pos);
                return v_ret;
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
            v_ret->d = strdup("File do not exist...\n");
            v_ret->r = 1;
            root_dir = get_directory(saved_pos);
            return v_ret;
        }
        
        INODE *in = get_inode(i);
        if (in->filetype != DIRECTORY)
        {
            char b[100];
            memset(buffer, 0, 500);
            printf("=========\n");
            strcat(buffer, "=========\n");
            memset(b, 0, 100);
            sprintf(b, "name: %s\n", cur);
            strcat(buffer, b);
            printf("%s", b);

            memset(b, 0, 100);
            sprintf(b, "inode: %d\n", i);
            strcat(buffer, b);
            printf("%s", b);

            strcat(buffer, "=========\n");
            printf("=========\n");
            root_dir = get_directory(saved_pos);

            v_ret->d = strdup(buffer);
            v_ret->r = 1;
            return v_ret;
        }

        root = get_directory(i);
    }

    print_dir(root, buffer);
    root_dir = get_directory(saved_pos);
    v_ret->d = buffer;
    v_ret->r = 1;
    return v_ret;
}

V_RETURN *changedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));

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
                v_ret->d = strdup("1File do not exist...\n");
                v_ret->r = 1;
                return v_ret;
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
            v_ret->d = strdup("File do not exist...\n");
            v_ret->r = 1;
            return v_ret;
        }
        INODE *in = get_inode(i);
        if (in->filetype != DIRECTORY)
        {
            printf("Not a directory\n");
            v_ret->d = strdup("Not a dir_entry\n");
            v_ret->r = 1;
            return v_ret;
        }

        root_dir = get_directory(i);
    }
    else
        root_dir = get_directory(ROOT_INODE);

    v_ret->d = strdup("\n");
    v_ret->r = 1;
    return v_ret;
}

V_RETURN *removedir(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));

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
                v_ret->d = strdup("1File do not exist...\n");
                v_ret->r = 1;
                return v_ret;
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
            v_ret->d = strdup("File do not exist...\n");
            v_ret->r = 1;
            return v_ret;
        }
        
        S_DIRECTORY *dir = get_directory(i);
        if (!is_empty(dir))
        {
            printf("Directory not empty\n");
            v_ret->d = strdup("Directory not empty\n");
            v_ret->r = 1;
            return v_ret;
        }
        else
        {
            rm_entry_from_parent(root->inode, i);
            add_inode_to_free_list(i);
        }
    }
    else
    {
        printf("Root cannot be deleted\n");
        v_ret->d = strdup("Root cannot be deleted\n");
        v_ret->r = 1;
        return v_ret;
    }

    root_dir = get_directory(saved_pos);
    v_ret->d = strdup("\n");
    v_ret->r = 1;
    return v_ret;
}

V_RETURN *touch(char *path)
{
    char *p = strdup(path);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));
    v_ret->r = 1;

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
            v_ret->d = strdup("Directory alread exists\n");
            root_dir = get_directory(saved_pos);
            return v_ret;
        }

        INODE *inode = alloc_inode(REGULAR);
        printf("%d %d %s %d\n", root->inode, i, cur, inode->filetype);
        add_entry_to_parent(root->inode, cur, inode->inode_number);
    }
    else
    {
        printf("Incorrect path\n");
        v_ret->d = strdup("Incorrect path\n");
    }
    
    root_dir = get_directory(saved_pos);
    if (v_ret->d == NULL)
        v_ret->d = strdup("\n");

    return v_ret;
}

void nano(char *path)
{
    char buffer[BLOCK_SIZE];
    int offset = 0;
    INODE *inode;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));
    v_ret->r = 1;

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
                printf("1File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
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

    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
    while (1)
    {
        memset(buffer, 0, BLOCK_SIZE);
        int r = read(1, buffer, BLOCK_SIZE);
        if (strcmp(buffer, "\n") == 0)
            break;

        write_buffer(inode, buffer, r, offset);
        offset += r;
    }

    write_inode(inode);
    root_dir = get_directory(saved_pos);

    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
}

V_RETURN *cat_new(char *path)
{
    char *buffer, *b=NULL;
    int offset = 0, len=0;
    INODE *inode;
    V_RETURN *v_ret = calloc(1, sizeof(V_RETURN));
    v_ret->r = 1;

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
                printf("1File do not exist...\n");
                v_ret->d = strdup("1File do not exist...\n");
                return v_ret;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
        if (i == -1)
        {
            printf("File do not exist...\n");
            v_ret->d = strdup("File do not exist...\n");
            return v_ret;
        }
        
        inode = get_inode(i);
    }
    else
    {
        printf("Invalid path -- root\n");
        v_ret->d = strdup("Invalid path -- root\n");
        return v_ret;
    }

    offset = inode->size;
    int off = 0;
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
    while (1)
    {
        if (offset <= 0)
            break;

        buffer = read_buffer(inode, BLOCK_SIZE, off);
        len = len + strlen(buffer);
        b = realloc(b, len+1);
        strcat(b, buffer);
        write(1, buffer, BLOCK_SIZE);
        offset -= BLOCK_SIZE;
        off += BLOCK_SIZE;
    }

    root_dir = get_directory(saved_pos);

    printf("\n");
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
    v_ret->d = b;
    return v_ret;
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
                printf("1File do not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }


    int i = 0;
    if (cur != NULL)
    {
        i = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, i);
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
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
    while (1)
    {
        if (offset <= 0)
            break;

        buffer = read_buffer(inode, BLOCK_SIZE, off);
        write(1, buffer, BLOCK_SIZE);
        offset -= BLOCK_SIZE;
        off += BLOCK_SIZE;
    }

    root_dir = get_directory(saved_pos);

    printf("\n");
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
}
