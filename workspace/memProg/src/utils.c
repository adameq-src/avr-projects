#include <avr/io.h>

void UsartInit(uint16_t ubrr)
{
    /* Set baud rate */
    UBRRH = (uint8_t)(ubrr>>8);
    UBRRL = (uint8_t)ubrr;
    /* Set frame format: 8data, 1stop bit */
    UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
    //URSEL decides where to write the values (either to UBRRH or to UCSRC), see datasheet
    /* Enable receiver and transmitter */
    UCSRB = (1<<RXEN)|(1<<TXEN);
}

void Tx(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while ( !( UCSRA & (1<<UDRE)) );
    /* Put data into buffer, sends the data */
    UDR = data;
}

unsigned char Rx(void)
{
    /* Wait for data to be received */
    while ( !(UCSRA & (1<<RXC)) );
    /* Get and return received data from buffer */
    return UDR;
}

void NewLine(void)
{
    Tx(0x0D); //CR
    Tx(0x0A); //LF
}

void TxString(uint8_t *string)
{
    while (*string)
    {
        loop_until_bit_is_set(UCSRA, UDRE);
        UDR = *string;
        string++;
    }
}

void println(uint8_t* string2)
{
    TxString(string2);
    NewLine();
}
