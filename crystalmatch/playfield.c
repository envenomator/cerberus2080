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
// 4 - plus
// 5 - dot
// 6 - cross
// 7 - cursor ID
// 8 - buffer from
// 9 - buffer to

#define CURSOR_ID       7
#define BUFFERFROM_ID   8
#define BUFFERTO_ID     9

uint8_t playfield[FIELDWIDTH][FIELDHEIGHT];     // the actual playfield with IDs in a 2D grid
uint8_t playfield_cursorx;                      // Cursor X position
uint8_t playfield_cursory;                      // Cursor Y position
uint8_t playfield_missing[FIELDWIDTH];          // Horizontal drop-check after implosions. Each items contains #items to drop in that column

#define TIMERDELAY  200
#define TIMERDELAY2 4000
#define NRTILES     7

unsigned char playfield_tiledefs[7][32] =
{
    {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
    {0x0,0x0,0x1,0x19,0x1D,0xF,0x7,0x3E,0x0,0x0,0x80,0x98,0xB8,0xF0,0xE0,0x7C,0x3E,0x7,0xF,0x1D,0x19,0x1,0x0,0x0,0x7C,0xE0,0xF0,0xB8,0x98,0x80,0x0,0x0},
    {0x0,0x0,0x3,0xE,0x18,0x10,0x30,0x20,0x0,0x0,0xC0,0x70,0x18,0x8,0xC,0x4,0x20,0x30,0x10,0x18,0xE,0x3,0x0,0x0,0x4,0xC,0x8,0x18,0x70,0xC0,0x0,0x0},
    {0x0,0x0,0x0,0x1F,0x1F,0x1F,0x1C,0x1C,0x0,0x0,0x0,0xF8,0xF8,0xF8,0x38,0x38,0x1C,0x1C,0x1F,0x1F,0x1F,0x0,0x0,0x0,0x38,0x38,0xF8,0xF8,0xF8,0x0,0x0,0x0},
    {0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x3F,0x0,0x0,0x80,0x80,0x80,0x80,0x80,0xFC,0x3F,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0xFC,0x80,0x80,0x80,0x80,0x80,0x0,0x0},
    {0x0,0x0,0x0,0x0,0x0,0x3,0x7,0x7,0x0,0x0,0x0,0x0,0x0,0xC0,0xE0,0xE0,0x7,0x7,0x3,0x0,0x0,0x0,0x0,0x0,0xE0,0xE0,0xC0,0x0,0x0,0x0,0x0,0x0},
    {0x0,0x0,0x0,0x18,0x1C,0xE,0x7,0x3,0x0,0x0,0x0,0x18,0x38,0x70,0xE0,0xC0,0x3,0x7,0xE,0x1C,0x18,0x0,0x0,0x0,0xC0,0xE0,0x70,0x38,0x18,0x0,0x0,0x0}
};
unsigned char playfield_tiles[10][4] = // Actual IDs in video memory
{
    {128,129,130,131},
    {132,133,134,135},
    {136,137,138,139},
    {140,141,142,143},
    {144,145,146,147},
    {148,149,150,151},
    {152,153,154,155},
    {156,157,158,159},   // ID 7 is a buffer for a shown cursor - never drawn by playfield_draw, only by cursor_show()
    {160,161,162,163},   // ID 8 is DROP start buffer
    {164,165,166,167}    // ID 9 is DROP end buffer
};

unsigned char cursor_border[]={ 0x3F,0x40,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x80,0x80,0x80,0x80,0x80,0x80,0x40,0x3F,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0xFC};
unsigned char cursor_border_swapped[]={ 0x26,0x40,0x80,0x0,0x0,0x80,0x80,0x0,0x64,0x2,0x1,0x0,0x0,0x1,0x1,0x0,0x0,0x80,0x80,0x0,0x0,0x80,0x40,0x26,0x0,0x1,0x1,0x0,0x0,0x1,0x2,0x64};
unsigned char cursor_mask[]={ 0x0,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x0,0xFC,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x0,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFC,0x0};

void playfield_init_tiles()
{
    chardefs *ptr = (chardefs *)0xf000; // start of video character definitions
    uint8_t n,i;
    uint8_t (*p)[32];

    p = playfield_tiledefs;

    for(n = 0; n < NRTILES; n++)
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
    uint8_t data[] = {1,2,2,1,5,5,1,2,3,1,2,5,6,1,2,1,3,1,5,1,1,5,3,2,1,4,1,2,1,4,2,1,6,2,1,3,2,6,3,1,1,3,5,1,5,5,1,3,1,4,4,1,2,3,6,1,4,2,4,2,3,4,1,5,4,5,1,5,2,1};
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
void playfield_drawtile(uint8_t x, uint8_t y, uint8_t tileid)
{
    // Draw tileID on PLAYFIELD coordinate on screen
    // tileid is 0..5
    uint8_t screenx, screeny;

    screenx = PLAYFIELD_STARTX + (x << 1);
    screeny = PLAYFIELD_STARTY + (y << 1);

    con_gotoxy(screenx, screeny);
    con_putc(playfield_tiles[tileid][0]);
    con_putc(playfield_tiles[tileid][1]);
    con_gotoxy(screenx, screeny + 1);
    con_putc(playfield_tiles[tileid][2]);
    con_putc(playfield_tiles[tileid][3]);
}
void playfield_draw()
{
    uint8_t x,y;

    for(y = 0; y < FIELDHEIGHT; y++)
    {
        for(x = 0; x < FIELDWIDTH; x++)
        {
            playfield_drawtile(x,y,playfield[x][y]);
        }
    } 
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
}

void playfield_implode_cycle()
{
    uint8_t tempx, tempy, tileid;

    while(queue_hasitems())
    {
        // get coordinates of current queue head item
        tempx = queue_getx();
        tempy = queue_gety();
        if(playfield_checkimplode(tempx, tempy))
        {
            // store old tileID, so we can animate it back to life later
            tileid = playfield[tempx][tempy];
            // Mark all radial items hor/vert from current tempx/tempy coordinate as empty
            playfield_markempty(tempx, tempy);
            // GUI implode
            playfield_gui_implode(tileid);
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

void playfield_gui_implode(uint8_t tileid)
{
    chardefs *ptr = (chardefs *)0xf000; // start of video character definitions
    uint8_t i,b,n;
    uint16_t timer;
    uint8_t (*p)[32];

    playfield_draw();
    // prepare tile ID 0 as buffer
    // currently, an implosion in the field has been marked as empty ID 0, so all we need to do
    // is animate ID 0.
    // First fill ID 0 with old tileid data and then animate it out of existence
    //
    for(n = 0; n < 4; n++)
    {
        // Copy original tile data to ID 0, so it briefly shows on screen
        for(i = 0; i < 4; i++)
        {
            for(b = 0; b < 8; b++)
            {
                ptr[playfield_tiles[0][i]][b] = ptr[playfield_tiles[tileid][i]][b];
            }
        }
        timer = TIMERDELAY2*2;
        while(timer--);
        
        // Reset tile ID 0 to empty space data
        p = playfield_tiledefs;
        for(i = 0; i < 4; i++)
        {
            memcpy((void *)ptr[(playfield_tiles[0][i])], *(p) + i*8, 8);
        }
        timer = TIMERDELAY2;
        while(timer--);
    }
}

void playfield_tiledrop(uint8_t x, uint8_t yfrom, uint8_t tileid)
{
    // drop ONE place down and ANIMATE DROP
    chardefs *ptr = (chardefs *)0xf000; // start of video character definitions
    uint8_t n,i,b;
    uint8_t (*p)[32];
    uint16_t timer;
    uint8_t tshift1, tshift2;

    p = playfield_tiledefs;

    for(i = 0; i < 4; i++)
    {
        n = BUFFERFROM_ID; // copy tileID data to DROP start buffer
        memcpy((void *)ptr[(playfield_tiles[n][i])], *(p + tileid) + i*8, 8);
        n = BUFFERTO_ID; // DROP end buffer will be 'empty' tile
        memcpy((void *)ptr[(playfield_tiles[n][i])], *(p + 0) + i*8, 8);
    }

    // start displaying the buffers on screen
    playfield_drawtile(x, yfrom, BUFFERFROM_ID);
    playfield_drawtile(x, yfrom+1, BUFFERTO_ID);

    // new
    for(i = 0; i < 8; i++)
    {
        ptr[playfield_tiles[BUFFERTO_ID][0]][0] = ptr[playfield_tiles[BUFFERFROM_ID][2]][7];
        ptr[playfield_tiles[BUFFERTO_ID][1]][0] = ptr[playfield_tiles[BUFFERFROM_ID][3]][7];
        tshift1 = ptr[playfield_tiles[BUFFERFROM_ID][0]][7];
        tshift2 = ptr[playfield_tiles[BUFFERFROM_ID][1]][7];
        for(b = 7; b > 0; b--)
        {
            ptr[playfield_tiles[BUFFERTO_ID][0]][b] = ptr[playfield_tiles[BUFFERTO_ID][0]][b-1];
            ptr[playfield_tiles[BUFFERTO_ID][1]][b] = ptr[playfield_tiles[BUFFERTO_ID][1]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][0]][b] = ptr[playfield_tiles[BUFFERFROM_ID][0]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][1]][b] = ptr[playfield_tiles[BUFFERFROM_ID][1]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][2]][b] = ptr[playfield_tiles[BUFFERFROM_ID][2]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][3]][b] = ptr[playfield_tiles[BUFFERFROM_ID][3]][b-1];
        }
        ptr[playfield_tiles[BUFFERFROM_ID][2]][0] = tshift1;
        ptr[playfield_tiles[BUFFERFROM_ID][3]][0] = tshift2;
        ptr[playfield_tiles[BUFFERFROM_ID][0]][0] = 0;
        ptr[playfield_tiles[BUFFERFROM_ID][1]][0] = 0;

        timer=TIMERDELAY;
        while(timer--);        
    }
    for(i = 0; i < 8; i++)
    {
        ptr[playfield_tiles[BUFFERTO_ID][2]][0] = ptr[playfield_tiles[BUFFERTO_ID][0]][7];
        ptr[playfield_tiles[BUFFERTO_ID][3]][0] = ptr[playfield_tiles[BUFFERTO_ID][1]][7];
        tshift1 = ptr[playfield_tiles[BUFFERFROM_ID][2]][7];
        tshift2 = ptr[playfield_tiles[BUFFERFROM_ID][3]][7];
        for(b = 7; b > 0; b--)
        {
            ptr[playfield_tiles[BUFFERTO_ID][0]][b] = ptr[playfield_tiles[BUFFERTO_ID][0]][b-1];
            ptr[playfield_tiles[BUFFERTO_ID][1]][b] = ptr[playfield_tiles[BUFFERTO_ID][1]][b-1];
            ptr[playfield_tiles[BUFFERTO_ID][2]][b] = ptr[playfield_tiles[BUFFERTO_ID][2]][b-1];
            ptr[playfield_tiles[BUFFERTO_ID][3]][b] = ptr[playfield_tiles[BUFFERTO_ID][3]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][2]][b] = ptr[playfield_tiles[BUFFERFROM_ID][2]][b-1];
            ptr[playfield_tiles[BUFFERFROM_ID][3]][b] = ptr[playfield_tiles[BUFFERFROM_ID][3]][b-1];
        }
        ptr[playfield_tiles[BUFFERTO_ID][0]][0] = tshift1;
        ptr[playfield_tiles[BUFFERTO_ID][1]][0] = tshift2;
        ptr[playfield_tiles[BUFFERFROM_ID][2]][0] = 0;
        ptr[playfield_tiles[BUFFERFROM_ID][3]][0] = 0;

        timer=TIMERDELAY;
        while(timer--);        
    }

    // stop displaying the buffers on screen
    playfield_drawtile(x, yfrom, 0); //empty tile
    playfield_drawtile(x, yfrom+1, tileid); // the original tile that was dropped there
}
void playfield_collapse()
{
    // check horizontal missing table, drop # of missing items in each column
    uint8_t x,y,yt,n;
    uint8_t ydrop;
    uint8_t tileid;

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
                    tileid = playfield[x][yt];  // swap with immediate top neigbor
                    playfield[x][y] = tileid;
                    queue_push(x,y);                    // record change to this location, needs check later
                    // later on, need to display this change
                    // display
                    ydrop = yt;
                    while(ydrop < y)    // start GUI drop, one at a time
                    {
                        playfield_tiledrop(x,ydrop,tileid); 
                        ydrop++;
                    }
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
                tileid = random_get();
                playfield[x][y] = tileid;
                queue_push(x,y);
                ydrop = 0;
                while(ydrop < y)
                {
                    playfield_tiledrop(x, ydrop,tileid);
                    ydrop++;
                }
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
    con_gotoxy(0,28);
    if(swap) con_puts("Select direction to swap");
    else     con_puts("ENTER to start swap mode");

    return;
}

