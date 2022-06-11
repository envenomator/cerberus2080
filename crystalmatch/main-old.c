#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"

#define FIELDWIDTH 20
#define FIELDHEIGHT 15
#define QUEUEDEPTH  FIELDWIDTH*FIELDHEIGHT*2
#define PLAYFIELD_STARTX    0
#define PLAYFIELD_STARTY    0

// Pseudo-random list of IDs
uint8_t random_list[] = {5, 5, 1, 2, 1, 2, 2, 1, 2, 5, 2, 4, 3, 5, 1, 5, 3, 1, 1, 2, 5, 1, 4, 3, 1, 3, 5, 2, 2, 3, 5, 1, 1, 1, 3, 1, 1, 1, 5, 1, 2, 2, 4, 4, 4, 1, 3, 3, 5, 4, 1, 2, 5, 2, 2, 3, 2, 4, 2, 5, 3, 5, 2, 3, 3, 5, 1, 2, 3, 5, 3, 3, 4, 3, 4, 4, 4, 5, 4, 1, 4, 4, 2, 1, 4, 2, 3, 4, 1, 4, 3, 3, 2, 5, 5, 2, 5, 5, 5, 5, 4, 5, 1, 1, 3, 5, 1, 5, 5, 4, 4, 2, 3, 3, 2, 2, 5, 1, 2, 1, 5, 4, 1, 5, 5, 2, 3, 1, 3, 3, 4, 1, 5, 5, 5, 2, 1, 2, 5, 5, 4, 1, 3, 5, 5, 2, 2, 3, 5, 3, 2, 4, 1, 1, 1, 3, 1, 1, 2, 2, 5, 2, 5, 1, 2, 2, 2, 3, 2, 3, 2, 2, 5, 4, 2, 4, 1, 2, 1, 1, 2, 5, 4, 1, 1, 2, 2, 4, 4, 1, 2, 1, 2, 5, 1, 5, 3, 5, 3, 3, 1, 5, 2, 4, 5, 1, 5, 1, 4, 2, 2, 4, 3, 3, 4, 1, 1, 3, 1, 2, 5, 4, 1, 4, 4, 4, 1, 2, 3, 5, 3, 2, 5, 4, 5, 3, 2, 3, 3, 1, 3, 3, 4, 3, 5, 3, 1, 1, 5, 5, 5, 4, 3, 5, 4, 5, 3, 4, 5, 3, 5, 2, 4, 1, 3, 4, 5, 5, 1, 1, 4, 1, 3, 5, 4, 5, 2, 5, 4, 4, 3, 1, 2, 2, 4, 1, 1, 2, 1, 5, 2, 2, 2, 1, 2, 1, 3, 5, 5, 5, 4, 4, 3, 3, 5, 1, 5, 5, 5, 2, 3, 1, 1, 5, 5, 5, 5, 4, 5, 5, 2, 5, 1, 4, 1, 3, 5, 3, 1, 1, 2, 1, 3, 5, 4, 4, 4, 5, 2, 4, 5, 2, 5, 4, 2, 3, 1, 3, 3, 1, 1, 2, 4, 3, 1, 4, 5, 4, 5, 3, 1, 2, 5, 1, 3, 4, 5, 3, 5, 3, 5, 2, 4, 1, 2, 5, 2, 3, 1, 1, 5, 4, 3, 3, 3, 5, 3, 3, 1, 1, 4, 5, 5, 5, 4, 5, 3, 3, 3, 5, 4, 4, 4, 5, 3, 3, 2, 2, 4, 4, 4, 2, 5, 2, 2, 2, 3, 1, 4, 4, 1, 4, 4, 1, 3, 1, 3, 2, 1, 5, 4, 5, 3, 5, 4, 2, 5, 1, 5, 3, 4, 3, 2, 1, 2, 4, 1, 3, 5, 2, 2, 3, 2, 4, 1, 5, 1, 2, 4, 3, 4, 3, 4, 3, 4, 5, 4, 4, 2, 4, 5, 4, 3, 1, 3, 1, 2, 4, 1, 2, 1, 4, 5, 3, 2, 1, 4, 1, 2, 2, 4, 4, 5, 4, 2, 4, 3, 4, 3, 1, 1, 2, 3, 2, 5, 5, 4, 3, 2, 1, 1, 3, 3, 5, 2, 4, 2, 1, 4, 5, 1, 2, 5, 3, 5, 2, 2, 4, 4, 2, 3, 5, 1, 1, 1, 5, 2, 5, 5, 3, 3, 1, 1, 5, 3, 1, 2, 4, 3, 3, 4, 5, 3, 3, 1, 3, 1, 2, 4, 5, 1, 4, 2, 3, 3, 4, 3, 5, 3, 4, 5, 5, 3, 4, 1, 1, 1, 5, 3, 5, 1, 3, 4, 4, 3, 4, 3, 1, 3, 1, 3, 2, 2, 3, 4, 2, 1, 4, 2, 4, 5, 2, 2, 5, 5, 2, 5, 1, 3, 5, 5, 5, 5, 5, 4, 4, 1, 3, 2, 3, 1, 5, 4, 5, 3, 1, 1, 2, 3, 3, 1, 3, 2, 5, 5, 4, 3, 5, 2, 1, 5, 4, 5, 4, 1, 2, 5, 3, 5, 4, 1, 1, 2, 5, 1, 1, 3, 4, 5, 5, 4, 1, 3, 3, 1, 2, 4, 5, 3, 5, 3, 2, 1, 3, 3, 5, 4, 5, 5, 3, 5, 1, 2, 3, 1, 1, 1, 3, 1, 3, 1, 2, 5, 1, 4, 3, 3, 3, 1, 2, 5, 5, 5, 4, 2, 2, 2, 2, 3, 1, 4, 4, 5, 3, 3, 1, 5, 2, 1, 3, 2, 1, 5, 5, 5, 3, 4, 5, 4, 1, 3, 3, 4, 5, 4, 1, 1, 5, 2, 4, 1, 3, 5, 2, 3, 2, 1, 1, 3, 5, 3, 4, 1, 4, 3, 2, 2, 4, 5, 5, 3, 1, 3, 1, 2, 4, 4, 3, 5, 4, 3, 1, 4, 4, 1, 1, 2, 5, 5, 3, 3, 3, 5, 3, 3, 3, 4, 2, 4, 1, 1, 3, 4, 3, 1, 1, 5, 4, 3, 3, 2, 3, 4, 5, 2, 1, 3, 5, 2, 3, 1, 5, 5, 2, 2, 1, 3, 5, 5, 4, 4, 2, 4, 4, 3, 2, 3, 5, 1, 2, 1, 2, 4, 1, 2, 2, 4, 3, 3, 1, 5, 5, 3, 3, 1, 5, 3, 2, 1, 3, 1, 2, 3, 5, 2, 5, 4, 5, 2, 3, 2, 1, 1, 2, 5, 3, 2, 1, 3, 3, 4, 5, 3, 4, 3, 2, 1, 3, 5, 2, 3, 3, 4, 4, 1, 5, 4, 3, 5, 3, 5, 2, 1, 2, 5, 2, 1, 5, 4, 5, 3, 5, 2, 4, 1, 4, 2, 2, 3, 2, 4, 1, 5, 5, 2, 4, 3, 1, 3, 2, 4, 4, 5, 5, 4, 3, 1, 5, 1, 3, 4, 2, 3, 3, 5, 3, 2, 4, 1, 3, 1, 1, 4, 5, 4, 1, 2, 2, 4, 5, 1, 2, 4, 1, 1, 3, 4, 4, 3, 2, 1, 5, 1, 2, 2, 5, 1, 4, 3, 5, 4, 3, 4, 1, 5, 2, 4, 2, 2, 4, 1, 2, 3, 1, 4, 5, 5, 5, 2, 4, 1, 1, 4, 2, 5, 4, 4, 3, 3, 1, 2, 5, 1, 5, 1, 1, 3, 2, 4, 1, 1, 5, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 5, 4};
uint8_t random_id;
void random_init()
{
    random_id = 0;
}
uint8_t random_get()
{
    // return next random number. roll over on 8-bit, so max 256 values
    return random_list[random_id++];
}

