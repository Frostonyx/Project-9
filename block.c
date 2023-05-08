#include <unistd.h>
#include "block.h"
#include "image.h"


unsigned char *bread(int block_num, unsigned char *block) {
    int offset = block_num * 4096;
    lseek(image_fd, offset, SEEK_SET);
    read(image_fd, block, 4096);
    return block;
}


void bwrite(int block_num, unsigned char *block) {
    int offset = block_num * 4096;
    lseek(image_fd, offset, SEEK_SET);
    write(image_fd, block, 4096);
}

int alloc(void) {
    unsigned char block_map[4096];
    bread(2, block_map);
    int block_num = find_free(block_map);
    if (block_num == -1) {
        return -1;
    }

    set_free(block_map, block_num, 1);
    bwrite(2, block_map);

    return block_num + 7;
}