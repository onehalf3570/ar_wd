#ifndef RING_BUFFER_H
#define RING_BUFFER_H
struct buf_info
{
  unsigned char * buf;
  unsigned int read_ptr, write_ptr;
  unsigned int size;
};

#define EEMPTY -1

void init (struct buf_info*, unsigned char *, int);
int write (struct buf_info*, unsigned char);
int read (struct buf_info*);
#endif
