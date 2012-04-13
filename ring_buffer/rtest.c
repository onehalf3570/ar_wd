#include <stdio.h>
#include "ring_buffer.h"


int main (void)
{
  struct buf_info test_buffer;
  unsigned char real_buf[20];
  init (&test_buffer, (unsigned char*)&real_buf, 20);



  return 0;
}