void cursor_show(bool swapstatus)
{
    uint8_t tileid = playfield[playfield_cursorx][playfield_cursory];
    chardefs *ptr = (chardefs *)0xf000; // start of video character definitions
    uint8_t i,b;
    uint8_t *brd;

    if(swapstatus) brd = cursor_border_swapped;
    else brd = cursor_border;

    // prepare tile ID 6 as buffer for this 'cursor' location
    for(i = 0; i < 4; i++)
    {
        for(b = 0; b < 8; b++)
        {
            ptr[playfield_tiles[CURSOR_ID][i]][b] = ptr[playfield_tiles[tileid][i]][b] | *brd;
            brd++;
        }
    }
    playfield_drawtile(playfield_cursorx, playfield_cursory, CURSOR_ID);
}
void cursor_hide()
{
    uint8_t tileid = playfield[playfield_cursorx][playfield_cursory];

    playfield_drawtile(playfield_cursorx, playfield_cursory, tileid);
}
void cursor_show_old()
{
    // for now, just inverse the current position
    char o;

    o = playfield_getchar(playfield[playfield_cursorx][playfield_cursory]);
    o = o | 128;

    con_gotoxy(PLAYFIELD_STARTX+playfield_cursorx, PLAYFIELD_STARTY+playfield_cursory);
    con_putc(o);
}
void cursor_hide_old()
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
            else playfield_cursory = FIELDHEIGHT - 1;
            break;
        case KEY_LEFT:
            if(playfield_cursorx > 0) playfield_cursorx--;
            else playfield_cursorx = FIELDWIDTH - 1;
            break;
        case KEY_DOWN:
            if(playfield_cursory < FIELDHEIGHT-1) playfield_cursory++;
            else playfield_cursory = 0;
            break;
        case KEY_RIGHT:
            if(playfield_cursorx < FIELDWIDTH-1) playfield_cursorx++;
            else playfield_cursorx = 0;
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
