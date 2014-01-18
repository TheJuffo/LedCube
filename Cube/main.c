/*
 * Code to controll an 8x8x8 ledcube using avr
 * http://www.instructables.com/id/Led-Cube-8x8x8/
 * See lisence.txt for lisence.
 */
#include "main.h"
#include "effect.h"
#include "launch_effect.h"
#include "draw.h"
#include "debug.h"
#include "util.h"
#include "uart.h"
//#include <stdbool.h>

// Main loop
// the AVR enters this function at boot time
int main (void)
{
	int effect_index = 0;
    int tempval;
    
    // This function initiates IO ports, timers, interrupts and
    // serial communications
	ioinit();

    usart_init();
    
	debug_init();

	util_init();
	
    // This variable specifies which layer is currently being drawn by the
	// cube interrupt routine. We assign a value to it to make sure it's not >7.
	current_layer = 7;
	
    // This variable specifies the starting mode, this can be changed by pushing the
    // mode button or by typing in commands in the prompt.
    current_mode = MODE_RANDOM;

	//Binary mode
	binary_mode = 0;


    // Reads a value from AD converter and uses it to seed the pseudo random number generator,
    // Not perfect since AD converter returns a value from 0 to 1023 but good enough.
    tempval = ADC_Read(7);
	srand(tempval);

	// Enable interrupts
	// This starts the routine that draws the cube content
	sei();

	// Main loop, does awesome shit forever!
	while (1)
	{
        if(current_mode == MODE_RANDOM)
        {
            effect_index = rand() % EFFECTS_TOTAL;
            
            launch_effect(effect_index);
        }
        else if(current_mode == MODE_SEQUENTIAL)
        {
            effect_index++;
            effect_index %= EFFECTS_TOTAL;
            
            launch_effect(effect_index);
        }
        else if(current_mode == MODE_SELF_TEST)
        {
            override_delay(0);
            
			SelfTest();
        }
        else if(current_mode == MODE_MUSIC_RANDOM)
        {
            
        }
        else if(current_mode == MODE_MUSIC_SEQUENTIAL)
        {
            
        }
        else if(current_mode == MODE_BINARY)
        {
            rs232();
        }
        else
        {
            current_mode = MODE_RANDOM;
        }
	}
}

/*
 * Multiplexer/framebuffer routine
 * This function is called by an interrupt generated by timer 2.
*/

ISR(TIMER2_COMP_vect)
{
	int i;
    
    // Increment the current_layer counter so that the next layer is
    // drawn the next time this function runs.
    current_layer = (current_layer + 1) % 8;
    
    LAYER_SELECT = 0x00; // Turn all cathode layers off. (all transistors off)
    
    // Loop through all 8 bytes of data in the current layer in reverse order
    // and shift it onto the layer.
    for (i = 7; i >= 0; i--)
    {
        // Set the shift clock to 0
        SHIFTCLK_ADDR &= ~SHIFTCLK_MASK;
        // Set the data on the data-bus of the latch array.
        DATA_BUS = cube[current_layer][i];
        // Set the shift clock to 1 to create a rising edge and shift in the data on the data bus.
        SHIFTCLK_ADDR |= SHIFTCLK_MASK;
    }
    
    LAYER_SELECT = (0x01 << current_layer); // Transistor ON for current layer}
}
/*
 * This routine does several things
 * 1: Sends tick to the debug functions.
 * 2: Sends tick to the delay functions.
 * This function is called by an interrupt generated by timer 0.
 * 
*/
ISR(TIMER0_COMP_vect)
{
	bool keyPressed = false;
    bool keyChanged = false;
    
    delay_tick();

	debug_tick();
    
    DebounceKey(&keyChanged, &keyPressed);
    if(keyChanged && keyPressed)
    {
        current_mode++;
        current_mode %= NO_OF_MODES;
        if(current_mode == MODE_BINARY)
        {
            override_delay(1);
        }
    }
}

