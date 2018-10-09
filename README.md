NFS-OS
------

Course Project for Operating System.

ls [pathname]
    Output: filename inode_number filetype

cd pathname

rm pathname

mv pathname foldername

touch pathname
    Creates a regular file

nano pathname
    Enables to write into a regular file.
    Overwrites the previous content.

cat pathname
    Enables to print content of a file

How to start:
    Client:
    ./client <ip> <port_number>

    Server:
    ./server <ip> <port_number>


Compilation:

Client:
    gcc client.c -o client


Bugs fixed:
    * while moving a file if an error occurs the source file gets removed
    
Bugs to reproduce and fix:
    * Somehow the filesystem metadata gets corrupted
