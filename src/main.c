#ifndef MAIN_C
#define MAIN_C

#include "built_graphics/test.h"
#include "include/agb_debug/debug.c"
#include <pokeagb/pokeagb.h>

#define BITMAP_TABLE_ADDR 0x08800000
#define INPUT_VAR 0x8002

#define CPUFSCPY 0
#define CPUFSSET 1
#define CPUModeFS(size, mode) ((size >> 2) | (mode << 24))

void **bitmap_table_addr = (void **)BITMAP_TABLE_ADDR;

void exit_anim();

void CpuFastSet(void* src, void* dst, u32 mode)
{
    __asm__("swi 0xC");
}

void main_c1_handler(){
	struct FadeControl pal_fade_control = *((struct FadeControl *)0x02037ab8);

	switch(super.multi_purpose_state_tracker){
        case 0:
            if (!pal_fade_control.active) {
                vblank_handler_set(0);
                hblank_handler_set(0);
                set_callback2(0);
                //enable mode 3
                lcd_io_set(REG_ID_DISPCNT, 0b0111111101100011);
                //reset bg2 (bitmap) scaling
                lcd_io_set(REG_ID_BG2PA, 0x0100);
                lcd_io_set(REG_ID_BG2PB, 0);
                lcd_io_set(REG_ID_BG2PC, 0);
                lcd_io_set(REG_ID_BG2PD, 0x0100);
                //reset bg2 (bitmap) x coord
                lcd_io_set(0x28, 0);
                lcd_io_set(0x2A, 0);
                //reset bg2 (bitmap) y coord
                lcd_io_set(0x2C, 0);
                lcd_io_set(0x2E, 0);
                
				//Clear VRAM
				u32 set = 0;
				CpuFastSet((void*)&set, (void*)ADDR_VRAM, CPUModeFS(0x10000, CPUFSSET));
                
                u16 var8002 = var_load(INPUT_VAR);

                //uncompress bitmap
                lz77UnCompVram(bitmap_table_addr[var8002], (void*)ADDR_VRAM);
				
				super.multi_purpose_state_tracker++;
			}
			break;
		case 1: //show screen
        switch (super.buttons_new_and_repeated) { 
                case KEY_SELECT:
                case KEY_R:
                case KEY_L: 
                case KEY_B:
                case KEY_A:
                case KEY_DOWN:
                case KEY_UP:
                case KEY_LEFT:
                case KEY_RIGHT:
                    exit_anim();
                    break;
			break;
        }
	}
	
}

int main(){
    tasks_init();
    fade_screen(0xFFFFFFFF, 0, 0, 16, 0x0000);
    set_callback1(main_c1_handler);
    super.multi_purpose_state_tracker = 0;
	return 1;
}
void exit_anim() {
    switch (super.multi_purpose_state_tracker) {
        case 0:
			fade_screen(0xFFFFFFFF, 0, 0, 16, 0x0000);
			gpu_sync_bg_hide(1);
			gpu_sync_bg_hide(0);	
			super.multi_purpose_state_tracker++;
            break;
        case 1:
            m4aMPlayVolumeControl(&mplay_BGM, 0xFFFF, 256);
            set_callback1(c1_overworld);
            set_callback2(c2_return_overworld_music_scripts_cont);
			super.multi_purpose_state_tracker++;
            break;
		case 2:
            fade_screen(0xFFFFFFFF, 0, 16, 0, 0x0000);
			gpu_sync_bg_show(0);
			gpu_sync_bg_show(1);
			super.multi_purpose_state_tracker++;
            script_env_2_enable_and_set_ctx_running();
		break;
	}
}

#endif