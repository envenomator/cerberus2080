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

unsigned char playfield_tiledefs[6][32] =
{
    {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
    {0x0,0x0,0x1,0x19,0x1D,0xF,0x7,0x3E,0x0,0x0,0x80,0x98,0xB8,0xF0,0xE0,0x7C,0x3E,0x7,0xF,0x1D,0x19,0x1,0x0,0x0,0x7C,0xE0,0xF0,0xB8,0x98,0x80,0x0,0x0},
    {0x0,0x0,0x3,0xE,0x18,0x10,0x30,0x20,0x0,0x0,0xC0,0x70,0x18,0x8,0xC,0x4,0x20,0x30,0x10,0x18,0xE,0x3,0x0,0x0,0x4,0xC,0x8,0x18,0x70,0xC0,0x0,0x0},
    {0x0,0x0,0x0,0x1F,0x1F,0x1F,0x1C,0x1C,0x0,0x0,0x0,0xF8,0xF8,0xF8,0x38,0x38,0x1C,0x1C,0x1F,0x1F,0x1F,0x0,0x0,0x0,0x38,0x38,0xF8,0xF8,0xF8,0x0,0x0,0x0},
    {0x0,0x0,0x1,0x1,0x1,0x3,0x5,0x3E,0x0,0x0,0x80,0x80,0x80,0xC0,0xA0,0x7C,0x3E,0x5,0x3,0x1,0x1,0x1,0x0,0x0,0x7C,0xA0,0xC0,0x80,0x80,0x80,0x0,0x0},
    {0x0,0x0,0x0,0x0,0x0,0x3,0x7,0x7,0x0,0x0,0x0,0x0,0x0,0xC0,0xE0,0xE0,0x7,0x7,0x3,0x0,0x0,0x0,0x0,0x0,0xE0,0xE0,0xC0,0x0,0x0,0x0,0x0,0x0}
};
unsigned char playfield_tiles[6][4] = // Actual IDs in video memory
{
    {128,129,130,131},
    {132,133,134,135},
    {136,137,138,139},
    {140,141,142,143},
    {144,145,146,147},
    {148,149,150,151}
};
void playfield_init_tiles()
{
    chardefs *ptr = (chardefs *)0xf000; // start of video character definitions
    uint8_t n,i;
    uint8_t (*p)[32];

    p = playfield_tiledefs;

    for(n = 0; n < 6; n++)
    {
        for(i = 0; i < 4; i++)
        {
            memcpy((void *)ptr[(playfield_tiles[n][i])], *(p + n) + i*8, 8);
        }
    }
}
void playfield_init_empty()
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

void playfield_init()
{
    uint8_t data[] = {1,2,2,1,5,5,1,2,3,1,2,5,1,1,2,1,3,1,5,1,1,5,3,2,1,4,1,2,1,4,2,1,1,2,1,3,2,1,3,1,1,3,5,1,5,5,1,3,1,4};
    uint8_t x,y,n;

    n = 0;
    for(y = 0; y < FIELDHEIGHT; y++)
    {
        for(x = 0; x < FIELDWIDTH; x++)
        {
            playfield[x][y] = data[n++];
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

}
void playfield_draw_old()
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
        case KEY_UP:
            if(playfield_cursory > 0)
            {
                tempx = playfield_cursorx;
                tempy = playfield_cursory - 1;
                playfield[playfield_cursorx][playfield_cursory] = playfield[playfield_cursorx][tempy];
                playfield[playfield_cursorx][tempy] = tempid;
            }
            break;
        case KEY_LEFT:
            if(playfield_cursorx > 0)
            {
                tempx = playfield_cursorx - 1;
                tempy = playfield_cursory;
                playfield[playfield_cursorx][playfield_cursory] = playfield[tempx][playfield_cursory];
                playfield[tempx][playfield_cursory] = tempid;
            }
            break;
        case KEY_DOWN:
            if(playfield_cursory < FIELDHEIGHT-1)
            {
                tempx = playfield_cursorx;
                tempy = playfield_cursory + 1;
                playfield[playfield_cursorx][playfield_cursory] = playfield[playfield_cursorx][tempy];
                playfield[playfield_cursorx][tempy] = tempid;
            }
            break;
        case KEY_RIGHT:
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
            con_getc();
            // Now collapse the field to fill implosion. Collapse will push/trigger additional checks in the queue
            playfield_collapse();
            playfield_draw();
        }
        queue_pop();    // remove item we checked last
    }
}

