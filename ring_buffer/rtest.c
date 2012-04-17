#include <stdio.h>
#include "ring_buffer.h"


int main (void)
{
  int i=0;
  struct buf_info test_buffer;
  unsigned char real_buf[20];
  rb_init (&test_buffer, (unsigned char*)&real_buf, 20);

  rb_write (&test_buffer, 'a');
  rb_write (&test_buffer, 'a');

  printf ("got %hhc %hhc\n", rb_read(&test_buffer), rb_read(&test_buffer));

  for (i = 0; i<52; i++)
  {
    rb_write (&test_buffer, (char)(i+33));
  }

  int chr = rb_read (&test_buffer);
  char str [21];
  int npos = 0;
  while (chr != EEMPTY)
  {
    str[npos++]=(char)chr;
    chr = rb_read (&test_buffer);
  }
  str[npos]=0;

  printf ("npos=%d, got %s\n", npos, str);

  return 0;
}
