#include <unistd.h>
#include "image.h"
#include "block.h"
#include "free.h"
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
}
