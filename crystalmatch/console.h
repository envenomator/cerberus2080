#ifndef   CONSOLE_H
#define   CONSOLE_H

#define SCREENWIDTH  40
#define SCREENHEIGHT 30
#define BIOS_OUTBOXFLAG     (unsigned char*)0x0200
#define BIOS_OUTBOXDATA     (unsigned char*)0x0201
#define VIDEOSTART          (unsigned char*)0xf800
#define VIDEOEND            (VIDEOSTART + SCREENWIDTH*SCREENHEIGHT)

#define KEY_UP      0x0b
#define KEY_DOWN    0x0a
#define KEY_LEFT    0x08
#define KEY_RIGHT   0x15
#define KEY_ESC     0x0b
#define KEY_ENTER   0x0d
typedef volatile unsigned char chardefs[8];             // use as pointer to character definition memory area (0xf000)
                                                        // ex: chardefs *ptr = (chardefs *)0xf000;
void con_init();                                        // needed before any console IO
void con_gotoxy(unsigned char x, unsigned char y);
void con_cls();
void con_putc(char c);
void con_puts(const char *s);
char con_getc();                                        // blocked wait

#endif
