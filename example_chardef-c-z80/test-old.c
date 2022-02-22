#include "console.h"

int main()
{
    const char welcome[] = "Press any key to CLS";
    
    con_init();
    con_gotoxy(10,10);
    con_puts(welcome);
    con_getc();
    con_cls();
    while(1);
}
