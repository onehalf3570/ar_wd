#ifndef RING_BUFFER_H
#define RING_BUFFER_H
struct buf_info
{
  unsigned char * buf;
  unsigned int read_ptr, write_ptr;
  unsigned int size;
};

#define EEMPTY -1

void rb_init (struct buf_info*, unsigned char *, int);
int rb_write (struct buf_info*, unsigned char);
int rb_read (struct buf_info*);
#endif
