#include <string.h>
#include <stdio.h>
#include "console.h"

extern void banner();

int main()
{
    int n;
    char c;

    con_init();
    con_cls();

    con_gotoxy(9,10);
    banner();
    for(n = 0; n < 40; n++)
    {
        con_gotoxy(n,0);
        con_putc('x');
        con_gotoxy(n,29);
        con_putc('x');
    }
    for(n = 1; n < 29; n++)
    {
        con_gotoxy(0,n);
        con_putc('x');
        con_gotoxy(39,n);
        con_putc('x');
    }

    while(1)
    {
        c = con_getc();
        con_gotoxy(20,20);
        con_putc(c);
    }

    while(1);
    return (0);
}
