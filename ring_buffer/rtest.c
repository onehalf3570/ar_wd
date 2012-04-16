#include <stdio.h>
#include "ring_buffer.h"


int main (void)
{
  struct buf_info test_buffer;
  unsigned char real_buf[20];
  rb_init (&test_buffer, (unsigned char*)&real_buf, 20);

  rb_write (&test_buffer, 'a');
  rb_write (&test_buffer, 'a');

  printf ("got %hhc %hhc\n", rb_read(&test_buffer), rb_read(&test_buffer));


  return 0;
}
