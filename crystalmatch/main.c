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
    bool swapkeystatus;
    bool gameover = false;
    bool playing = true;
    bool choice;
    char key;
    uint16_t totalpoints = 0;

    con_init();                 // generic console
    random_init();              // random value generator
    queue_init();               // push/pop queue for stacked checks
    playfield_init_tiles();     // load tiles from memory to video memory

    while(playing)
    {
        swapkeystatus = false;
        gameover = false;
        con_cls();              // clear screen
        con_init_timer(120);       // reset timer for this game
        playfield_draw_borders();         
        playfield_init();
        playfield_draw();
        playfield_cursor_show(swapkeystatus);

        playfield_display_swap_message(swapkeystatus);
        while(!gameover)
        {
            key = con_getc_timer(750);
            switch(key)
            {
                case KEY_ENTER:   // LF / ENTER
                    swapkeystatus = !swapkeystatus;
                    playfield_display_swap_message(swapkeystatus);
                    playfield_cursor_show(swapkeystatus);
                    break;
                case 0x1b:
                    swapkeystatus = false;
                    playfield_display_swap_message(swapkeystatus);
                    playfield_cursor_show(swapkeystatus);
                    break;
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    if(swapkeystatus)
                    {
                        playfield_swap(key);
                        swapkeystatus = false;
                        playfield_display_swap_message(swapkeystatus);
                        playfield_cursor_show(swapkeystatus);
                    }
                    else
                    {
                        playfield_cursor_hide();
                        playfield_cursor_move(key);
                        playfield_cursor_show(swapkeystatus);
                    }
                    break;
                case 'Q':
                    gameover = true;
                    break;
                default:
                    break;
            }
        }
        // end of game - ask for new game or quit
        playfield_display_gameover();

        choice = false;
        while(!choice)
        {
            key = con_getc();
            switch(key)
            {
                case 'y':
                    choice = true;
                    playing = true;
                    break;
                case 'n':
                    choice = true;
                    playing = false;
                    break;
            }
        }
    }
    con_exit();
}
