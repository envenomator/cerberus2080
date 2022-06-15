#ifndef   QUEUE_H
#define   QUEUE_H

#include "playfield.h"

#define QUEUEDEPTH  FIELDWIDTH*FIELDHEIGHT
void queue_init();
void queue_push(uint8_t x, uint8_t y);
void queue_pop();           // remove head/front of the queue
bool queue_isempty();
bool queue_hasitems();
uint8_t queue_getx();
uint8_t queue_gety();
#endif
