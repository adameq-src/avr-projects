/*
Author: adameq
 */


#include <avr/io.h>
#define F_CPU 9600000UL
#include <util/delay.h>


#define 	sbi(port, bit)   (port) |= (1 << (bit))
#define 	cbi(port, bit)   (port) &= ~(1 << (bit))


#define		LED			PB0//OC0A

#define		DIV_1		(uint8_t) 0x00
#define		DIV_2		(uint8_t) 0x01
#define		DIV_4		(uint8_t) 0x02
#define		DIV_8		(uint8_t) 0x03
#define		DIV_16		(uint8_t) 0x04
#define		DIV_32		(uint8_t) 0x05
#define		DIV_64		(uint8_t) 0x06
#define		DIV_128		(uint8_t) 0x07
#define		DIV_256		(uint8_t) 0x08

void init(void);
void startTimer(uint8_t mode);
void divideClock(uint8_t div);
/*
TCCR0A:
	-COM0A[7:6]	= 0b01
	-COM0B[5:4]	= 0b00
	-reser[3:2]	
	-WGM01[0:1]	= 0b10

TCCR0A = 0x42
TCCR0B = 0x05;//divide by 1024
OCR0A je strop


TCCR1:
	-CTC1[7] 	= 1
	-PWM1A[6] 	= 0
	-COM1A[5:4]	= 0b01
	-CS1[3:0]	=
*/

/*
F[Hz]	OCR0A	N0	DIV
-------------------------
1200	249		8	2
2400	249		8	1
4800	124		8	1
9600	249		1	2
19200	249		1	1
38400	124		1	1


F[Hz]	OCR1A	N1		DIV
----------------------------
1		145		16384	2
2		72		16384	2
4		36		16384	2
8		17		16384	2
*/

int main(void)
{
	init();


	//startTimer(1);
	//divideClock(DIV_1);
	
	OCR0A = 0x02; //--> 1.6 MHz
	TCCR0A = 0x42;
	TCCR0B = 0x01;//divide by N = 1	

	OCR1A = 124; //-->38400 Hz (16*2400)
	TCCR1 = 0x91; 




	while(1) {
		/*
		sbi(PORTB, LED);
		_delay_ms(500);
		cbi(PORTB, LED);
		_delay_ms(500);
		*/
	}

}

void init(void)
{
	//DDRB = 0x01;
	sbi(DDRB, PB0);
	sbi(DDRB, PB1);
	PORTB = 0x00;
}

void divideClock(uint8_t div)
{
	CLKPR = 0x80;
	CLKPR = div;
}

void startTimer(uint8_t mode)
{
	//f_OC0A = F_CPU/(2*N*(OCR0A + 1))
	//OCR0A \ -1 + F_CPU/(2*N*f_OC0A)
	switch(mode) {
		case 0x00:
			divideClock(DIV_32);
			OCR0A = 0xFF;
			TCCR0A = 0x42;
			TCCR0B = 0x05;//divide by N = 1024
		break;

		case 0x01:
			divideClock(DIV_1);
			OCR0A = 249;
			TCCR0A = 0x42;
			TCCR0B = 0x02;//divide by N = 8
		break;

		case 0x02:
			divideClock(DIV_1);
			OCR0A = 0x04;
			TCCR0A = 0x42;
			TCCR0B = 0x01;//divide by N = 1	
		break;

		default:
		break;
	}
}

/*

*/

