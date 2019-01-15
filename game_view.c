////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: a wrapper for internal_game_view
// this is for part 1 of the assignment since the ADT cannot be changed
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "internal_game_view.h"

typedef struct game_view {
  struct _game_view *gv;
} game_view;

game_view *gv_new(char *past_plays, player_message messages[]) {
  game_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate GameView");
  new->gv = _gv_new(past_plays, messages);
  return new;
}

void gv_drop(game_view *gv) {
  _gv_drop(gv->gv);
  free(gv);
}

round_t gv_get_round(game_view *gv) { return _gv_get_round(gv->gv); }

enum player gv_get_player(game_view *gv) { return _gv_get_player(gv->gv); }

int gv_get_score(game_view *gv) { return _gv_get_score(gv->gv); }

int gv_get_health(game_view *gv, enum player player) {
  return _gv_get_health(gv->gv, player);
}

location_t gv_get_location(game_view *gv, enum player player) {
  return _gv_get_location(gv->gv, player);
}

void gv_get_history(game_view *gv, enum player player,
                    location_t trail[TRAIL_SIZE]) {
  return _gv_get_move_history(gv->gv, player, trail);
}

location_t *gv_get_connections(game_view *gv, size_t *n_locations,
                               location_t from, enum player player,
                               round_t round, bool road, bool rail, bool sea) {
  return _gv_get_connections(gv->gv, n_locations, from, player, round, road,
                             rail, sea);
}
