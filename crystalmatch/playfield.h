#ifndef   PLAYFIELD_H
#define   PLAYFIELD_H

#include <stdbool.h>
#include <stdint.h>

#define FIELDWIDTH 10
#define FIELDHEIGHT 5
#define PLAYFIELD_STARTX    10
#define PLAYFIELD_STARTY    5

extern uint8_t playfield_cursorx;
extern uint8_t playfield_cursory;

void playfield_init();
void playfield_init_random();
void playfield_load();
char playfield_getchar(uint8_t id);
void playfield_draw();
void playfield_drawtile(uint8_t screenx, uint8_t screeny, uint8_t tileid);  // draw tileid 0..5 to specific screen x/y coordinate
void playfield_swap(char key);
void draw_borders();
void display_swap_message(bool swap);
void cursor_show();
void cursor_hide();
void cursor_move(char key);
bool playfield_checkimplode(uint8_t x, uint8_t y);  // only check if a location NEEDS implosion, no actual work
void playfield_implode_cycle();                     // internal function - all the implosion work goes here
void playfield_markempty(uint8_t x, uint8_t y);
void playfield_gui_implode(uint8_t tileid);           // internal function
void playfield_collapse();                          // internal function - will trigger additional push of checks to the queue
void playfield_init_tiles();
#endif
