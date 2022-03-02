#include <string.h>
#include <stdio.h>
#include "console.h"

void definechar(unsigned char x, unsigned char *array)
{
    chardefs *ptr = (chardefs *)0xf000; // base pointer to character definitions 'array'
    memcpy((void *)ptr[x],array,8);
    return;
}

void wait(unsigned char ticks)
{
    *((unsigned char*)0x00ff) = 0;   // reset timer
    while(*((unsigned char*)0x00ff) < ticks);
    return;
}

void shiftup(unsigned char from, unsigned char to)
{
    unsigned char i;
    chardefs *ptr = (chardefs *)0xf000;

    for(i = 0; i < 7; i++) 
    {
        *(ptr[to] + i) = *(ptr[to] + i + 1);    // shift one up
    }
    *(ptr[to] + 7) = *(ptr[from]);
    for(i = 0; i < 7; i++)
    {
        *(ptr[from] + i) = *(ptr[from] + i + 1);
    }
    *(ptr[from] + 7) = 0;
    return;
}

void shiftdown(unsigned char from, unsigned char to)
{
    unsigned char i;
    chardefs *ptr = (chardefs *)0xf000;

    for(i = 7; i > 0; i--)
    {
        *(ptr[to] + i) = *(ptr[to] + i - 1);    // shift one down
    }
    *(ptr[to]) = *(ptr[from] + 7);
    for(i = 7; i > 0; i--)
    {
        *(ptr[from] + i) = *(ptr[from] + i - 1); // shift one down
    }
    *(ptr[from]) = 0;
}

void main(void)
{
    unsigned char block[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    unsigned char empty[] = {0,0,0,0,0,0,0,0};
    unsigned char y,i;
    con_init();

    memcpy(block,(void *)(((chardefs *)0xf000)[0]),8);    
    definechar(2, empty);  
    while(1)
    {
        for(y = 29; y > 0; y--)
        {
            definechar(0,block);
            definechar(1,empty);

            con_gotoxy(1,y);
            con_putc(0);
            con_gotoxy(1,y-1);
            con_putc(1);
            for(i = 0; i < 8; i++)
            {
                wait(1);
                shiftup(0,1);
            }
            con_gotoxy(1,y);
            con_putc(2);
        }
        for(y = 0; y < 29; y++)
        {
            definechar(0,block);
            definechar(1,empty);

            con_gotoxy(1,y);
            con_putc(0);
            con_gotoxy(1,y+1);
            con_putc(1);
            for(i = 0; i < 8; i++)
            {
                wait(1);
                shiftdown(0,1);
            }
            con_gotoxy(1,y);
            con_putc(2);
        }
    } 
    while(1);
    return;
}
