/*
 * command_line.h
 * Created by Johan Andersson
 * Functions for reading uart and then executing commands.
 */

#include <string.h>
#include "command_line.h"
#include "uart.h"


#define CMD_BUFFER_SIZE 64
#define NO_OF_COMMANDS 2

char data[CMD_BUFFER_SIZE];
uint8_t current_pos = 0;


const char *command_list [NO_OF_COMMANDS]  = {
    "setmode",
    "launcheffect"
};


void read_and_execute()
{
	char tempChar;
    
	while(HasChars())
	{
        receive_char(&tempChar);
		data[current_pos] = tempChar;
        
		if(tempChar == '\r')
		{
            parse();
		}
        
        current_pos++;
	}
}

void parse()
{
    char * command;
    int i;
    
    command = strtok(data, " ");
    
    for(i = 0; i < NO_OF_COMMANDS; i++)
    {
        if(strcasecmp(command, command_list[i]) == 0)
        {
            
        }
    }
    
    
}