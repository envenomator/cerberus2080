#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "console.h"
#include "random.h"
#include "queue.h"
#include "playfield.h"

int main()
{
    bool swapstatus = false;
    bool gameover = false;
    bool playing = true;
    char key;
    uint16_t totalpoints = 0;

    con_init();                 // generic console
    random_init();              // random value generator
    queue_init();               // push/pop queue for stacked checks
    playfield_init_tiles();     // load tiles from memory to video memory

    while(playing)
    {
        gameover = false;
        con_cls();              // clear screen
        con_init_timer();       // reset timer for this game
        draw_borders();         
        playfield_init();
        playfield_draw();
        cursor_show(swapstatus);

        display_swap_message(swapstatus);
        while(!gameover)
        {
            key = con_getc_timer(500);
            switch(key)
            {
                case KEY_ENTER:   // LF / ENTER
                    swapstatus = !swapstatus;
                    display_swap_message(swapstatus);
                    cursor_show(swapstatus);
                    break;
                case 0x1b:
                    swapstatus = false;
                    display_swap_message(swapstatus);
                    cursor_show(swapstatus);
                    break;
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    if(swapstatus)
                    {
                        playfield_swap(key);
                        swapstatus = false;
                        display_swap_message(swapstatus);
                        cursor_show(swapstatus);
                    }
                    else
                    {
                        cursor_hide();
                        cursor_move(key);
                        cursor_show(swapstatus);
                    }
                    break;
                case 'Q':
                    con_gotoxy(0,25);
                    con_puts("GAME OVER");
                    gameover = true;
                    break;
                default:
                    break;
            }
        }
        con_gotoxy(10,25);
        con_puts(" - New game? (Y/N)");
        key = con_getc();
        if(key == 'n' || key == 'N') playing = false;
    }
    con_exit();
}
