CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p
LDFLAGS=-mmcu=atmega328p
OBJCOPY=avr-objcopy

default: ar_wd.hex

ar_wd.o: ar_wd.c

uart.o: uart.c

ar_wd: ar_wd.o uart.o

ar_wd.hex: ar_wd
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: ar_wd.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$<

clean:
	rm -rf uart.o ar_wd.o ar_wd.hex ar_wd
