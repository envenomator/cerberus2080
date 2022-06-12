#ifndef   QUEUE_H
#define   QUEUE_H

#include "playfield.h"

#define QUEUEDEPTH  FIELDWIDTH*FIELDHEIGHT*2
void queue_init();
void queue_add(uint8_t x, uint8_t y);
void queue_delete();
bool queue_isempty();
bool queue_hasitems();

#endif
