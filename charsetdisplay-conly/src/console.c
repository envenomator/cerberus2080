#include "console.h"

unsigned char *console;

void con_init()
{
    console = VIDEOSTART;
    return;
}

void con_gotoxy(unsigned char x, unsigned char y)
{
    console = VIDEOSTART + (y*SCREENWIDTH) + x;
    return;
}

void con_cls()
{
    for(console = VIDEOSTART; console < VIDEOEND; console++)
    {
        *console = ' ';
    }
    console = VIDEOSTART;
    return;
}
void con_putc(char c)
{
    *console = c;
    console++;
    if(console == VIDEOEND) 
        console = VIDEOSTART;
    return;
}

void con_puts(const char *s)
{
    while(*s)
    {
        con_putc(*s);
        s++;
    }
    return;
}

char con_getc()
{
    while(*(BIOS_OUTBOXFLAG) == 0); // blocked wait for the mailbox flag
    *(BIOS_OUTBOXFLAG) = 0;         // acknowlege reception
    return *(BIOS_OUTBOXDATA);      // return the data slot
}
