#ifndef DEBUG_H
#define DEBUG_H



// Red led on D4
#define LED_RED 0x10
// Green led D5
#define LED_GREEN 0x20
// Yellow led on D6
#define LED_YELLOW 0x40;

void debug_init(void);

void change_debug_mode(void);

void debug_tick(void);

void send_debug_info(const char * data, uint8_t prgMem);

void debug_blink(uint8_t LEDS);


#endif