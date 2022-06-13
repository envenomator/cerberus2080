#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"
#include "playfield.h"
#include "random.h"
#include "queue.h"

// On-screen playfield - containing IDs of crystals
// 0 - empty
// 1 - star
// 2 - circle
// 3 - square
// 4 - triangle
// 5 - dot

uint8_t playfield[FIELDWIDTH][FIELDHEIGHT];     // the actual playfield with IDs in a 2D grid
uint8_t playfield_cursorx;                      // Cursor X position
uint8_t playfield_cursory;                      // Cursor Y position
uint8_t playfield_missing[FIELDWIDTH];          // Horizontal drop-check after implosions. Each items contains #items to drop in that column

void playfield_init()
{
    // start out with an empty playfield
    uint8_t x, y;
    for(x = 0; x < FIELDWIDTH; x++)
    {
        for(y = 0; y < FIELDHEIGHT; y++)
        {
            playfield[x][y] = 1; // empty ID
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
    uint8_t tempid;
    uint8_t tempx = 0, tempy = 0;
    tempid = playfield[playfield_cursorx][playfield_cursory]; // record current location ID

    cursor_hide();
    switch(key)
    {
        case 'w':
            if(playfield_cursory > 0)
            {
                tempx = playfield_cursorx;
                tempy = playfield_cursory - 1;
                playfield[playfield_cursorx][playfield_cursory] = playfield[playfield_cursorx][tempy];
                playfield[playfield_cursorx][tempy] = tempid;
            }
            break;
        case 'a':
            if(playfield_cursorx > 0)
            {
                tempx = playfield_cursorx - 1;
                tempy = playfield_cursory;
                playfield[playfield_cursorx][playfield_cursory] = playfield[tempx][playfield_cursory];
                playfield[tempx][playfield_cursory] = tempid;
            }
            break;
        case 's':
            if(playfield_cursory < FIELDHEIGHT-1)
            {
                tempx = playfield_cursorx;
                tempy = playfield_cursory + 1;
                playfield[playfield_cursorx][playfield_cursory] = playfield[playfield_cursorx][tempy];
                playfield[playfield_cursorx][tempy] = tempid;
            }
            break;
        case 'd':
            if(playfield_cursorx < FIELDWIDTH-1)
            {
                tempx = playfield_cursorx + 1;
                tempy = playfield_cursory;
                playfield[playfield_cursorx][playfield_cursory] = playfield[tempx][playfield_cursory];
                playfield[tempx][playfield_cursory] = tempid;
            }
            break;
        default:
            break;
    }
    playfield_draw(); // complete the swap on screen
    queue_push(playfield_cursorx, playfield_cursory);    // push cursor position first
    queue_push(tempx, tempy);                            // push swapped position next
    playfield_implode_cycle();    // all the work goes here
    cursor_show();      // work is done, return to the user with an intact cursor
}

void playfield_implode_cycle()
{
    uint8_t tempx, tempy;

    while(queue_hasitems())
    {
        // get coordinates of current queue head item
        tempx = queue_getx();
        tempy = queue_gety();
        if(playfield_checkimplode(tempx, tempy))
        {
            // Mark all radial items hor/vert from current tempx/tempy coordinate as empty
            playfield_markempty(tempx, tempy);
            // GUI implode
            playfield_gui_implode(tempx, tempy);
            // Now collapse the field to fill implosion. Collapse will push/trigger additional checks in the queue
            playfield_collapse();
        }
        queue_pop();    // remove item we checked last
    }
}

void playfield_markempty(uint8_t tempx, uint8_t tempy)
{

}
void playfield_gui_implode(uint8_t tempx, uint8_t tempy)
{
    
}
void playfield_collapse()
{

}

void draw_borders()
{
    return;
}

void display_swap_message(bool swap)
{
    // stub code for now
    con_gotoxy(0,22);
    if(swap) con_puts("Select direction to swap");
    else     con_puts("ENTER to start swap mode");

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
}

bool playfield_checkimplode(uint8_t x, uint8_t y)
{
    uint8_t origin;
    uint8_t t;
    uint8_t number = 0;

    origin = playfield[x][y];
    
    // check left
    t = x;
    number = 1;
    while(t > 0)
    {
        t--;
        if(playfield[t][y] == origin) number++;
        else break; 
    }
    // check right
    t = x;
    while(t < FIELDWIDTH-1)
    {
        t++;
        if(playfield[t][y] == origin) number++;
        else break;
    }
    if(number > 2) return true;

    // check up
    t = y;
    number = 1;
    while(t > 0)
    {
        t--;
        if(playfield[x][t] == origin) number++;
        else break;
    }
    // check down
    t = y;
    while(t < FIELDHEIGHT-1)
    {
        t++;
        if(playfield[x][t] == origin) number++;
        else break;
    }
    if(number > 2) return true;
    return false;
}
