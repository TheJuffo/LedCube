#include "util.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay_basic.h>
#include <util/atomic.h>

volatile uint16_t m_current_delay;


void InitUtil()
{
	// Set up AD Converter
	// Sets prescaler to 128
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// Set to use VCC as reference
	ADMUX |= _BV(REFS0);
	// Enables the ADC
	ADCSRA |= _BV(ADEN);
}

void DelayTick()
{
	if(m_override_delay == 1)
	{
		m_current_delay = 0;
	}
	else
	{
		// Count down if there is a delay started
		if(m_current_delay > 0)
		{
			m_current_delay--;
		}
	}
}

void delay_ms(uint16_t delay)
{
	if(m_override_delay == 0)
	{
		if(delay < UINT16_MAX)
		{
			// Adding 1 to ensure that the delay the wait time is at least "delay" ms.
			m_current_delay = delay + 1;
		}
	
		while(m_current_delay != 0);
	}
}

void OverrideDelay(uint8_t override_delay)
{
	m_override_delay = override_delay;
}

uint8_t GetChar(const uint8_t * str_ptr, uint8_t prgMem)
{
	if(prgMem)
	{
		return pgm_read_byte(str_ptr);
	}
	else
	{
		return *str_ptr;
	}
}

#define MUX_INPUT_MASK 0x1f

unsigned int ADC_Read(unsigned char portNo)
{
	unsigned int ad_value;
	// Clears the input 
	ADMUX &= ~MUX_INPUT_MASK;

	// Selects input
	ADMUX |= portNo;

	// Starts the conversion
	ADCSRA |= _BV(ADSC);

	// Waits until its don
	while(!(ADCSRA & ADIF));

	// ADCL must be read before ADCH
	ad_value = ADCL;
	ad_value = ADCH << 8;

	return 0;
}

#define EQ_PORT PORTA
#define EQ_STROBE 0x04
#define EQ_RESET 0x08
#define EQ_AD_LEFT 0x01
#define EQ_AD_RIGHT 0x00

#define STROBE_TO_STROBE_DELAY (((72 * F_CPU) / 4 / 1000000) + 1)
#define STROBE_PULSE_WIDTH (((18 * F_CPU) / 4 / 100000) + 1)
#define OUTPUT_SETTLING_TIME (((36 * F_CPU) / 4 / 100000) + 1)


// buffer_ptr should be an array of 14 unsigned ints.
// Values are entered with the left channel on the first 7 indices and the right on the last 7 indices
// Values are entered with the lowest frequency on index 0 and 7 respectively and going up.
// Frequencys are in ascending order: 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz.
void EQ_Read(unsigned int * buffer_ptr)
{
	int i;

	// Resets the control port
	EQ_PORT |= EQ_RESET;
	// Inserts a couple of nops to ensure that the minimum reset time of 100ns is fulfilled
	asm volatile(" nop");
	asm volatile(" nop");
	EQ_PORT &= ~EQ_RESET;

	for(i = 0; i < 7; i++)
	{
		// Delays to ensure that the Strobe to Strobe delay is fulfilled (and also the reset to strobe delay)
		_delay_loop_2(STROBE_TO_STROBE_DELAY);
		// Pulses the strobe
		EQ_PORT |= EQ_STROBE;
		// Delay to ensure that the minimum pulse width is fulfilled
		_delay_loop_2(STROBE_PULSE_WIDTH);
		EQ_PORT &= ~EQ_STROBE;
		// Delay to get the outputs to settle
		_delay_loop_2(OUTPUT_SETTLING_TIME);
		// And read the two chips.
		buffer_ptr[i] = ADC_Read(EQ_AD_LEFT);
		buffer_ptr[(i + 7)] = ADC_Read(EQ_AD_RIGHT);
	}
}

#define CHECK_MS 1
#define PRESS_MS 20
#define RELEASE_MS 50
#define BUTTON_PORT PORTD
#define BUTTON0 0x04

void DebounceKey(bool *keyChanged, bool *keyPressed)
{
	static unsigned char count = PRESS_MS / CHECK_MS;
	static bool debouncedKeyPress = false;

	*keyChanged = false;
	*keyPressed = debouncedKeyPress;

	bool rawState = !(BUTTON_PORT & BUTTON0);

	if(rawState == debouncedKeyPress)
	{
		if(debouncedKeyPress)
		{
			count = RELEASE_MS / CHECK_MS;
		}
		else
		{
			count = PRESS_MS / CHECK_MS;
		}
	}
	else
	{
		count--;
		if(count == 0)
		{
			debouncedKeyPress = rawState;
			*keyChanged = true;
			*keyPressed = debouncedKeyPress;
			if(debouncedKeyPress)
			{
				count = RELEASE_MS / CHECK_MS;
			}
			else
			{
				count = PRESS_MS / CHECK_MS;
			}
		}
	}
}