CC=avr-gcc
OBJCOPY=avr-objcopy

default: ar_wd.hex

ar_wd.o: ar_wd.c
	$(CC) -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o $@ $<

ar_wd: ar_wd.o
	$(CC) -mmcu=atmega328p $< -o $@

ar_wd.hex: ar_wd
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: ar_wd.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$<

clean:
	rm -rf ar_wd.o ar_wd.hex ar_wd