void playfield_markempty(uint8_t x, uint8_t y)
{
    // Marks all horizontal/vertical neighbors with same ID as EMPTY id (0)
    // Also mark number of empty items at each row for later collapse
    uint8_t origin;
    uint8_t t;

    origin = playfield[x][y];
   
    // clear missing table out to 0 first
    for(t = 0; t < FIELDWIDTH; t++) playfield_missing[t] = 0;

    // collapse x/y position first
    playfield_missing[x] = 1;

    // mark given position as empty
    playfield[x][y] = 0;

    // check left
    t = x;
    while(t > 0)
    {
        t--;
        if(playfield[t][y] == origin)
        {
            playfield[t][y] = 0;   // mark empty id
            playfield_missing[t] = 1;   // mark collapse
        } 
        else break; 
    }
    // check right
    t = x;
    while(t < FIELDWIDTH-1)
    {
        t++;
        if(playfield[t][y] == origin)
        {
            playfield[t][y] = 0;
            playfield_missing[t] = 1;
        } 
        else break;
    }
    // check up
    t = y;
    while(t > 0)
    {
        t--;
        if(playfield[x][t] == origin)
        {
            playfield[x][t] = 0;
            playfield_missing[x] += 1;
        } 
        else break;
    }
    // check down
    t = y;
    while(t < FIELDHEIGHT-1)
    {
        t++;
        if(playfield[x][t] == origin)
        {
            playfield[x][t] = 0;   
            playfield_missing[x] += 1;
        } 
        else break;
    }
}

void playfield_gui_implode(uint8_t x, uint8_t y)
{
    // implode area around tempx / tempy
    // simply redraw entire playfield for now
    playfield_draw(); 
}

void playfield_collapse()
{
    // check horizontal missing table, drop # of missing items in each column
    uint8_t x,y,yt,n;

    for(x = 0; x < FIELDWIDTH; x++)
    {
        n = playfield_missing[x];
        if(n)
        {
            // find bottom position first; position is marked as empty id (0)
            y = FIELDHEIGHT - 1;
            while(playfield[x][y]) y--;

            // now collapse remaining stack
            // first check if there is anything left on top
            if(y > (n-1))
            {
                yt = y - n; // top of the ceiling
                while(1) 
                {
                    playfield[x][y] = playfield[x][yt]; // swap with immediate top neighbor
                    queue_push(x,y);                    // record change to this location, needs check later
                    // later on, need to display this change
                    // display
                    // delay() etc
                    // display, something
                    y--;
                    if(yt) yt--;
                    else break; // we have copied the last item from the top (yt==0)
                }
            }
            
            // now fill new entries from the top
            while(n)
            {
                playfield[x][y] = random_get();
                queue_push(x,y);
                // later on, need to display this change
                y--;
                n--;
            }            
        }
    }
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
        case KEY_UP:
            if(playfield_cursory > 0) playfield_cursory--;  
            break;
        case KEY_LEFT:
            if(playfield_cursorx > 0) playfield_cursorx--;
            break;
        case KEY_DOWN:
            if(playfield_cursory < FIELDHEIGHT-1) playfield_cursory++;
            break;
        case KEY_RIGHT:
            if(playfield_cursorx < FIELDWIDTH-1) playfield_cursorx++;
            break;
        default:
            break;
    }
}

bool playfield_checkimplode(uint8_t x, uint8_t y)
{
    // checks if the given coordinate NEEDS implosion, doesn't actually do anything to the playfield
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
