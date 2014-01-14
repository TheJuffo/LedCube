#include "debug.h"
#include "uart.h"

void InitDebug()
{
	// This variable specifies whether or not debug leds are active
	debug_active = 1;

	// This variable specifies what the last debug mode was, in order to show debug mode on
	// the debug leds
	last_debug_mode = 1;
}


void ChangeDebugMode()
{
	debug_mode = (debug_mode + 1) % NO_OF_DEBUG_MODES;
}

void DebugTick()
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

void SendDebugInfo(const char * data, uint8_t prgMem)
{
	SendString(data, prgMem);
}

void DebugBlink(uint8_t LEDS)
{

}