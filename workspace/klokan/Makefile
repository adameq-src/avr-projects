CC=avr-gcc
OBJCOPY=avr-objcopy
AVRSIZE=avr-size
AVRDUDE=avrdude
MCU=attiny45
CFLAGS= -Wall -mmcu=$(MCU) -std=gnu99 -Os
INC=-Iinc
FORMAT=ihex

all: clean klokan.elf klokan.hex size

klokan.elf: main.o
	$(CC) -o "$@" main.o

main.o: src\main.c
	$(CC) $(CFLAGS) $(INC) -c src\main.c

utils.o: src\utils.c
	$(CC) $(CFLAGS) $(INC) -c src\utils.c

klokan.hex:
	$(OBJCOPY) -v -O $(FORMAT) klokan.elf "$@"

size:
	$(AVRSIZE) klokan.hex

load:
	$(AVRDUDE) -cusbasp -pt45 -e -U flash:w:klokan.hex:i

clean:
	cd src
	rm -r main.o klokan.elf klokan.hex 
