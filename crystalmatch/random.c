#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "console.h"
#include "random.h"

uint8_t random_list[] = {3, 5, 2, 5, 3, 1, 4, 5, 3, 3, 3, 3, 3, 2, 5, 4, 5, 3, 5, 5, 5, 4, 5, 1, 5, 1, 5, 5, 5, 5, 3, 5, 3, 5, 2, 4, 4, 1, 2, 5, 4, 2, 3, 4, 2, 2, 1, 1, 1, 4, 1, 5, 1, 2, 3, 5, 1, 1, 4, 2, 5, 1, 4, 5, 5, 4, 5, 2, 2, 3, 1, 5, 5, 5, 4, 5, 4, 2, 3, 4, 2, 2, 2, 2, 5, 3, 5, 4, 2, 4, 5, 2, 2, 1, 3, 2, 2, 1, 4, 3, 5, 2, 4, 4, 2, 1, 1, 4, 5, 1, 4, 1, 3, 4, 4, 2, 5, 5, 1, 2, 3, 3, 3, 3, 3, 3, 4, 3, 1, 1, 4, 4, 3, 4, 1, 5, 3, 4, 2, 3, 2, 3, 3, 2, 2, 2, 1, 5, 4, 3, 4, 2, 2, 2, 2, 1, 3, 4, 3, 3, 1, 2, 2, 2, 3, 2, 1, 1, 5, 5, 2, 1, 1, 3, 4, 3, 5, 2, 1, 5, 3, 3, 5, 5, 5, 5, 3, 1, 4, 3, 1, 1, 4, 5, 5, 1, 4, 3, 4, 4, 2, 4, 2, 2, 1, 2, 3, 1, 2, 2, 1, 2, 1, 4, 2, 3, 3, 3, 5, 4, 5, 3, 5, 4, 5, 3, 5, 1, 2, 2, 3, 4, 5, 5, 3, 4, 3, 4, 5, 1, 4, 1, 1, 5, 5, 1, 3, 4, 4, 5, 5, 5, 5, 2, 3, 5}; 
uint8_t random_id;

void random_init()
{
    random_id = 0;
}
uint8_t random_get()
{
    // return next random number. roll over on 8-bit, so max 256 values
    return random_list[random_id++];
}

