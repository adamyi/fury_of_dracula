////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.h: an interface to a Map data type
//
// 2017-11-30   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

#include "places.h"

typedef struct edge {
    enum location_id start, end;
    enum transport_type type;
} edge;

// graph representation is hidden
typedef struct map *Map;

// operations on graphs
Map newMap (void);
void disposeMap (Map);
void showMap (Map);
int numV (Map);
int numE (Map, enum transport_type);

#endif // !defined(FOD__MAP_H_)
