#include "console.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *console;
uint32_t con_timer_counter;
uint16_t con_timer_ms;

void con_init()
{
    console = VIDEOSTART;
    return;
}

void con_init_timer(uint8_t init)
{
    con_timer_counter = 0;
    con_timer_ms = init;
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

void print_timer();

char con_getc_timer(uint16_t threshold)
{
    while(*(BIOS_OUTBOXFLAG) == 0)
    {
        con_timer_counter++;
        if(con_timer_counter > threshold)
        {
            con_timer_counter = 0;
            con_timer_ms--; 
            if(con_timer_ms == 0)
            {
                con_timer_ms = 0;
                print_timer();
                *(BIOS_OUTBOXDATA) = 'Q';
                break;
            }
        }
        print_timer();
    } // blocked wait for the mailbox flag
    *(BIOS_OUTBOXFLAG) = 0;         // acknowlege reception
    return *(BIOS_OUTBOXDATA);      // return the data slot
}

char con_getc()
{
    while(*(BIOS_OUTBOXFLAG) == 0); // blocked wait for the mailbox flag
    *(BIOS_OUTBOXFLAG) = 0;         // acknowlege reception
    return *(BIOS_OUTBOXDATA);      // return the data slot
}

void con_exit()
{
    *(BIOS_INBOXFLAG) = 0x7F;       // request to exit();
    while(1);
}
// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
  {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    n = strlen(buffer);
    switch(n)
    {
        case 1:
            strcat(buffer,"  ");
            i += 2;
            break;
        case 2:
            strcat(buffer," ");
            i += 1;
            break;
        default:
            break;
    }
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

void print_timer()
{
    char msg[40];
    
    con_gotoxy(34,10);
    itoa(con_timer_ms, msg, 10);
    con_puts(msg);
}   
