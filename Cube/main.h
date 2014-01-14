#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

#define DATA_BUS PORTC
#define LAYER_SELECT PORTB
#define SHIFTCLK_ADDR PORTD
#define SHIFTCLK_MASK 0x80


// Mode select button on D2
#define MODE_SELECT_BTN 0x04

void ioinit (void);

void SelfTest(void);

volatile unsigned char current_layer;

volatile unsigned char override_delay;
volatile unsigned char self_test;

uint32_t *RandomSeed_ptr EEMEM;

uint8_t binary_mode;



#endif

