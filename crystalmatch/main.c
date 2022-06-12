#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"
#include "random.h"
#include "queue.h"
#include "playfield.h"

int main()
{
    char key;
    bool swap = false;

    random_init();
    playfield_init();
    queue_init();
    con_init();

    draw_borders();
    playfield_init_random();
    playfield_draw();
    cursor_show();

    display_swap_message(swap);
    while(1)
    {
        key = con_getc();
        switch(key)
        {
            case 0xd:   // LF / ENTER
                swap = !swap;
                display_swap_message(swap);
                break;
            case 0x1b:  // ESCAPE
                swap = false;
                display_swap_message(swap);
            case 'w':
            case 'a':
            case 's':
            case 'd':
                if(swap)
                {
                    playfield_swap(key);
                    swap = false;
                    display_swap_message(swap);
                }
                else cursor_move(key);
                break;
            default:
                break;
        }
    }
}
