#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdio.h>

#include "uart.h"

#define DEFAULT_COUNTER_VALUE 10

#define PING_LED PORTB1
#define WARNING_LED PORTB4
#define RELAY PORTB2
#define BUZZER PORTB3
#define HEARTBEAT_LED PORTB5 //yellow arduino led

volatile int counter=DEFAULT_COUNTER_VALUE; //current counter, decreases every second in COMP_A ISR
int max_counter_value=DEFAULT_COUNTER_VALUE; //user-adjustable maximum counter value

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

int main (void)
{
  wdt_enable(WDTO_2S);

  uart_init();
  stdout=&uart_output;
  stdin=&uart_input;

  /* set pins of PORTB for output*/
  DDRB |= _BV(DDB5) | _BV(DDB1) | _BV (DDB4) | _BV(DDB2) | _BV (DDB3);

  //enable timer for counting seconds
  TCCR1B = _BV (WGM12); //CTC enable
  TCCR1B |= _BV(CS10) | _BV(CS12); //prescaler=1024
 
  TIMSK1 |= _BV (OCIE1A); //enable interrupt

  //FIXME: this should be calculated from F_CPU
  OCR1A = 15624; //1 second for 16MHz and prescaler=1024


  sei(); //enable interrupts

  for (;;)
  {
    if bit_is_set(UCSR0A, RXC0)
    {
      char recv = UDR0;
      if (recv == 'r')
      {
        PORTB |= _BV (PING_LED);
        counter=max_counter_value;
        puts ("resetting timer\n");
        PORTB &= ~_BV(PING_LED);
      } else
      {
        puts ("wrong command\n");
      }
    }
    wdt_reset(); 
    sleep_mode();
  }

  return 0;
}

ISR (TIMER1_COMPA_vect)
{
  PORTB ^= _BV(HEARTBEAT_LED);
 
  counter--;
  if (counter == 1)
  { 
    //warn about reset
    PORTB |= _BV(WARNING_LED);

    //do reset
    PORTB |= _BV(RELAY);

    //enable timer for buzzer
    TCCR2A = _BV (WGM21); //CTC mode
    TCCR2A |= _BV (COM2A0); //toggle OC2A on compare match
    
    TCCR2B = _BV (CS22); //prescaler = 64

    OCR2A = 104; //2400 Hz with prescaler = 64
  }
  if (!counter)
  {
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
