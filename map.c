#include <assert.h>

#include "map.h"
#include "mapdata.h"

inline char get_connection_type_mask(transport_t t) {
  assert(t >= MIN_TRANSPORT && t <= MAX_TRANSPORT);
  switch (t) {
    case ROAD:
      return 1;  // 001
    case RAIL:
      return 2;  // 010
    case BOAT:
      return 4;  // 100
    default:
      __builtin_unreachable();
  }

  __builtin_unreachable();
}

bool isConnectedVia(location_t v, location_t w, char tspt) {
  return ((ADJMATRIX[v][w] & tspt) > 0);
}

bool isConnected(location_t v, location_t w) {
  return isConnectedVia(v, w, 7);  // 111
}

struct adj_connection *getConnections(location_t v) {
  return ADJLIST[v];
}

location_t special_location_find_by_abbrev(char *abbrev) {
  if (abbrev[0] == 'C' && abbrev[1] == '?') return CITY_UNKNOWN;
  if (abbrev[0] == 'S' && abbrev[1] == '?') return SEA_UNKNOWN;
  if (abbrev[0] == 'H' && abbrev[1] == 'I') return HIDE;
  if (abbrev[0] == 'T' && abbrev[1] == 'P') return TELEPORT;
  if (abbrev[0] == 'D') {
    if (abbrev[1] >= '1' && abbrev[1] <= '5')
      return DOUBLE_BACK_1 + abbrev[1] - '1';
    return NOWHERE;
  }
  return NOWHERE;
}
