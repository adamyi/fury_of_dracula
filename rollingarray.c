// Rolling Array Data Type
// written by Adam Yi <i@adamyi.com>

#include "rollingarray.h"
#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

static inline size_t get_ra_ind(size_t start, size_t capacity, size_t ind) {
  ind += start;
  if (ind >= capacity) ind -= capacity;
  return ind;
}

rollingarray_t *new_rollingarray(size_t capacity) {
  rollingarray_t *new = malloc(sizeof(rollingarray_t));
  new->capacity = capacity;
  new->value = malloc(capacity * sizeof(rollingarray_t));
  memset(new->value, 0, capacity);
  new->size = new->start = 0;
  return new;
}

void destroy_rollingarray(rollingarray_t *ra) {
  free(ra->value);
  free(ra);
}

ra_item_t rollingarray_get_item(rollingarray_t *ra, size_t ind) {
  if (ind >= ra->size) return RA_UNKNOWN_ITEM;
  return ra->value[get_ra_ind(ra->start, ra->capacity, ind)];
}

void rollingarray_add_item(rollingarray_t *ra, ra_item_t val) {
  if (ra->size < ra->capacity)
    ra->value[get_ra_ind(ra->start, ra->capacity, ra->size++)] = val;
  else
    ra->value[ra->start++] = val;
}

int rollingarray_size(rollingarray_t *ra) { return ra->size; }

size_t rollingarray_to_array(rollingarray_t *ra, ra_item_t arr[]) {
  for (int i = ra->start, j = 0; j < ra->size;
       i = (i + 1 == ra->capacity ? 0 : i + 1), j++)
    arr[j] = ra->value[i];
  return ra->size;
}
