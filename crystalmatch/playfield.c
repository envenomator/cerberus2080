#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"
#include "playfield.h"
#include "random.h"

// On-screen playfield - containing IDs of crystals
// 0 - empty
// 1 - star
// 2 - circle
// 3 - square
// 4 - triangle
// 5 - dot

uint8_t playfield[FIELDWIDTH][FIELDHEIGHT];
uint8_t playfield_cursorx;
uint8_t playfield_cursory;

void playfield_init()
{
    // start out with an empty playfield
    uint8_t x, y;
    for(x = 0; x < FIELDWIDTH; x++)
    {
        for(y = 0; y < FIELDHEIGHT; y++)
        {
            playfield[x][y] = 0; // empty ID
        }
    }

    // cursor +/- in the middle
    playfield_cursorx = FIELDWIDTH/2;
    playfield_cursory = FIELDHEIGHT/2;
}
void playfield_init_random()
{
    uint8_t x, y;
    for(x = 0; x < FIELDWIDTH; x++)
    {
        for(y = 0; y < FIELDHEIGHT; y++)
        {
            playfield[x][y] = random_get();
        }
    }

    // cursor +/- in the middle
    playfield_cursorx = FIELDWIDTH/2;
    playfield_cursory = FIELDHEIGHT/2;
}
void playfield_load()
{
    // stub
    playfield_init();
}
char playfield_getchar(uint8_t id)
{
    char o;
    switch(id)
    {
        case 0:
            o = ' ';
            break;
        case 1:
            o = '*';
            break;
        case 2:
            o = 'O';
            break;
        case 3:
            o = '#';
            break;
        case 4:
            o = '^';
            break;
        case 5:
            o = '.';
            break;
        default:
            o = 'U';
            break;
    }
    return o;
}
void playfield_draw()
{
    uint8_t x,y;
    char o;

    for(y = 0; y < FIELDHEIGHT; y++)
    {
        for(x = 0; x < FIELDWIDTH; x++)
        {
            con_gotoxy(PLAYFIELD_STARTX+x,PLAYFIELD_STARTY+y);
            o = playfield_getchar(playfield[x][y]);
            con_putc(o);
        }
    } 
    return;
}
void playfield_swap(char key)
{
    if(key == 'a') return;
    return;
}

void draw_borders()
{
    return;
}

void display_swap_message()
{
    return;
}

void cursor_show()
{
    // for now, just inverse the current position
    char o;

    o = playfield_getchar(playfield[playfield_cursorx][playfield_cursory]);
    o = o | 128;

    con_gotoxy(PLAYFIELD_STARTX+playfield_cursorx, PLAYFIELD_STARTY+playfield_cursory);
    con_putc(o);
}
void cursor_hide()
{
    // for now, just normalize the current position
    char o;

    o = playfield_getchar(playfield[playfield_cursorx][playfield_cursory]);

    con_gotoxy(PLAYFIELD_STARTX+playfield_cursorx, PLAYFIELD_STARTY+playfield_cursory);
    con_putc(o);
}
void cursor_move(char key)
{
    cursor_hide();
    switch(key)
    {
        case 'w':
            if(playfield_cursory > 0) playfield_cursory--;  
            break;
        case 'a':
            if(playfield_cursorx > 0) playfield_cursorx--;
            break;
        case 's':
            if(playfield_cursory < FIELDHEIGHT-1) playfield_cursory++;
            break;
        case 'd':
            if(playfield_cursorx < FIELDWIDTH-1) playfield_cursorx++;
            break;
        default:
            break;
    }
    cursor_show();
}
