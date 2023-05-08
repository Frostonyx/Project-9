#include "free.h"
#include "block.h"

int find_low_clear_bit(unsigned char x) {
    for (int i = 0; i < 8; i++) {
        if (!(x & (1 << i))) {
            return i;
        }
    }
    return -1;
}

void set_free(unsigned char *block, int num, int set) {
    int byte_num = num / 8;
    int bit_num = num % 8;
    unsigned char mask = 1 << bit_num;
    if (set) {
        block[byte_num] |= mask;
    } else {
        block[byte_num] &= ~mask;
    }
}

int find_free(unsigned char *block) {
    for (int i = 0; i < 4096; i++) {
        if (block[i] != 0xff) {
            int bit_num = find_low_clear_bit(~block[i]);
            if (bit_num != -1) {
                return i * 8 + bit_num;
            }
        }
    }
    return -1;
}
