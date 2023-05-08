#include "inode.h"
#include "free.h"
#include "block.h"

int ialloc(void) {
    unsigned char inode_map[4096];
    bread(1, inode_map);
    int inode_num = find_free(inode_map);
    if (inode_num != -1) {
        set_free(inode_map, inode_num, 1);
        bwrite(1, inode_map);
    }
    return inode_num;
}
