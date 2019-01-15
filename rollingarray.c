// Rolling Array Data Type
// written by Adam Yi <i@adamyi.com>

#include "rollingarray.h"
#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static inline size_t get_ra_ind(size_t start, size_t capacity, size_t ind) {
  ind += start;
  if (ind >= capacity) ind -= capacity;
  return ind;
}

static inline size_t get_ra_ind_backwards(size_t start, size_t capacity,
                                          size_t size, size_t ind) {
  size_t last_ind = get_ra_ind(start, capacity, size - 1);
  // avoid negative number because all are unsigned
  if (last_ind < ind)
    return capacity + last_ind - ind;
  return last_ind - ind;
}

rollingarray_t *new_rollingarray(size_t capacity) {
  rollingarray_t *new = malloc(sizeof(rollingarray_t));
  new->capacity = capacity;
  new->value = malloc(capacity * sizeof(ra_item_t));
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

ra_item_t rollingarray_last_item(rollingarray_t *ra) {
  return rollingarray_get_item_backwards(ra, 0);
}

ra_item_t rollingarray_get_item_backwards(rollingarray_t *ra, size_t ind) {
  if (ind >= ra->size) return RA_UNKNOWN_ITEM;
  return ra
      ->value[get_ra_ind_backwards(ra->start, ra->capacity, ra->size, ind)];
}

void rollingarray_add_item(rollingarray_t *ra, ra_item_t val) {
  if (ra->size < ra->capacity)
    ra->value[get_ra_ind(ra->start, ra->capacity, ra->size++)] = val;
  else
    ra->value[ra->start++] = val;
  if (ra->start == ra->capacity) ra->start = 0;
}

size_t rollingarray_size(rollingarray_t *ra) { return ra->size; }

size_t rollingarray_to_array(rollingarray_t *ra, ra_item_t arr[],
                             bool reversed) {
  if (reversed) {
    for (int i = (int)ra->start, j = (int)ra->size - 1; j >= 0;
         i = (i + 1 == ra->capacity ? 0 : i + 1), j--)
      arr[j] = ra->value[i];
  } else {
    for (int i = (int)ra->start, j = 0; j < ra->size;
         i = (i + 1 == (int)ra->capacity ? 0 : i + 1), j++)
      arr[j] = ra->value[i];
  }
  for (int i = ra->size; i < ra->capacity; i++) arr[i] = RA_UNKNOWN_ITEM;
  return ra->size;
}

bool rollingarray_has_item(rollingarray_t *ra, ra_item_t item) {
  size_t endind = (ra->size < ra->capacity) ? ra->size : ra->capacity;
  for (size_t i = 0; i < endind; i++) {
    if (ra->value[i] == item) return true;
  }
  return false;
}
