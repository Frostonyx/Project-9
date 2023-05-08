#include "image.h"

int image_fd = -1;

int image_open(char *filename, int truncate) {
    int flags = O_RDWR | O_CREAT;
    if (truncate) {
        flags |= O_TRUNC;
    }
    int permissions = 0600;
    image_fd = open(filename, flags, permissions);
    return image_fd;
}

int image_close(void) {
    int result = close(image_fd);
    image_fd = -1;
    return result;
}
