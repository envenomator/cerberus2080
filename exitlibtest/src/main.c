#include "console.h"

int main(void)
{
    con_init();
    con_cls();
    con_puts("Press any key to exit");
    con_getc();

    return 0;
}
