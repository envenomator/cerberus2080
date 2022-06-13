#ifndef   PLAYFIELD_H
#define   PLAYFIELD_H

#include <stdbool.h>
#include <stdint.h>

#define FIELDWIDTH 20
#define FIELDHEIGHT 15
#define PLAYFIELD_STARTX    10
#define PLAYFIELD_STARTY    5

extern uint8_t playfield_cursorx;
extern uint8_t playfield_cursory;

void playfield_init();
void playfield_init_random();
void playfield_load();
char playfield_getchar(uint8_t id);
void playfield_draw();
void playfield_swap(char key);
void draw_borders();
void display_swap_message(bool swap);
void cursor_show();
void cursor_hide();
void cursor_move(char key);
bool playfield_checkimplode(uint8_t x, uint8_t y);
#endif
