#include <avr/io.h>
#include "debug.h"
#include "uart.h"


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


void debug_init()
{
	// This variable specifies what the debug LEDs show, from the start they show nothing
	debug_mode = 0;

	// This variable specifies whether or not debug leds are active
	debug_active = 1;

	// This variable specifies what the last debug mode was, in order to show debug mode on
	// the debug leds
	last_debug_mode = 1;
}


void change_debug_mode()
{
	debug_mode = (debug_mode + 1) % NO_OF_DEBUG_MODES;
}

void debug_tick()
{
	// If debug mode changed
	if(debug_mode != last_debug_mode)
	{
		// disable showing debug info
		debug_active = 0;

		last_debug_mode = debug_mode;

		// Restart debug_mode_counter
		debug_mode_counter = DEBUG_MODE_COUNTER_START;

		//Set the leds to show current debug mode
		//LED_PORT = (debug_mode + 1) << LED_START;
	}

	// If currently showing debug mode instead of debug info
	if(debug_active == 0)
	{
		debug_mode_counter--;
		// If we're done showing debug mode
		if(debug_mode_counter == 0)
		{
			// Show debug info instead of debug_mode
			debug_active = 1;
			// Set the leds to show nothing
			//LED_PORT = 0x00;
			
		}
	}
}

void send_debug_info(const char * data, uint8_t prgMem)
{
	send_string(data);
}

void send_debug_info_p(const char * data)
{
    send_string_p(data);
}

void debug_blink(uint8_t LEDS)
{

}