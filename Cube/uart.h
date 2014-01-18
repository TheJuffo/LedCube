#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <string.h>
// Member functions
void usart_init(void);
int send_char(char data);
int send_string(const char *data);
int send_string_p(const char *data);

int receive_char(char *data);
int receive_string(char * data, int max_chars);

uint8_t HasChars(void);

void SetEcho(unsigned char echo);



#endif