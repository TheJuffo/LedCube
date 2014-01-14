#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>



volatile uint8_t m_override_delay;

void InitUtil(void);

void DelayTick(void);

void delay_ms(uint16_t delay);

void OverrideDelay(uint8_t override_delay);

uint8_t GetChar(const uint8_t *str_ptr, uint8_t prgMem);

unsigned int ADC_Read(unsigned char portNo);

void EQ_Read(unsigned int * buffer_ptr);

void DebounceKey(bool *KeyChanged, bool *Key_pressed);

#endif