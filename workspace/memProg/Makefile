CC=avr-gcc
OBJCOPY=avr-objcopy
AVRSIZE=avr-size
AVRDUDE=avrdude
MCU=atmega8535
CFLAGS= -Wall -mmcu=$(MCU) -std=gnu99 -Os
INC=-Iinc
FORMAT=ihex

all: clean MemProg.elf MemProg.hex size

MemProg.elf: utils.o main.o
	$(CC) -o "$@" utils.o main.o

main.o: src\main.c
	$(CC) $(CFLAGS) $(INC) -c src\main.c

utils.o: src\utils.c
	$(CC) $(CFLAGS) $(INC) -c src\utils.c

MemProg.hex:
	$(OBJCOPY) -v -O $(FORMAT) MemProg.elf "$@"

size:
	$(AVRSIZE) MemProg.hex

load:
	$(AVRDUDE) -cusbasp -pm8535 -e -U flash:w:MemProg.hex:i

clean:
	cd src
	rm -r main.o utils.o MemProg.elf MemProg.hex 
