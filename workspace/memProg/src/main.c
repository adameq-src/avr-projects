/*
Memory programmer using AVR atmega8535
Author: adameq
 */


#include <avr/io.h>
#include "stdlib.h"
#define F_CPU 8000000UL
#include <util/delay.h>
#include "utils.h"
#define BAUD 9600//38400
#define MYUBRR F_CPU/16/BAUD-1

#define 	sbi(port, bit)   (port) |= (1 << (bit))
#define 	cbi(port, bit)   (port) &= ~(1 << (bit))

#define		ADDRL		PORTA
#define		ADDRH		PORTB
#define		DATAIN		PINC
#define 	DATAOUT		PORTC
#define		DATADIR		DDRC
#define		IN			0x00
#define		OUT			0xFF
#define		CNTRL		PORTD

#define		RX			PD0
#define		TX			PD1
#define		LED			PD2
#define		CS_ROM		PD3//EEPROM,FLASH, etc.
#define 	OE			PD4
#define		WE			PD5
#define		CS_RAM		PD6//RAM
#define		na2			PD7


/*
	PORTA = addr high

	PORTB:
		PB0 = DIR_DATA
		PB1 = ALE
		PB2 = OE_DATA
		PB3 = OE_ADDR

	PORTC = data & addr low

	PORTD:
		PD4 = OE
		PD5 = WE 
		PD6 = CS_RAMAM
*/


void Init(void);
void WriteByte(uint8_t data, uint16_t address, uint8_t nCS);
uint8_t ReadByte(uint16_t address, uint8_t nCS);
void PreInit(void);
void PostInit(void);
void pause(uint8_t *msg);
void test(void);

void RAM_WriteBuffer(uint8_t *buffer, uint16_t startAddress, uint16_t size);
void RAM_ReadBuffer(uint8_t *buffer, uint16_t startAddress, uint16_t size);

void ROM_MassErase(void);
void ROM_WriteByte(uint8_t data, uint16_t address);
void ROM_WritePageBuffer(uint8_t *buffer, uint16_t startAddress, uint8_t size);


int main(void)
{
	uint8_t inp, addrl, addrh;
	uint8_t cmd;
	uint16_t i;

	Init();
	UsartInit(MYUBRR);


	println((uint8_t *) "ATM8535 BUS INTERFACE");

	while(1)
	{
		cmd = Rx();

		switch (cmd)
		{
			
		case 'w'://write byte
			//TxString((uint8_t *) "enter data byte: ");
			inp = Rx();
			NewLine();

			//TxString((uint8_t *) "enter ADDR low byte: ");
			addrl = Rx();
			NewLine();

			//TxString((uint8_t *) "enter ADDR high byte: ");
			addrh = Rx();
			NewLine();

			WriteByte(inp, (uint16_t) (((uint16_t)addrh << 8) | addrl), CS_RAM);
			break;
		case 'r'://read byte
			//TxString((uint8_t *) "enter ADDR low byte: ");
			addrl = Rx();
			NewLine();

			//TxString((uint8_t *) "enter ADDR high byte: ");
			addrh = Rx();
			NewLine();

			inp = ReadByte((uint16_t) (((uint16_t)addrh << 8) | addrl), CS_RAM);			
			Tx(inp);
			break;	
		case 'x':
			println((uint8_t *) "test R/W access");
			WriteByte('U', 0x00FA, CS_RAM);
			_delay_us(200);
			inp = ReadByte(0x00FA, CS_RAM);
			TxString((uint8_t *) "result: ");
			Tx(inp);
			NewLine();
			break;

		case 'q':
			test();
			break;
		case 'a':
			for(i = 0; i < 1024; i++) {
				Tx(ReadByte(i, CS_ROM));
				if((i%512 == 0) && (i != 0)) {
					pause((uint8_t *) "dalsi");
				}
			}

			//i = strlen("fg");
			break;
		case 'z':
			WriteByte(0xAA, 0x5555, CS_ROM);
			WriteByte(0x55, 0x2AAA, CS_ROM);
			WriteByte(0xA0, 0x5555, CS_ROM);

			for(i = 0; i < 40; i++)
			{
				WriteByte(0x30+i, i, CS_ROM);
			}
			_delay_us(330);
			break;
		case 'l':
			for(i = 0; i < 64; i++)
			{
				inp = ReadByte((uint16_t) i, CS_RAM);
				//TxString((uint8_t *) "result: ");
				Tx(inp);
				//Tx(' ');
			}

			NewLine();
			break;
		case 'L':
			for(i = 0x2000; i < 0x2000 + 64; i++)
			{
				inp = ReadByte((uint16_t) i, CS_RAM);
				//TxString((uint8_t *) "result: ");
				Tx(inp);
				//Tx(' ');
			}

			NewLine();
			break;

		default:
			println((uint8_t *) "Unknown command!");
			break;
		}

	}

    return 0;
}

