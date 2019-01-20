////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "ac_log.h"
#include "ac_memory.h"

#include "game.h"
#include "hunter_view.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"

typedef struct hunter_view {
  struct _game_view *gv;
} hunter_view;

hunter_view *hv_new(char *past_plays, player_message messages[]) {
  hunter_view *new = ac_malloc(sizeof *new, "new hunter view");
  new->gv = _gv_new(past_plays, messages, false);

  return new;
}

void hv_drop(hunter_view *hv) {
  _gv_drop(hv->gv);
  free(hv);
}

round_t hv_get_round(hunter_view *hv) { return _gv_get_round(hv->gv); }

enum player hv_get_player(hunter_view *hv) { return _gv_get_player(hv->gv); }

int hv_get_score(hunter_view *hv) { return _gv_get_score(hv->gv); }

int hv_get_health(hunter_view *hv, enum player player) {
  return _gv_get_health(hv->gv, player);
}

location_t hv_get_location(hunter_view *hv, enum player player) {
  location_t loc = _gv_get_real_location(hv->gv, player);
  if (loc < MIN_MAP_LOCATION || loc > MAX_MAP_LOCATION)
    return _gv_get_location(hv->gv, player);
  return loc;
}

void hv_get_trail(hunter_view *hv, enum player player,
                  location_t trail[TRAIL_SIZE]) {
  _gv_get_move_history(hv->gv, player, trail);
}

location_t *hv_get_dests(hunter_view *hv, size_t *n_locations, bool road,
                         bool rail, bool sea) {
  return hv_get_dests_player(hv, n_locations, hv_get_player(hv), road, rail,
                             sea);
}

location_t *hv_get_dests_player(hunter_view *hv, size_t *n_locations,
                                enum player player, bool road, bool rail,
                                bool sea) {
  if (player == PLAYER_DRACULA)
    return _gv_get_connections_with_trail(hv->gv, n_locations,
                                          hv_get_location(hv, player), player,
                                          hv_get_round(hv), road, rail, sea);
  else
    return _gv_get_connections(hv->gv, n_locations,
                               hv_get_location(hv, player), player,
                               hv_get_round(hv), road, rail, sea);
}
