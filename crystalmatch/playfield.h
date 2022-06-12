#ifndef   PLAYFIELD_H
#define   PLAYFIELD_H

#define FIELDWIDTH 20
#define FIELDHEIGHT 15
#define PLAYFIELD_STARTX    10
#define PLAYFIELD_STARTY    5

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

#endif