void ROM_WritePageBuffer(uint8_t *buffer, uint16_t startAddress, uint8_t size)
{//page size is 128B, this function allows to write max 128B large buffer
	uint8_t i;

	if(((uint32_t) (startAddress + size) > 0xFFFF) || (size <= 128)) {
		println((uint8_t *) "ERROR: size too big!");
		return;
	}

	WriteByte(0xAA, 0x5555, CS_ROM);
	WriteByte(0x55, 0x2AAA, CS_ROM);
	WriteByte(0xA0, 0x5555, CS_ROM);

	for(i = 0; i < size; i++)
	{
		WriteByte(i, startAddress + i, CS_ROM);
	}

	_delay_us(330);
}

void ROM_WriteByte(uint8_t data, uint16_t address) 
{
	WriteByte(0xAA, 0x5555, CS_ROM);
	WriteByte(0x55, 0x2AAA, CS_ROM);
	WriteByte(0xA0, 0x5555, CS_ROM);
	
	WriteByte(data, address, CS_ROM);
	
	_delay_us(330);
}

void ROM_MassErase(void)
{
	TxString((uint8_t *) "erasing... ");
	
	WriteByte(0xAA, 0x5555, CS_ROM);
	WriteByte(0x55, 0x2AAA, CS_ROM);
	WriteByte(0x80, 0x5555, CS_ROM);

	WriteByte(0xAA, 0x5555, CS_ROM);
	WriteByte(0x55, 0x2AAA, CS_ROM);
	WriteByte(0x10, 0x5555, CS_ROM);

	_delay_ms(22);

	println((uint8_t *) "DONE");
}

void test(void)
{
	uint16_t i;
	uint8_t data, err = 0;

	TxString((uint8_t *) "testing ");

	for(i = 0; i < 0x7FFF; i++) {
		WriteByte(i & 0xFF, i, CS_RAM);
	}

	for(i = 0; i < 0x7FFF; i++) {
		data = ReadByte(i, CS_RAM);

		if(data != (i & 0xFF)) {
			Tx('e');
			err++;
		}

		if(i%1024 == 0) {
			Tx('.');
		}
	}

	for(i = 0; i < 0x7FFF; i++) {
		WriteByte(0x00, i, CS_RAM);
	}

	if(err == 0) {
		println((uint8_t *) " PASSED");
	} else {
		println((uint8_t *) " FAILED");
	}
}

void RAM_ReadBuffer(uint8_t *buffer, uint16_t startAddress, uint16_t size)
{
	uint16_t i;

	if((uint32_t) (startAddress + size) > 0xFFFF) {
		println((uint8_t *) "ERROR: size too big!");
		return;
	}

	for(i = 0; i < size; i++)
	{
		buffer[i] = ReadByte(startAddress + i, CS_RAM);
	}
}

void RAM_WriteBuffer(uint8_t *buffer, uint16_t startAddress, uint16_t size)
{
	uint16_t i;

	if((uint32_t) (startAddress + size) > 0xFFFF) {
		println((uint8_t *) "ERROR: size too big!");
		return;
	}

	for(i = 0; i < size; i++)
	{
		WriteByte(buffer[i], startAddress + i, CS_RAM);
	}
}

