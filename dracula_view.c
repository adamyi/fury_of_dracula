////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "ac_log.h"
#include "ac_memory.h"

#include "dracula_view.h"
#include "game.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"

typedef struct dracula_view {
  _GameView gv;
} dracula_view;

dracula_view *dv_new(char *past_plays, player_message messages[]) {
  dracula_view *new = ac_malloc(sizeof *new, "new dracula view");
  new->gv = _gv_new(past_plays, messages, true);

  return new;
}

void dv_drop(dracula_view *dv) {
  _gv_drop(dv->gv);
  free(dv);
}

round_t dv_get_round(dracula_view *dv) { return _gv_get_round(dv->gv); }

int dv_get_score(dracula_view *dv) { return _gv_get_score(dv->gv); }

int dv_get_health(dracula_view *dv, enum player player) {
  return _gv_get_health(dv->gv, player);
}

location_t dv_get_location(dracula_view *dv, enum player player) {
  return _gv_get_real_location(dv->gv, player);
}

void dv_get_player_move(dracula_view *dv, enum player player, location_t *start,
                        location_t *end) {
  location_t trail[TRAIL_SIZE];
  _gv_get_move_history(dv->gv, player, trail);

  *end = trail[0];
  *start = trail[1];
}

void dv_get_locale_info(dracula_view *dv, location_t where, int *n_traps,
                        int *n_vamps) {
  _gv_get_locale_info(dv->gv, where, n_traps, n_vamps);
}

void dv_get_trail(dracula_view *dv, enum player player,
                  location_t trail[TRAIL_SIZE]) {
  if (player == PLAYER_DRACULA)
    _gv_get_location_history(dv->gv, player, trail);
  else
    _gv_get_move_history(dv->gv, player, trail);
}

location_t *dv_get_possible_moves(dracula_view *dv, size_t *n_locations) {
  if (dv_get_round(dv) == 0) {
    *n_locations = 70;
    location_t *ret = ac_malloc(70 * sizeof(location_t), "dv_get_possible_moves for round 0");
    for (int i = MIN_MAP_LOCATION, j = 0; i <= MAX_MAP_LOCATION; i++, j++) {
        if (i == HOSPITAL_LOCATION) {
            j--;
            continue;
        }
        ret[j] = i;
    }
    return ret;
  }
  return _gv_do_get_connections(
      _gv_get_player_class(dv->gv, PLAYER_DRACULA), n_locations,
      dv_get_location(dv, PLAYER_DRACULA), PLAYER_DRACULA, dv_get_round(dv),
      true, false, true, true, false, true);
}

location_t *dv_get_dests(dracula_view *dv, size_t *n_locations, bool road,
                         bool sea) {
  return _gv_get_connections_with_trail(
      dv->gv, n_locations, dv_get_location(dv, PLAYER_DRACULA), PLAYER_DRACULA,
      dv_get_round(dv), road, false, sea);
}

location_t *dv_get_dests_player(dracula_view *dv, size_t *n_locations,
                                enum player player, bool road, bool rail,
                                bool sea) {
  if (player == PLAYER_DRACULA) return dv_get_dests(dv, n_locations, road, sea);
  return _gv_get_connections(dv->gv, n_locations, dv_get_location(dv, player),
                             player, dv_get_round(dv), road, rail, sea);
}

player_t *dv_get_player_class(DraculaView dv, enum player player) {
  return _gv_get_player_class(dv->gv, player);
}
