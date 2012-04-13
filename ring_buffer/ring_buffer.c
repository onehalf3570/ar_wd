#include "ring_buffer.h"

void init (struct buf_info * bi, unsigned char * buf_ptr, int size)
{
  bi->buf = buf_ptr;
  bi->read_ptr=0;
  bi->write_ptr=0;
  bi->size=size;
}

int write (struct buf_info * bi, unsigned char c)
{
  bi->buf[bi->write_ptr] = c;
  bi->write_ptr = (bi->write_ptr + 1) % bi->size;
  if (bi->write_ptr == bi->read_ptr)
  {
    bi->read_ptr = (bi->read_ptr + 1) % bi->size;
  }
}

int read (struct buf_info * buf)
{
  if ((bi->write_ptr + 1) % bi->size == bi->read_ptr)
  {
    return EEMPTY;
  } else
  {

  }
}
