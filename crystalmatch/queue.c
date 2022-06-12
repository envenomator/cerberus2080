#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "queue.h"

// Queue with to-be-checked coordinates
struct coordinate
{
    uint8_t x;
    uint8_t y;
};
struct coordinate queue[QUEUEDEPTH];
uint16_t queue_headnumber;
uint16_t queue_tailnumber;
uint16_t queue_items;

void queue_init()
{
    // start out with an empty queue
    queue_headnumber = 0;
    queue_tailnumber = 0;
    queue_items = 0;
}
void queue_add(uint8_t x, uint8_t y)
{
    queue_items++;
    queue_tailnumber++;
    if(queue_tailnumber == QUEUEDEPTH) queue_tailnumber = 0;
    queue[queue_tailnumber].x = x;
    queue[queue_tailnumber].y = y;
}
void queue_delete()
{
    queue_items--;
    queue_headnumber++;
    if(queue_headnumber == QUEUEDEPTH) queue_headnumber = 0;
}
bool queue_isempty()
{
    return queue_items == 0;
}
bool queue_hasitems()
{
    return queue_items > 0;
}
