#ifndef DEBUG_H
#define DEBUG_H

#include <avr/io.h>

// Red led on D4
#define LED_RED 0x10
// Green led D5
#define LED_GREEN 0x20
// Yellow led on D6
#define LED_YELLOW 0x40;

#define LED_START 4
// Leds connected to port D
//#define LED_PORT PORTD

// Start value of the counter, 2000 = 2s
#define DEBUG_MODE_COUNTER_START 2000


// Show no activity on the debug leds
#define DEBUG_MODE_NONE 0x00
// Show RS232 activity on the debug leds
#define DEBUG_MODE_RS232 0x01
// Show Framebuffer activity on the debug leds
#define DEBUG_MODE_FRAMEBUFFER 0x02
// Show blinking debug leds
#define DEBUG_MODE_BLINKING 0x03
// Number of debug modes, increment this when adding a new debug mode
#define NO_OF_DEBUG_MODES 0x04

volatile unsigned char debug_mode;
volatile unsigned char debug_active;
volatile unsigned char last_debug_mode;
volatile unsigned char debug_mode_counter;

void InitDebug(void);

void ChangeDebugMode(void);

void DebugTick(void);

void SendDebugInfo(const char * data, uint8_t prgMem);

void DebugBlink(uint8_t LEDS);


#endif