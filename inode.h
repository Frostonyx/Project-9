#ifndef INODE_H
#define INODE_H

struct inode *iget(int inode_num);
void iput(struct inode *in);
struct inode *ialloc(void);

#endif
