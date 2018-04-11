OPTIONS=`pkg-config --cflags --libs glib-2.0` -g -Wall

nfs: main.c superblock.c superblock.h inode.h inode.c directories.c directories.h
	gcc ${OPTIONS} main.c superblock.c inode.c directories.c -o nfs

clean:
	rm -rf main.o superblock.o inode.o directories.o nfs
