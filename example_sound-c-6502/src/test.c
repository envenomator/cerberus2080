#include <string.h>
#include <stdio.h>
#include "console.h"

#define BIOS_INBOXFLAG      0x0202
#define BIOS_INBOXDATA      0x0203

struct khz
{
    unsigned int frequency;
    unsigned int duration;
};

int main()
{
    struct khz tone;
    tone.frequency = 2000;
    tone.duration = 10;

    con_init();

    *((unsigned int *)BIOS_INBOXDATA) = (unsigned int)&tone;
    *((unsigned char*)BIOS_INBOXFLAG) = 0x01;   // beep command
    
    while(*((unsigned char*)BIOS_INBOXFLAG) != 0x01);    // wait until command is done
    
    con_gotoxy(1,28);
    con_puts("Press any key - return to BIOS");
    con_getc();
    return (0);
}
