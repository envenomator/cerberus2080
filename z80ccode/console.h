#ifndef   CONSOLE_H
#define   CONSOLE_H

#define SCREENWIDTH  40
#define SCREENHEIGHT 30
#define videostart  (unsigned char*)0xf800
#define videoend    (videostart + SCREENWIDTH*SCREENHEIGHT)

void con_init();
void con_gotoxy(unsigned char x, unsigned char y);
void con_cls();
void putc(char c);
void puts(const char *s);

#endif

