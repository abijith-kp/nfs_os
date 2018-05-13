OPTIONS=-g -Wall

nfs: main.c superblock.c superblock.h inode.h inode.c directories.c avl.c avl.h fs.c fs.h directories.h
	gcc ${OPTIONS} main.c superblock.c inode.c directories.c avl.c fs.c -o nfs

test: main.c superblock.c superblock.h inode.h inode.c directories.c avl.c avl.h directories.h fs.c fs.h
	gcc ${OPTIONS} -DTEST main.c fs.c superblock.c inode.c directories.c avl.c -o nfs
	rm -rf network.fs
	./nfs

clean:
	rm -rf main.o superblock.o inode.o directories.o avl.o nfs network.fs fs.o