void WriteByte(uint8_t data, uint16_t address, uint8_t nCS)
{
	PreInit();

	PORTA = (address/256);

	/* bus control block */
	sbi(PORTD, PD4);//OE
	cbi(PORTD, nCS);//CS_RAMxM

	/* output address to the AD[0..7] bus */
	sbi(PORTB, PB0);//DIR_DATA = H
	DDRC = 0xFF;
	PORTC = address%256;
	cbi(PORTB, PB2);//nOE_DATA = 0

	/*latch address */
	sbi(PORTB, PB1);//ALE = 1
	_delay_us(2);
	cbi(PORTB, PB1);//ALE = 0
	_delay_us(2);

	/* output address to the A[0..15] bus*/
	cbi(PORTB, PB3);//nOE_ADDR = 0

	/* output data to the AD[0..7] = D[0.77] bus */
	PORTC = data;

	/* write delay */
	_delay_us(2);
	cbi(PORTD, PD5);//WE

	_delay_us(2);
	sbi(PORTD, PD5);//WE
	_delay_us(2);
	sbi(PORTD, nCS);//CS_RAMxM

	sbi(PORTD, nCS);//CS_RAMxM
	sbi(PORTB, PB2);//nOE_DATA = H
	sbi(PORTB, PB3);//nOE_ADDR = H

	/* re-Init pins */
	PostInit();
}

uint8_t ReadByte(uint16_t address, uint8_t nCS)
{
	uint8_t input = 0x55;

	PreInit();

	PORTA = (address/256);

	/* bus control block */
	sbi(PORTD, PD4);//OE
	sbi(PORTD, PD5);//WE
	cbi(PORTD, nCS);//CS_RAMxM


	/* output address to the AD[0..7] bus */
	sbi(PORTB, PB0);//DIR_DATA = H
	DDRC = 0xFF;
	PORTC = address%256;
	cbi(PORTB, PB2);//nOE_DATA = 0

	/*latch address */
	sbi(PORTB, PB1);//ALE = 1
	_delay_us(2);
	cbi(PORTB, PB1);//ALE = 0
	_delay_us(2);

	/* output address to the A[0..15] bus*/
	cbi(PORTB, PB3);//nOE_ADDR = 0



	/* read data from the AD[0..7] = D[0.77] bus */
	DDRC = 0x00;
	PORTC = 0xFF; //enable pull-ups
	cbi(PORTB, PB0);//DIR_DATA = L
	cbi(PORTB, PB2);//nOE_DATA = L
	cbi(PORTD, PD4);//OE

	_delay_us(2);
	input = PINC;

	/*zbytecnost*/
	sbi(PORTD, nCS);//CS_RAMxM
	sbi(PORTD, PD4);//OE
	sbi(PORTB, PB2);//nOE_DATA = H
	sbi(PORTB, PB3);//nOE_ADDR = H

	/* re-Init pins */
	PostInit();

	return input;
}

void Init(void)
{
	// output control
	PORTB = 0x0F;
	DDRB = 0x0F;
	PORTB = 0x0E;

	// bus control
	PORTD = 0x78;
	DDRD = 0x78;
	PORTD = 0x78;

	// data & lower address
	PORTC = 0xFF;
	DDRC = 0x00;

	// higher address
	PORTA = 0x00;
	DDRA = 0xFF;
	PORTA = 0x00;
}

void PreInit(void)
{
	Init();
}

void PostInit(void)
{
	Init();
}

void pause(uint8_t *msg)
{
	uint8_t inp;

	// print user message
	//println(msg);
	TxString(msg);
	TxString((uint8_t *) ": ");
	
	// wait for user input
	TxString((uint8_t *) "press any key when ready ... ");
	inp = Rx();
	println((uint8_t *) "DONE");
}
