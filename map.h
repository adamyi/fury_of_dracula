////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.h: an interface to a Map data type
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdbool.h>
#include <stdlib.h>

#include "places.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

typedef struct edge {
  location_t start, end;
  transport_t type;
} edge;

// graph representation is hidden
typedef struct map *Map;

/** Create a new Map. */
Map map_new(void);
/** Release resources associated with a Map. */
void map_drop(Map);
/** Print a Map to `stdout`. */
void map_show(Map);
/** Get the number of vertices. */
size_t map_nv(Map);
/** Get the number of edges. */
size_t map_ne(Map, transport_t);

#endif  // !defined(FOD__MAP_H_)
