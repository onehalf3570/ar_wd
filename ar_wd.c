#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "ring_buffer/ring_buffer.h"

#define DEFAULT_COUNTER_VALUE 10

#define PING_LED PORTB1
#define WARNING_LED PORTB4
#define RELAY PORTB2
#define BUZZER PORTB3
#define HEARTBEAT_LED PORTB5 //yellow arduino led

#define DEFAULT_FREQ_2400 104 //2400 Hz from 16Mhz input with prescaler = 64
#define DEFAULT_FREQ_3000 83

#define UART_FIFO_SIZE 64

//FIXME: this should be calculated from F_CPU
#define FREQ_1S 15624 //1 second for 16MHz and prescaler=1024

#define PING_CMD "ping"
#define SHOW_CMD "show"
#define SET_CMD "set"

volatile int counter=DEFAULT_COUNTER_VALUE; //current counter, decreases every second in COMP_A ISR
int max_counter_value=DEFAULT_COUNTER_VALUE; //user-adjustable maximum counter value

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

unsigned char uart_recv_buffer [UART_FIFO_SIZE+1];

struct buf_info uart_rx;

int main (void)
{
  wdt_enable(WDTO_2S);

  uart_init();
  stdout=&uart_output;
  stdin=&uart_input;

  //command prompt
  putchar ('>');

  rb_init (&uart_rx, uart_recv_buffer, UART_FIFO_SIZE+1);

  //enable uart interrupts
  UCSR0B |= _BV(RXCIE0);

  /* set pins of PORTB for output*/
  DDRB |= _BV(DDB5) | _BV(DDB1) | _BV (DDB4) | _BV(DDB2) | _BV (DDB3);

  //enable timer for counting seconds
  TCCR1B = _BV (WGM12); //CTC enable
  TCCR1B |= _BV(CS10) | _BV(CS12); //prescaler=1024
 
  TIMSK1 |= _BV (OCIE1A); //enable interrupt

  OCR1A = FREQ_1S;

  sei(); //enable interrupts

  for (;;)
  {
    wdt_reset(); 
    sleep_mode();
  }

  return 0;
}

ISR (TIMER1_COMPA_vect)
{
  PORTB ^= _BV(HEARTBEAT_LED);
 
  counter--;
  if (counter == 0)
  { 
    //light up warning led
    PORTB |= _BV(WARNING_LED);

    //do reset
    PORTB |= _BV(RELAY);

    //enable timer for buzzer
    TCCR2A = _BV (WGM21); //CTC mode

    TCCR2A |= _BV (COM2A0); //toggle OC2A on compare match
    
    TCCR2B = _BV (CS22); //prescaler = 64

    OCR2A = DEFAULT_FREQ_2400;

    //disable all interrupts
    //cli();

    _delay_ms (500);
    OCR2A = DEFAULT_FREQ_3000;
    _delay_ms (500);

    //enable interrupts
    //sei();

    //reset is done
    PORTB &= ~_BV(WARNING_LED);
    PORTB &= ~_BV(RELAY);

    //disable buzzer
    TCCR2B = 0;
    TCCR2A = 0;

    //reset counter
    counter = DEFAULT_COUNTER_VALUE; //reset to default value to allow PC to boot successfully
  }
}

ISR (USART_RX_vect)
{
  char recv = UDR0;
  //echo
  putchar(recv);

  rb_write(&uart_rx, recv);

  if (recv == '\r')
  {
    putchar('\n');

    //process command
    char cmd [UART_FIFO_SIZE+1];
    int npos = 0;
    int chr = rb_read (&uart_rx);
    while (chr != EEMPTY)
    {
      cmd[npos++]=(char) chr;
      chr = rb_read(&uart_rx);
    }

    //strip \r\n at the end
    while (cmd[npos-1] == '\r' || cmd [npos-1] == '\n')
    {
      cmd[--npos]=0;
    }

    printf ("npos=%d\r\n", npos);
    if (npos > 0)
    {
      if (strncmp (cmd, PING_CMD,npos) == 0)
      {
        PORTB |= _BV (PING_LED);
        counter=max_counter_value;
        printf ("resetting timer to %d\r\n", max_counter_value);
        PORTB &= ~_BV(PING_LED);
      } else if (strncmp (cmd, SHOW_CMD, npos) == 0)
      {
        printf ("max_counter_value is %d\r\n", max_counter_value);
      } else if ((npos >= sizeof(SET_CMD)-1) && (strncmp (cmd, SET_CMD, sizeof (SET_CMD)-1) == 0))
      {
        int new_cval = (int)strtol (cmd+sizeof(SET_CMD)-1, NULL, 10);
        new_cval = new_cval ? new_cval : 1;
        printf ("stub: setting max_counter_value to %d\r\n", new_cval);
        max_counter_value=new_cval;
      } else
      {
        printf ("wrong command: '%s' (size=%d)\r\n",cmd,npos);
      }
    }

    putchar ('>');
  }
}
