#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "console.h"
#include "random.h"
#include "queue.h"
#include "playfield.h"

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

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

void print_cursorposition()
{
    char msg[40];

    con_gotoxy(0,0);
    con_puts("X:        ");
    con_gotoxy(2,0);
    itoa(playfield_cursorx, msg, 10);
    con_puts(msg);
    con_gotoxy(0,1);
    con_puts("Y:        ");
    con_gotoxy(2,1);
    itoa(playfield_cursory, msg, 10);
    con_puts(msg);
}

void print_sim(uint8_t number)
{
    char msg[40];

    con_gotoxy(0,2);
    con_puts("Sim:      ");
    con_gotoxy(4,2);
    itoa(number, msg, 10);
    con_puts(msg);
}

int main()
{
    char key;
    bool swap = false;
    uint8_t test;

    random_init();
    queue_init();
    con_init();

    draw_borders();
    playfield_init_tiles();
/*
    playfield_drawtile(0,0,0);
    playfield_drawtile(0,2,1);
    playfield_drawtile(0,4,2);
    playfield_drawtile(0,6,3);
    playfield_drawtile(0,8,4);
    playfield_drawtile(0,10,5);
    while(1);
    con_gotoxy(0,0);
    con_putc(128);
    con_putc(129);
    con_gotoxy(0,1);
    con_putc(130);
    con_putc(131);
    con_gotoxy(1,2);
    con_putc(132);
    con_putc(133);
    con_gotoxy(1,3);
    con_putc(134);
    con_putc(135);
    con_gotoxy(2,4);
    con_putc(136);
    con_putc(137);
    con_gotoxy(2,5);
    con_putc(138);
    con_putc(139);
    con_gotoxy(3,6);
    con_putc(140);
    con_putc(141);
    con_gotoxy(3,7);
    con_putc(142);
    con_putc(143);
    con_gotoxy(4,8);
    con_putc(144);
    con_putc(145);
    con_gotoxy(4,9);
    con_putc(146);
    con_putc(147);
    con_gotoxy(5,10);
    con_putc(148);
    con_putc(149);
    con_gotoxy(5,11);
    con_putc(150);
    con_putc(151);
    while(1);
*/

    playfield_init();
    playfield_draw();
    cursor_show();
    while(1);

    display_swap_message(swap);
    while(1)
    {
        print_cursorposition();
        test = playfield_checkimplode(playfield_cursorx, playfield_cursory);
        print_sim(test);

        key = con_getc();
        switch(key)
        {
            case KEY_ENTER:   // LF / ENTER
                swap = !swap;
                display_swap_message(swap);
                break;
            case 0x1b:
                swap = false;
                display_swap_message(swap);
                break;
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                if(swap)
                {
                    playfield_swap(key);
                    swap = false;
                    display_swap_message(swap);
                }
                else
                {
                    cursor_hide();
                    cursor_move(key);
                    cursor_show();
                }
                break;
            default:
                break;
        }
    }
}
