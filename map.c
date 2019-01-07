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
  return ((ADJMATRIX[v][w] & tspt) == tspt);
}

bool isConnected(location_t v, location_t w) {
  return isConnectedVia(v, w, 7);  // 111
}

struct adj_connection *getConnections(location_t v) {
  return ADJLIST[v];
}