// Queue with to-be-checked coordinates
struct coordinate
{
    uint8_t x;
    uint8_t y;
};
struct coordinate queue[QUEUEDEPTH];
uint16_t queue_head;
uint16_t queue_tail;
uint16_t queue_items;
void queue_init()
{
    // start out with an empty queue
    queue_head = 0;
    queue_tail = 0;
    queue_items = 0;
}
void queue_add(uint8_t x, uint8_t y)
{
    queue_items++;
    queue_tail++;
    if(queue_tail == QUEUEDEPTH) queue_tail = 0;
    queue[queue_tail].x = x;
    queue[queue_tail].y = y;
}
void queue_delete()
{
    queue_items--;
    queue_head++;
    if(queue_head == QUEUEDEPTH) queue_head = 0;
}
bool queue_isempty()
{
    return queue_items == 0;
}
bool queue_hasitems()
{
    return queue_items > 0;
}

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
            switch(playfield[x][y])
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

    while(1)
    {
        key = con_getc();
        switch(key)
        {
            case 0xa:   // LF / ENTER
                swap = !swap;
                display_swap_message();
                break;
            case 'w':
            case 'a':
            case 's':
            case 'd':
                if(swap) playfield_swap(key);
                else cursor_move(key);
                break;
            default:
                break;
        }
    }
}
