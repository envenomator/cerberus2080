#include "console.h"

int main()
{
    const char *welcome = "Hello world!";
    //char *ptr;
    
    con_init();
    con_gotoxy(10,10);
    puts(welcome);
    con_cls();
    while(1);
}
