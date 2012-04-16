#include <stdio.h>
#include "ring_buffer.h"


int main (void)
{
  struct buf_info test_buffer;
  unsigned char real_buf[20];
  init (&test_buffer, (unsigned char*)&real_buf, 20);

  write (&test_buffer, 'a');
  write (&test_buffer, 'a');

  printf ("got %hhc %hhc\n", read(&test_buffer), read(&test_buffer));


  return 0;
}
