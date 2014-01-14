#include "uart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/atomic.h>

void USARTInit()
{
	RINGFIFO_RESET(RXFifo);
	RINGFIFO_RESET(TXFifo);

    // Initiate RS232
    // USART Baud rate is defined in MYUBRR
    UBRRH = (unsigned char)(MYUBRR >> 8);
    UBRRL = (unsigned char)(MYUBRR);
    // UCSRC - USART control register
	// bit 7		register select, must be 1
    // bit 6		sync/ascyn 0 = async,  1 = sync
    // bit 5-4      parity 00 = disabled
    // bit 3        stop bits 0 = 1 bit  1 = 2 bits
    // bit 2-1      frame length 11 = 8
    // bit 0        clock polarity, should be 0 in asynch mode
    UCSRC  = 0b10000110;
    // Enable RS232, rx, tx, Data Register empty interrupt and rx complete interrupt
    UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(UDRIE) | _BV(RXCIE);

	// Local echo on
	m_Echo = 1;
}

int SendChar(char data)
{
	if(!RINGFIFO_FULL(TXFifo))
	{
		RINGFIFO_WR(TXFifo, data);

		//Ensure that no interrupts can fiddle with the UCSRB while we set the drie flag.
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			UCSRB |= _BV(UDRIE);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int SendString(const char * data, uint8_t prgMem)
{
	int pos = 0;
	uint8_t tempChar;

	if(prgMem == 0)
	{
		tempChar = data[pos];
	}
	else
	{
		tempChar = pgm_read_byte(&(data[pos]));
	}
	
	while(!RINGFIFO_FULL(TXFifo) && tempChar != 0)
	{
		RINGFIFO_WR(TXFifo, data[pos]);
		pos++;

		if(prgMem == 0)
		{
			tempChar = data[pos];
		}
		else
		{
			tempChar = pgm_read_byte(&(data[pos]));
		}
	}

	//Ensure that no interrupts can fiddle with the UCSRB while we set the drie flag.
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UCSRB |= _BV(UDRIE);
	}

	return pos;
}

// Receives a character from the SW buffer
int ReceiveChar(char *data)
{
	char tempValue;
	if(!RINGFIFO_EMPTY(RXFifo))
	{
		RINGFIFO_RD(RXFifo, tempValue);
		data[0] = tempValue;
		return 1;
	}
	else
	{
		return 0;
	}
}

// Receives a string from the SW buffer, 
// data: Pointer to a 
// Return value is number of chars that were read from the buffer
int ReceiveString(char * data, uint8_t maxChars)
{
	char tempValue;
	int charsRead = 0;
	while(!RINGFIFO_EMPTY(RXFifo) && charsRead < maxChars)
	{
		RINGFIFO_RD(RXFifo, tempValue);
		data[charsRead] = tempValue;
		charsRead++;
	}

	return charsRead;
}

// Set whether or not to echo received chars
void SetEcho(unsigned char echo)
{
	m_Echo = echo;
}

uint8_t HasChars()
{
	return !RINGFIFO_EMPTY(RXFifo);
}

// Interrupt handler for the Data Register Empty Interrupt
// Transmits the first character in the SW Buffer
// and disables the interrupt if there are no more characters to send.
ISR(USART_UDRE_vect)
{
	char tempValue;
	if(!RINGFIFO_EMPTY(TXFifo))
	{
		RINGFIFO_RD(TXFifo, tempValue);
		UDR = tempValue;
	}

	// Clear the Data register empty interrupt if there are no more data to be transmitted
	// This will be set by the sender functions once more data is loaded to the buffer.
	if(RINGFIFO_EMPTY(TXFifo))
	{
		UCSRB &= ~_BV(UDRIE);
	}
}

// Interrupt handler for the Receive Completed interrupt
// Re
ISR(USART_RXC_vect)
{
	// Status must be read before data
	char Status = UCSRA;
	char tempValue = UDR;
	// If there is no error when receiving
	if(!(Status & _BV(FE)) && !(Status & _BV(DOR)) && !(Status & _BV(PE)))
	{
		// If there is room in the buffer
		if(!RINGFIFO_FULL(RXFifo))
		{
			RINGFIFO_WR(RXFifo, tempValue);
			if(m_Echo != 0)
			{
				SendChar(tempValue);
			}
		}
	}
}