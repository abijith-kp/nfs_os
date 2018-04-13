OPTIONS=-g -Wall

nfs: main.c superblock.c superblock.h inode.h inode.c directories.c avl.c avl.h directories.h
	gcc ${OPTIONS} main.c superblock.c inode.c directories.c avl.c -o nfs

clean:
	rm -rf main.o superblock.o inode.o directories.o avl.o nfs
