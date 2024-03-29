#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"
#include "random.h"

const uint8_t random_list[] = {1, 5, 2, 6, 2, 5, 1, 2, 3, 2, 4, 4, 2, 6, 5, 5, 6, 4, 1, 2, 1, 3, 3, 2, 3, 6, 2, 1, 1, 6, 1, 1, 1, 1, 4, 4, 3, 6, 1, 1, 3, 3, 6, 6, 4, 4, 2, 1, 4, 1, 6, 3, 4, 2, 1, 1, 3, 3, 4, 1, 4, 2, 5, 2, 2, 5, 1, 1, 3, 1, 6, 1, 1, 6, 5, 5, 3, 4, 6, 4, 5, 4, 1, 3, 5, 2, 3, 5, 2, 3, 4, 1, 2, 5, 6, 5, 6, 6, 2, 4, 4, 2, 5, 4, 6, 1, 5, 6, 4, 2, 2, 3, 5, 3, 5, 4, 1, 3, 3, 4, 5, 3, 1, 6, 6, 1, 2, 6, 3, 4, 2, 5, 4, 5, 3, 5, 5, 6, 4, 5, 1, 6, 5, 3, 5, 5, 4, 3, 1, 1, 4, 2, 5, 2, 1, 2, 5, 2, 1, 3, 5, 6, 3, 2, 2, 6, 2, 3, 5, 2, 2, 5, 5, 5, 3, 6, 4, 4, 2, 2, 5, 2, 2, 6, 6, 3, 4, 4, 2, 2, 5, 5, 4, 2, 2, 3, 5, 6, 4, 2, 4, 2, 6, 5, 3, 4, 4, 3, 1, 3, 4, 1, 3, 3, 1, 6, 1, 2, 2, 1, 2, 1, 2, 2, 4, 6, 4, 5, 4, 4, 4, 6, 4, 3, 5, 3, 6, 4, 3, 6, 3, 1, 1, 4, 3, 5, 4, 5, 2, 5, 2, 1, 1, 3, 3, 3};
uint8_t random_id;

void random_init()
{
    random_id = 0;
}
uint8_t random_get()
{
    uint8_t ret;

    // return next random number. roll over on 8-bit, so max 256 values
    ret = random_list[random_id];
    if(random_id == 255) random_id = 0;    
    else random_id++;

    return ret;
}
