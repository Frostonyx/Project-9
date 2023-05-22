#include <stdio.h>
#include "inode.h"
#include "free.h"
#include "block.h"
#include "pack.h"

#define INODE_PTR_COUNT 16
#define MAX_SYS_OPEN_FILES 64
#define BLOCK_SIZE 4096
static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;
    unsigned int inode_num;
};

struct inode *ialloc(void) {
    int inode_num = find_free(); 

    if (inode_num == -1) {
        return NULL;
    }

    struct inode *in = iget(inode_num);

    if (in == NULL) {
        return NULL;
    }

    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    in->link_count = 0;
    int i;
    for (i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }
    in->inode_num = inode_num;

    write_inode(in);

    return in;
}

struct inode *find_incore_free(void) {
    int i;
    for (i = 0; i < incore; i++) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num) {
    int i;
    for (i = 0; i < incore; i++) {
        if (incore[i].ref_count > 0 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num) {
    unsigned char block[BLOCK_SIZE];
    int block_num = 3 + inode_num / 4; 
    int offset = (inode_num % 4) * sizeof(struct inode);

    bread(block_num, block);
    unsigned char *inode_data = block + offset;

    in->size = read_u32(inode_data);
    in->owner_id = read_u16(inode_data + 4);
    in->permissions = read_u8(inode_data + 6);
    in->flags = read_u8(inode_data + 7);
    in->link_count = read_u8(inode_data + 8);
    int i;
    for (i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(inode_data + 9 + i * 2);
    }
}

void write_inode(struct inode *in) {
    unsigned char block[BLOCK_SIZE];
    int block_num = 3 + in->inode_num / 4;
    int offset = (in->inode_num % 4) * sizeof(struct inode); 

    bread(block_num, block);
    unsigned char *inode_data = block + offset;

    write_u32(inode_data, in->size);
    write_u16(inode_data + 4, in->owner_id);
    write_u8(inode_data + 6, in->permissions);
    write_u8(inode_data + 7, in->flags);
    write_u8(inode_data + 8, in->link_count);
    int i;
    for (i = 0; i < INODE_PTR_COUNT; i++) {
        write_u16(inode_data + 9 + i * 2, in->block_ptr[i]);
    }

    bwrite(block_num, block);
}

struct inode *iget(int inode_num) {
    struct inode *in = find_incore(inode_num);

    if (in != NULL) {
        in->ref_count++;
        return in;
    }

    in = find_incore_free();

    if (in == NULL) {
        return NULL;
    }

    read_inode(in, inode_num);
    in->ref_count = 1; 
    in->inode_num = inode_num;

    return in;
}

void iput(struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }

    in->ref_count--; 

    if (in->ref_count == 0) {
        write_inode(in);
    }
}