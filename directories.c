#include <stdio.h>
#include <string.h>
#include <block.h>
#include <inode.h>
// Inspired by the `dirname` and `basename` Unix utilities
//
// But doesn't handle a trailing '/' on the path correctly.
//
// So don't do that.
char *get_dirname(const char *path, char *dirname)
{
strcpy(dirname, path);
char *p = strrchr(dirname, '/');
if (p == NULL) {
strcpy(dirname, ".");
return dirname;
}
if (p == dirname) // Last slash is the root /
*(p+1) = '\0';
else
*p = '\0'; // Last slash is not the root /
return dirname;
}
char *get_basename(const char *path, char *basename)
{
if (strcmp(path, "/") == 0) {
strcpy(basename, path);
return basename;
}
const char *p = strrchr(path, '/');
if (p == NULL)
p = path; // No slash in name, start at beginning
else
p++; // Start just after slash
strcpy(basename, p);
return basename;
}
int main(void)
{
char result[1024];
puts(get_dirname("/foo/bar/baz", result)); // /foo/bar
puts(get_dirname("/foo/bar", result)); // /foo
puts(get_dirname("/foo", result)); // /
puts(get_dirname("/", result)); // /
puts(get_dirname("foo", result)); // .
puts(get_dirname("", result)); // .
puts(get_basename("/foo/bar/baz", result)); // baz
puts(get_basename("/foo/bar", result)); // bar
puts(get_basename("/foo", result)); // foo
puts(get_basename("/", result)); // /
puts(get_basename("foo", result)); // foo
puts(get_basename("", result)); //
}


struct inode *namei(char *path) {

    if (strcmp(path, "/") == 0) {

        return iget(ROOT_INODE_NUM);
    }


    return NULL;
}


int directory_make(char *path) {
    char dirname[1024];
    char basename[256];


    get_dirname(path, dirname);


    get_basename(path, basename);

    struct inode *parent_inode = namei(dirname);
    if (parent_inode == NULL) {

        return -1;
    }


    if (!(parent_inode->flags & DIRECTORY)) {

        iput(parent_inode);
        return -1;
    }


    struct inode *new_inode = ialloc();
    if (new_inode == NULL) {

        iput(parent_inode);
        return -1;
    }


    int new_block_num = alloc();
    if (new_block_num == -1) {

        iput(new_inode);
        iput(parent_inode);
        return -1;
    }

    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);


    struct directory_entry *dot_entry = (struct directory_entry *)block;
    dot_entry->inode_num = new_inode->inode_num;
    strcpy(dot_entry->name, ".");

    struct directory_entry *dotdot_entry = (struct directory_entry *)(block + 32);
    dotdot_entry->inode_num = parent_inode->inode_num;
    strcpy(dotdot_entry->name, "..");

    bwrite(new_block_num, block);


    new_inode->flags = DIRECTORY;
    new_inode->size = 64;
    new_inode->block_ptr[0] = new_block_num;


    write_inode(new_inode);

    int parent_block_num = parent_inode->block_ptr[(parent_inode->size - 1) / BLOCK_SIZE];
    unsigned char parent_block[BLOCK_SIZE];
    bread(parent_block_num, parent_block);


    struct directory_entry *new_entry = (struct directory_entry *)(parent_block + parent_inode->size);
    new_entry->inode_num = new_inode->inode_num;
    strcpy(new_entry->name, basename);


    bwrite(parent_block_num, parent_block);


    parent_inode->size += 32;


    write_inode(parent_inode);


    iput(new_inode);
    iput(parent_inode);

    return 0;
}
