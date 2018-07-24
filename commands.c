#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
        free(inode);
    }
    else
    {
        printf("Incorrect path\n");
    }
    
    free(root_dir);
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

    if (root_dir)
         free(root_dir);
    
    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(saved_pos);
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
    free(root_dir);
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

    if (root_dir)
        free(root_dir);

    if (path[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(saved_pos);
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

    free(root_dir);
    root_dir = get_directory(saved_pos);
}

void copy(char* source, char* destination)
{
    // Traverse to the source file and get its inode
    char *p = strdup(source);
    char *tmp = strtok(p, "/");
    char *cur = tmp, *prev = NULL;
    int saved_pos = root_dir->inode;

    if (root_dir)
        free(root_dir);
    
    if (source[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(saved_pos);
    S_DIRECTORY *root = root_dir;

    printf("copy: %s %s %s [%s]\n", cur, prev, tmp, source);
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

    printf("copy: %s %s %s\n", cur, prev, tmp);

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

    int source_path_inode = root->inode;

    root_dir = get_directory(saved_pos);

    // Traverse to the destination directory
    p = strdup(destination);
    tmp = strtok(p, "/");
    cur = tmp, prev = NULL;
    saved_pos = root_dir->inode;

    if (destination[0] == '/')
        root_dir = get_directory(ROOT_INODE);
    else
        root_dir = get_directory(saved_pos);
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
        if (i != -1)
        {
            printf("Incorrect destination. File already exists\n");
            root_dir = get_directory(saved_pos);
            return;
        }

        /*
        root = get_directory(i);

        // Check if file already exists in the destination
        int j;
        for (j = 0; j < root->count; j++) {
            if (strcmp((root->dir_entry[j]).filename, src_file_name) == 0 && (root->dir_entry[j]).inode_number != 0) {
                printf("File already exists in destination\n");
                return;
            }
        }
        */

        // Create a new inode in the destination directory
        INODE *dest_inode  = alloc_inode(REGULAR);
        printf("%d %d %s %d %d\n", root->inode, i, src_file_name, dest_inode->inode_number, source_path_inode);
        add_entry_to_parent(root->inode, cur, dest_inode->inode_number); 

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

void move(char* source, char* destination)
{
    copy(source, destination);
    removedir(source);
}
