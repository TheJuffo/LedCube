#include "command_line.h"
#include "uart.h"

#define CMD_BUFFER_SIZE 64

unsigned char data[CMD_BUFFER_SIZE];
uint8_t current_pos = 0;
//uint8_t


void ReadAndExecute()
{
	char tempChar; 
	while(HasChars())
	{
		ReceiveChar(&tempChar);
		data[current_pos] = tempChar;

		if(tempChar = '\r')
		{

		}
	}
}