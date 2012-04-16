#include <avr/io.h>
#include <stdio.h>

#define BAUD_TOL 3
#ifndef BAUD
#define BAUD 115200
#endif
#include <util/setbaud.h>

extern struct buf_info uart_rx, uart_tx;

void uart_init(void)
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
    
#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8bit
  UCSR0B = _BV(RXEN0) | _BV(TXEN0); // RX + TX Enable
}

void uart_putchar(char c, FILE *stream)
{
  if (c == '\n')
  {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  //rb_write(&uart_tx, c);
}

char uart_getchar(FILE *stream) 
{
  //should not be called
  return (char)NULL;
}
