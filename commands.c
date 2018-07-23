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
                printf("1File does not exist...\n");
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
            printf("File does not exist...\n");
            root_dir = get_directory(saved_pos);
            return;
        }
        
        INODE *in = get_inode(i);
        if (in->filetype != DIRECTORY)
        {
            printf("=========\n");
            printf("name: %s\n", cur);
            printf("inode: %d\n", i);
            printf("=========\n");
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
                printf("1File does not exist...\n");
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
            printf("File does not exist...\n");
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
                printf("1File does not exist...\n");
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
            printf("File does not exist...\n");
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
            printf("Directory already exists\n");
            root_dir = get_directory(saved_pos);
            return;
        }

        INODE *inode = alloc_inode(REGULAR);
        printf("%d %d %s %d\n", root->inode, i, cur, inode->filetype);
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

    /* get inode from looping */

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
                printf("1File does not exist...\n");
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
            printf("File does not exist...\n");
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
        read(1, buffer, BLOCK_SIZE);
        if (strcmp(buffer, "\n") == 0)
            break;

        write_buffer(inode, buffer, BLOCK_SIZE, offset);
        offset += BLOCK_SIZE;
    }

    write_inode(inode);
    root_dir = get_directory(saved_pos);

    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
}

void cat(char *path)
{
    char *buffer;
    int offset = 0;
    INODE *inode;

    /* get inode from looping */

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
                printf("File does not exist...\n");
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
            printf("File does not exist...\n");
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
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
    while (1)
    {
        if (offset <= 0)
            break;

        buffer = read_buffer(inode, BLOCK_SIZE, offset);
        write(1, buffer, BLOCK_SIZE);
        offset -= BLOCK_SIZE;
    }

    root_dir = get_directory(saved_pos);

    printf("\n");
    printf("nano inode: %d %d %d\n", inode->inode_number, inode->entries[0], inode->size);
}

void copy(char* source, char* destination) {
    // Traverse to the source file and get its inode
    char *p = strdup(source);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (source[0] == '/')
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
                printf("File does not exist...\n");
                return;
            }
            root = get_directory(i);
        }
    }

    int src_file_inode;
    INODE* src_file;
    char *src_file_name;

    if (cur != NULL)
    {
        src_file_inode = get_inode_dir(root, cur);
        printf("[[%s %s %d]]\n", root->name, cur, src_file_inode);
        if (src_file_inode == -1)
        {
            printf("File does not exist...\n");
            return;
        }
        
        // Get inode of the source file
        src_file = get_inode(src_file_inode);
        src_file_name = cur;

        if (src_file->filetype == DIRECTORY) {
            printf("cp: %s is a directory (not copied)\n", source);
            return;
        }
    }
    else
        printf("Invalid source path -- root\n");

    root_dir = get_directory(saved_pos);

    // Traverse to the destination directory
    p = strdup(destination);
    tmp = strtok(p, "/");
    cur = tmp, prev = NULL;
    saved_pos = root_dir->inode;

    if (destination[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(root_dir->inode);
    root = root_dir;

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
        if (i == -1)
        {
            printf("Incorrect destination path\n");
            root_dir = get_directory(saved_pos);
            return;
        }
        root = get_directory(i);

        // Check if file already exists in the destination
        int j;
        for (j = 0; j < root->count; j++) {
            if (strcmp((root->dir_entry[j]).filename, src_file_name) == 0 && (root->dir_entry[j]).inode_number != 0) {
                printf("File already exists in destination\n");
                return;
            }
        }

        // Create a new inode in the destination directory
        INODE *dest_inode  = alloc_inode(REGULAR);
        printf("%d %d %s %d\n", root->inode, i, src_file_name, dest_inode->filetype);
        add_entry_to_parent(root->inode, src_file_name, dest_inode->inode_number); 

        // Copy the contents of the source inode to destination inode 
        char* buffer = read_buffer(src_file, src_file->size, 0);
        write_buffer(dest_inode, buffer, src_file->size, 0);      
    }
    else
    {
        printf("Incorrect destination path\n");
    }
    
    root_dir = get_directory(saved_pos);
}

void move(char* source, char* destination) {
    copy(source, destination);
    removedir(source);
}
