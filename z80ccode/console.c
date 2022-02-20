#include "console.h"

unsigned char *console;

void con_init()
{
    console = videostart;
    return;
}

void con_gotoxy(unsigned char x, unsigned char y)
{
    console = videostart + (y*SCREENWIDTH) + x;
    return;
}

void con_cls()
{
    for(console = videostart; console < videoend; console++)
    {
        *console = ' ';
    }
    console = videostart;
    return;
}
void putc(char c)
{
    *console = c;
    console++;
    if(console == videoend) 
        console = videostart;
    return;
}

void puts(const char *s)
{
    while(*s)
    {
        putc(*s);
        s++;
    }
    return;
}
