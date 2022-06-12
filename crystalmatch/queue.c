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
uint16_t queue_head;
uint16_t queue_tail;
uint16_t queue_items;

void queue_init()
{
    // start out with an empty queue
    queue_head = 0;
    queue_tail = 0;
    queue_items = 0;
}
void queue_add(uint8_t x, uint8_t y)
{
    queue_items++;
    queue_tail++;
    if(queue_tail == QUEUEDEPTH) queue_tail = 0;
    queue[queue_tail].x = x;
    queue[queue_tail].y = y;
}
void queue_delete()
{
    queue_items--;
    queue_head++;
    if(queue_head == QUEUEDEPTH) queue_head = 0;
}
bool queue_isempty()
{
    return queue_items == 0;
}
bool queue_hasitems()
{
    return queue_items > 0;
}

