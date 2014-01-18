#include "uart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/atomic.h>

// Define USART Baudrate
#define BAUD 115200
#define MYUBRR (((((F_CPU * 10) / (16L * BAUD)) + 5) / 10) - 1)

#define RINGFIFO_SIZE (128) // serial buffer in bytes (power 2)
#define RINGFIFO_MASK (RINGFIFO_SIZE - 1ul) // buffer size mask

// Buffer read / write macros
#define RINGFIFO_RESET(ringFifo)      {ringFifo.rdIdx = ringFifo.wrIdx = 0;}
#define RINGFIFO_WR(ringFifo, dataIn) {ringFifo.data[RINGFIFO_MASK & ringFifo.wrIdx++] = (dataIn);}
#define RINGFIFO_RD(ringFifo, dataOut){ringFifo.rdIdx++; dataOut = ringFifo.data[RINGFIFO_MASK & (ringFifo.rdIdx-1)];}
#define RINGFIFO_EMPTY(ringFifo)      (ringFifo.rdIdx == ringFifo.wrIdx)
#define RINGFIFO_FULL(ringFifo)       ((RINGFIFO_MASK & ringFifo.rdIdx) == (RINGFIFO_MASK & (ringFifo.wrIdx+1)))
#define RINGFIFO_COUNT(ringFifo)      (RINGFIFO_MASK & (ringFifo.wrIdx - ringFifo.rdIdx))

// buffer type
typedef struct{
    uint32_t size;
    uint32_t wrIdx;
    uint32_t rdIdx;
    uint8_t data[RINGFIFO_SIZE];
} RingFifo_t;

RingFifo_t RXFifo;
RingFifo_t TXFifo;


// Member variables
char m_Echo;

void usart_init()
{
	RINGFIFO_RESET(RXFifo);
	RINGFIFO_RESET(TXFifo);

    // Initiate RS232
    // USART Baud rate is defined in MYUBRR
    UBRRH = MYUBRR >> 8;
    UBRRL = MYUBRR;
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
    //UDR = 0x00; // send an empty byte to indicate powerup.


	// Local echo on
	m_Echo = 1;
}


int send_char(char data)
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


int send_string(const char *data)
{
	int pos = 0;
	uint8_t tempChar;

    tempChar = data[pos];
	
	while(!RINGFIFO_FULL(TXFifo) && tempChar != 0)
	{
		RINGFIFO_WR(TXFifo, data[pos]);
		pos++;

        tempChar = data[pos];
	}

	//Ensure that no interrupts can fiddle with the UCSRB while we set the drie flag.
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UCSRB |= _BV(UDRIE);
	}

	return pos;
}


int send_string_p(const char * data)
{
    int pos = 0;
	uint8_t tempChar;
    
	
    tempChar = pgm_read_byte(&(data[pos]));
	
	while(!RINGFIFO_FULL(TXFifo) && tempChar != 0)
	{
		RINGFIFO_WR(TXFifo, data[pos]);
		pos++;

        tempChar = pgm_read_byte(&(data[pos]));
	}
    
	//Ensure that no interrupts can fiddle with the UCSRB while we set the drie flag.
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UCSRB |= _BV(UDRIE);
	}
    
	return pos;
}


// Receives a character from the SW buffer
int receive_char(char *data)
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
// data: Pointer to where the data will be received
// Return value is number of chars that were read from the buffer
int receive_string(char * data, int max_chars)
{
	char tempValue;
	int charsRead = 0;
	while(!RINGFIFO_EMPTY(RXFifo) && charsRead < max_chars)
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
				send_char(tempValue);
			}
		}
	}
}