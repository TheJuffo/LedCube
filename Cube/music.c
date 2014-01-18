//
//  music.c
//  Led Cube
//
//  Created by Johan Andersson on 2013-11-02.
//  Copyright (c) 2013 Johan Andersson. All rights reserved.
//

#include <stdio.h>
#include "music.h"
#include "draw.h"
#include "effect.h"
#include "debug.h"
#include "util.h"
#include <stdbool.h>


void launch_music_effect(int effect)
{
    fill(0x00);
    
    switch(effect)
    {
        case 0:
            send_debug_info(PSTR("Music bars\r\n"), 1);
            music_bars();
            break;
        case 1:
            send_debug_info(PSTR("Music ball\r\n"), 1);
            music_ball();
        default:
            send_debug_info(PSTR("Music fail\r\n"), 1);
			effect_stringfly2(PSTR("MUSIC FAIL"), 1);
			break;
            
    }
}

void music_bars()
{
    unsigned int eq_buffer[14];
    int i;
    
    // Run this for 10 seconds
    set_timer(10000);
    
    while(!timer_expired())
    {
        EQ_Read(eq_buffer);
        fill(0x00);
        // Basen längst bak
        box_filled(7, 0, 0, 6, 3, (eq_buffer[0] / 128));
        box_filled(7, 4, 0, 6, 7, (eq_buffer[7] / 128));
        
        for(i = 1; i < 7; i++)
        {
            if(i % 2 == 1)
            {
                box_filled((7 - i), 0, 0, (6 - i), 1, (eq_buffer[i] / 128));
                box_filled((7 - i), 6, 0, (6 - i), 7, (eq_buffer[i + 7] / 128));
            }
            else
            {
                box_filled((7 - i), 2, 0, (6 - i), 3, (eq_buffer[i] / 128));
                box_filled((7 - i), 4, 0, (6 - i), 5, (eq_buffer[i + 7] / 128));

            }
        }
    }
}

void music_ball()
{
    unsigned int eq_buffer[14];
    int i;
    
    set_timer(10000);
    
    while(!timer_expired)
    {
        EQ_Read(eq_buffer);
        fill(0x00);
        
        
    }
}
