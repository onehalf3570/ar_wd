#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define DEFAULT_COUNTER_VALUE 10
#define PING_LED PB1
#define WARNING_LED PB4
#define RELAY PB2
#define BUZZER PB3

volatile int counter=DEFAULT_COUNTER_VALUE; //current counter, decreases every second in COMP_A ISR
int max_counter_value=DEFAULT_COUNTER_VALUE; //user-adjustable maximum counter value

int main (void)
{
  wdt_enable(WDTO_2S);

  /* set pins of PORTB for output*/
  DDRB |= _BV(DDB5) | _BV(DDB1) | _BV (DDB4) | _BV(DDB2);
  TCCR1B = _BV (WGM12); //CTC enable
  TCCR1B |= _BV(CS10) | _BV(CS12); //prescaler=1024
 
  TIMSK1 |= _BV (OCIE1A); //enable interrupt

  sei(); //enable interrupts

  //FIXME: this should be calculated from F_CPU
  OCR1A = 15624; //1 second for 16MHz and prescaler=1024

  for (;;)
  {
    wdt_reset(); 
    sleep_mode();
  }

  return 0;
}

ISR (TIMER1_COMPA_vect)
{
  PORTB ^= _BV(PORTB1) | _BV(PORTB4);
  counter--;
  if (counter == 1)
  { 
    //warn about upcoming reset
    PORTB |= _BV(PORTB5);
    PORTB |= _BV(PORTB2);
  }
  if (!counter)
  {
    //perform reset
    PORTB &= ~_BV(PORTB5);
    PORTB &= ~_BV(PORTB2);
    //PORTB ^= _BV(PORTB5);
    counter = DEFAULT_COUNTER_VALUE; //reset to default value to allow PC to boot successfully
  }
}
