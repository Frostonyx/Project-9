#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "inode.h"
#include "free.h"
#include "pack.h"
#include "mkfs.h"

void mkfs(void) {
    unsigned char block[4096];
    int i;

    memset(block, 0, 4096);
    for (i = 0; i < 1024; i++) {
        write(image_fd, block, 4096);
    }

    for (i = 0; i < 7; i++) {
        int block_num = alloc();
    }

    struct inode *root_inode = ialloc();
    if (root_inode == NULL) {
        fprintf(stderr, "Failed to allocate inode for root directory.\n");
        return;
    }

    int data_block_num = alloc();
    if (data_block_num == -1) {
        fprintf(stderr, "Failed to allocate data block for root directory.\n");
        return;
    }
        // Initialize the root inode
    root_inode->flags = 2;
    root_inode->size = 64;
    root_inode->block_ptr[0] = data_block_num;

    // Create the . and .. directory entries
    unsigned char dir_block[BLOCK_SIZE];
    memset(dir_block, 0, BLOCK_SIZE);
    struct directory_entry *dir_entry;

    // . entry
    dir_entry = (struct directory_entry *) (dir_block);
    write_u16(&(dir_entry->inode_num), root_inode->inode_num);
    strcpy(dir_entry->name, ".");
    
    // .. entry
    dir_entry = (struct directory_entry *) (dir_block + sizeof(struct directory_entry));
    write_u16(&(dir_entry->inode_num), root_inode->inode_num);
    strcpy(dir_entry->name, "..");

    bwrite(data_block_num, dir_block);

    write_inode(root_inode);
    iput(root_inode);
}

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

struct directory *directory_open(int inode_num) {
    struct inode *inode = iget(inode_num);
    if (inode == NULL) {
        return NULL;
    }

    struct directory *dir = malloc(sizeof(struct directory));
    if (dir == NULL) {
        iput(inode);
        return NULL;
    }

    dir->inode = inode;
    dir->offset = 0;

    return dir;

}

int directory_get(struct directory *dir, struct directory_entry *ent) {
    unsigned int offset = dir->offset;
    unsigned int directory_size = dir->inode->size;

    if (offset >= directory_size) {
        return -1;
    }

    unsigned int data_block_index = offset / BLOCK_SIZE;
    unsigned int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    unsigned int offset_in_block = offset % BLOCK_SIZE;
    struct directory_entry *dir_entry = (struct directory_entry *) (block + offset_in_block);

    ent->inode_num = read_u16(&(dir_entry->inode_num));
    strcpy(ent->name, dir_entry->name);

    dir->offset += sizeof(struct directory_entry);

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}