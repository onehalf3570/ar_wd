#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

int main (void)
{
  wdt_enable(WDTO_2S);

  /* set pin 5 of PORTB for output*/
  DDRB |= _BV(DDB5);
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
  /* set pin 5 high to turn led on */
  PORTB ^= _BV(PORTB5);
}
