#include <stdbool.h>

#include "mapdata.h"
#include "places.h"

#ifndef FOD__MAP_H_
#define FOD__MAP_H_

inline char get_connection_type_mask(transport_t t);
bool isConnectedVia(location_t v, location_t w, char tspt);
bool isConnected(location_t v, location_t w);
struct adj_connection *getConnections(location_t v);
location_t special_location_find_by_abbrev(char *abbrev);

#endif  // !defined (FOD__MAP_H_)
