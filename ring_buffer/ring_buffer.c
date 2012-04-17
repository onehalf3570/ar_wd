#include <stdio.h>
#include "ring_buffer.h"

//read_ptr == write_ptr : buffer is empty
//read_ptr == write_ptr+1 : buffer is full

void rb_init (struct buf_info * bi, unsigned char * buf_ptr, int size)
{
  bi->buf = buf_ptr;
  bi->read_ptr=0;
  bi->write_ptr=0;
  bi->size=size;
}

void rb_write (struct buf_info * bi, unsigned char c)
{
  //printf ("writing %c\n", c);
  bi->buf[bi->write_ptr] = c;
  bi->write_ptr = (bi->write_ptr + 1) % bi->size;
  if (bi->write_ptr == bi->read_ptr)
  {
    bi->read_ptr = (bi->read_ptr + 1) % bi->size;
  }
  //printf ("write_ptr=%d, read_ptr=%d\n", bi->write_ptr, bi->read_ptr);
}

int rb_read (struct buf_info * bi)
{
  if (bi->write_ptr == bi->read_ptr)
  {
    return EEMPTY;
  } else
  {
    char ret = bi->buf[bi->read_ptr];
    bi->read_ptr = (bi->read_ptr + 1) % bi->size;
    //printf ("reading %c\n", (char)ret);
    return ret;
  }
}
