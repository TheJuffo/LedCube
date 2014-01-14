#include <avr/pgmspace.h>

#include "launch_effect.h"
#include "effect.h"
#include "draw.h"
#include "gameoflife.h"
#include "debug.h"
#include "util.h"

void launch_effect (int effect)
{
	int i;
    unsigned char ii;

	fill(0x00);

	switch (effect)
	{
		case 0x00:
			SendDebugInfo(PSTR("0: Rain\r\n"), 1);
			effect_rain(100);
			break;
		
			
		case 1:
			SendDebugInfo(PSTR("1: Random voxels\r\n"), 1);
			sendvoxels_rand_z(20,220,2000);
			break;
				
		case 2:
			SendDebugInfo(PSTR("2: Random filler\r\n"), 1);
			effect_random_filler(5,1);
			effect_random_filler(5,0);
			effect_random_filler(5,1);
			effect_random_filler(5,0);
			break;
				
		case 3:
			SendDebugInfo(PSTR("3: Z axis up/down\r\n"), 1);
			effect_z_updown(20,1000);
			break;
				
		case 4:
			SendDebugInfo(PSTR("4: Wormsqueeze\r\n"), 1);
			effect_wormsqueeze (2, AXIS_Z, -1, 100, 1000);
			break;
				
		case 5:
			SendDebugInfo(PSTR("5: Blinky\r\n"), 1);
			effect_blinky2();
			break;
				
		case 6: 
			SendDebugInfo(PSTR("6: Boxes\r\n"), 1);
            for (ii=0;ii<8;ii++)
			{
				effect_box_shrink_grow (1, ii % 4, ii & 0x04, 450);
			}

			effect_box_woopwoop(800,0);
			effect_box_woopwoop(800,1);
			effect_box_woopwoop(800,0);
			effect_box_woopwoop(800,1);
			break;
			
		case 7:
			SendDebugInfo(PSTR("7: Planboing\r\n"), 1);
			effect_planboing (AXIS_Z, 400);
			effect_planboing (AXIS_X, 400);
			effect_planboing (AXIS_Y, 400);
			effect_planboing (AXIS_Z, 400);
			effect_planboing (AXIS_X, 400);
			effect_planboing (AXIS_Y, 400);
			break;
		
		case 8:
			SendDebugInfo(PSTR("8: Telc stairs\r\n"), 1);
			effect_telcstairs(0,800,0xff);
			effect_telcstairs(0,800,0x00);
			effect_telcstairs(1,800,0xff);
			effect_telcstairs(1,800,0xff);
			break;
			
		case 9:
			SendDebugInfo(PSTR("9: Suspend random\r\n"), 1);
			effect_axis_updown_randsuspend(AXIS_Z, 550,5000,0);
			effect_axis_updown_randsuspend(AXIS_Z, 550,5000,1);
			effect_axis_updown_randsuspend(AXIS_Z, 550,5000,0);
			effect_axis_updown_randsuspend(AXIS_Z, 550,5000,1);
			effect_axis_updown_randsuspend(AXIS_X, 550,5000,0);
			effect_axis_updown_randsuspend(AXIS_X, 550,5000,1);
			effect_axis_updown_randsuspend(AXIS_Y, 550,5000,0);
			effect_axis_updown_randsuspend(AXIS_Y, 550,5000,1);
			break;
			
		case 10:
			SendDebugInfo(PSTR("10: Load bar\r\n"), 1);
			effect_loadbar(700);
			break;
			
		case 11:
			SendDebugInfo(PSTR("11: Worm squeeze\r\n"), 1);
			effect_wormsqueeze(1, AXIS_Z, 1, 100, 1000);
			break;
			
			
		case 12:
			SendDebugInfo(PSTR("12: String fly\r\n"), 1);
			effect_stringfly2(PSTR("INSTRUCTABLES"), 1);
			break;
			
		case 13:
			SendDebugInfo(PSTR("13: Game of Life\r\n"), 1);
            // Create a random starting point for the Game of Life effect.
			for (i = 0; i < 20;i++)
			{
				setvoxel(rand()%4,rand()%4,rand()%4);
			}
	
			gol_play(20, 400);
			break;
			
		case 14:
			SendDebugInfo(PSTR("14: Flying boxes?\r\n"), 1);
			effect_boxside_randsend_parallel (AXIS_Z, 0 , 200, 1);
			delay_ms(1500);
			effect_boxside_randsend_parallel (AXIS_Z, 1 , 200, 1);
			delay_ms(1500);
			
			effect_boxside_randsend_parallel (AXIS_Z, 0 , 200, 2);
			delay_ms(1500);
			effect_boxside_randsend_parallel (AXIS_Z, 1 , 200, 2);
			delay_ms(1500);
			
			effect_boxside_randsend_parallel (AXIS_Y, 0 , 200, 1);
			delay_ms(1500);
			effect_boxside_randsend_parallel (AXIS_Y, 1 , 200, 1);
			delay_ms(1500);
			break;
			
		case 15:
			SendDebugInfo(PSTR("15: Boing boing\r\n"), 1);
			boingboing(250, 600, 0x01, 0x02);
			break;
			
		case 16:
			SendDebugInfo(PSTR("16: Smiley spin\r\n"), 1);
			effect_smileyspin(2,1000,0);
			break;
			
		case 17:
			SendDebugInfo(PSTR("17: Path spiral\r\n"), 1);
			effect_pathspiral(100,500);
			break;
			
		case 18:
			SendDebugInfo(PSTR("18: Path bitmap\r\n"), 1);
			effect_path_bitmap(700,2,3);
			break;
			
		case 19:
			SendDebugInfo(PSTR("19: Smiley spin\r\n"), 1);
			effect_smileyspin(2,1000,1);
			break;
			
		case 20:
			SendDebugInfo(PSTR("20: Path text\r\n"), 1);
			effect_path_text(1000, "TG", 0);
			break;
	
		case 21:
			SendDebugInfo(PSTR("21: Path text\r\n"), 1);
			effect_rand_patharound(200,500);
			break;
			
		case 22:
			SendDebugInfo(PSTR("22: Worm squeeze\r\n"), 1);
			effect_wormsqueeze(1, AXIS_Z, -1, 100, 1000);
			break;
			
		case 23:
			SendDebugInfo(PSTR("23: Smiley spin\r\n"), 1);
			effect_smileyspin(2,1000,2);
			break;
			
		case 24:
			SendDebugInfo(PSTR("24: Random sparkle\r\n"), 1);
			effect_random_sparkle();
			break;
			
		case 25:
			SendDebugInfo(PSTR("25: Worm squeeze\r\n"), 1);
			effect_wormsqueeze(1, AXIS_Z, -1, 100, 1000);
			break;
		
		case 26:
			SendDebugInfo(PSTR("26: Boing boing\r\n"), 1);
			boingboing(250, 600, 0x01, 0x03);
			break;
		
		// In case the effect number is out of range:
		default:
			SendDebugInfo(PSTR("Fail\r\n"), 1);
			effect_stringfly2(PSTR("FAIL"), 1);
			break;
	}
}

