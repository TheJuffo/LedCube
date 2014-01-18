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

#define MODE_RANDOM 0x00
#define MODE_SEQUENTIAL 0x01
#define MODE_SELF_TEST 0x02
#define MODE_MUSIC_RANDOM 0x03
#define MODE_MUSIC_SEQUENTIAL 0x04
#define MODE_BINARY 0x05
#define NO_OF_MODES 0x05

void ioinit (void);

void SelfTest(void);

void rs232(void);

volatile unsigned char current_layer;

volatile unsigned char current_mode;

uint32_t *RandomSeed_ptr EEMEM;

uint8_t binary_mode;



#endif

