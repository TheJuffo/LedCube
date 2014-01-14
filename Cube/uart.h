#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <string.h>

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

// Member functions
void USARTInit(void);
int SendChar(char data);
int ReceiveChar(char *data);
int SendString(const char *data, uint8_t prgMem);
int ReceiveString(char * data, uint8_t MaxChars);

uint8_t HasChars(void);

void SetEcho(unsigned char echo);



#endif