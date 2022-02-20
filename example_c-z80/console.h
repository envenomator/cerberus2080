#ifndef   CONSOLE_H
#define   CONSOLE_H

#define SCREENWIDTH  40
#define SCREENHEIGHT 30
#define BIOS_OUTBOXFLAG     (unsigned char*)0x0200
#define BIOS_OUTBOXDATA     (unsigned char*)0x0201
#define VIDEOSTART          (unsigned char*)0xf800
#define VIDEOEND            (VIDEOSTART + SCREENWIDTH*SCREENHEIGHT)

void con_init();                                        // needed before any console IO
void con_gotoxy(unsigned char x, unsigned char y);
void con_cls();
void con_putc(char c);
void con_puts(const char *s);
char con_getc();                                        // blocked wait

#endif
