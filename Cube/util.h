#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

void util_init(void);

void delay_tick(void);
void delay_ms(uint16_t delay);

void set_timer(unsigned int time_ms);
bool timer_expired(void);

void override_delay(unsigned char override);

uint8_t GetChar(const char *str_ptr, uint16_t index, uint8_t prgMem);

unsigned int ADC_Read(unsigned char input);
void EQ_Read(unsigned int * buffer_ptr);

void DebounceKey(bool *keyChanged, bool *keyPressed);

#endif