void ioinit (void)
{
	DDRA = 0x0c;    // EQ AD input and output
    DDRB = 0xff;    // Layer select and ISP
    DDRC = 0xff;    // Data port
    DDRD = 0xf3;    // RS232, buttons, LEDS and shift clock
    
    
    PORTA = 0x00;   // EQ AD input pullup off? EQ outputs off
    PORTB = 0x00;   // Layer select off
    PORTC = 0x00;   // Data port off
    PORTD = 0x04;   // All outputs off, enable pullup on button
    

	// Timer 0
	// General purpose timer interrupt
	// 14745600/64/230 = 1001 interrupts per second or 1 interrupt every 0,998ms
	OCR0 = 229;
	TCCR0 |= _BV(CS01) | _BV(CS02); // Prescaler = 64.
	TCCR0 |= _BV(WGM01);            // CTC mode. Reset counter when OCR0 is reached.
	TCNT0 = 0x00;                   // Initial counter value = 0;
	TIMSK |= _BV(OCIE0);            // Enable CTC interrupt.

	// Timer 2
	// Frame buffer interrupt
	// 14745600/128/11 = 10472.72 interrupts per second
	// 10472.72/8 = 1309 frames per second
	OCR2 = 10;                      // interrupt at counter = 10
    TCCR2 |= _BV(CS20) | _BV(CS22); // Prescaler = 128.
	TCCR2 |= _BV(WGM21);            // CTC mode. Reset counter when OCR2 is reached.
	TCNT2 = 0x00;                   // initial counter value = 0;
	TIMSK |= _BV(OCIE2);            // Enable CTC interrupt.

}

void SelfTest()
{
	int i, x, y;

	for(i = 0; i < 7; i++)
	{
		setplane_z(i);
		delay_ms(1000);	
		clrplane_z(i);
	}
	for(x = 0; x <= 7; x++)
	{
		for(y = 0; y <= 7; y++)
		{
			line(x, y, 0, x, y, 7);
			delay_ms(1000);
			fill(0);
		}
	}
}

// Take input from a computer and load it onto the cube buffer
void rs232()
{
	char tempval;
	int x = 0;
	int y = 0;
    int escape = 0;
	
	while (current_mode == MODE_BINARY)
	{
		// Switch state on red LED for debugging
		// Should switch state every time the code
		// is waiting for a byte to be received.
		debug_blink(LED_RED);

		// Wait until a byte has been received
		while(!HasChars());
        
		// Load the received byte from rs232 into a buffer.
		receive_char(&tempval);

		// Uncommet this to echo data back to the computer
		// for debugging purposes.
		//SendChar(tempval);

		// Every time the cube receives a 0xff byte,
		// it goes into sync escape mode.
		// if a 0x00 byte is then received, the x and y counters
		// are reset to 0. This way the x and y counters are
		// always the same on the computer and in the cube.
		// To send an 0xff byte, you have to send it twice!

		// Go into sync escape mode
		if (tempval == 0xff)
		{
            // Wait for the next byte
            while(!HasChars());
            // Get the next byte
            receive_char(&tempval);

            // Sync signal is received.
            // Reset x and y counters to 0.
            if (tempval == 0x00)
            {
                x = 0;
				y = 0;
                escape = 1;
            }
            // if no 0x00 byte is received, proceed with
            // the byte we just received.
		}

        if (escape == 0)
        {
			// Load data into the current position in the buffer
			fb[x][y] = tempval;

    		// Check if we have reached the limits of the buffer array.
    		if (y == 7)
    		{
    			if (x == 7)
    			{
    				// All data is loaded. Reset both counters
    				y = 0;
    				x = 0;
                    // Copy the data onto the cube.
    				tmp2cube();
    			} 
				else
    			{
    				// A layer is loaded, reset y and increment x.
    				x++;
    				y = 0;
    			}
    		} 
			else
    		{
    			// We are in the middle of loading a layer. increment y.
    			y++;
    		}
	
	    } 
		else
        {
            escape = 0;
        }
    }
}


