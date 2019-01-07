// Rolling Array Data Type
// written by Adam Yi <i@adamyi.com>

#include <stddef.h>
#include "places.h"

#ifndef ROLLINGARRAY_H_
#define ROLLINGARRAY_H_

#define RA_UNKNOWN_ITEM -1

typedef location_t ra_item_t;

typedef struct rollingarray {
  size_t capacity;
  ra_item_t *value;
  size_t start, size;
} rollingarray_t;

rollingarray_t *new_rollingarray(size_t capacity);
void destroy_rollingarray(rollingarray_t *ra);
ra_item_t rollingarray_get_item(rollingarray_t *ra, size_t ind);
void rollingarray_add_item(rollingarray_t *ra, ra_item_t val);
int rollingarray_size(rollingarray_t *ra);
size_t rollingarray_to_array(rollingarray_t *ra, ra_item_t arr[]);

#endif  // !defined (ROLLINGARRAY_H_)
