#include <string.h>
#include <stdio.h>
#include "console.h"

int main()
{
    char row,col;
    char c;
    char str[8];
    unsigned int num = 0, max=128;

    con_init();

    while(1)
    {
        con_cls();
        for(col = 0; col < 5; col++)
        {
            for(row = 0; row < 30; row++)
            {
                if(num < max)
                {
                    snprintf(str,40,"0x%02x ",num); 
                    con_gotoxy(col*8,row);
                    con_puts(str);
                    con_putc(num);  // also print out character 0
                    num = num + 1;
                }
            }
        }
        if(num == 128)
        {
            max = 256;
        }
        else
        {
            num = 0;
            max = 128;
        }
        c = con_getc(); // wait for any key to display next set
    }

    return (0);
}
