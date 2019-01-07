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

#include "dracula_view.h"
#include "game.h"
#include "game_view.h"
#include "map.h"
#include "mapdata.h"

typedef struct dracula_view {
  GameView gv;
} dracula_view;

dracula_view *dv_new(char *past_plays, player_message messages[]) {
  dracula_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate DraculaView");
  new->gv = gv_new(past_plays, messages);

  return new;
}

void dv_drop(dracula_view *dv) {
  gv_drop(dv->gv);
  free(dv);
}

round_t dv_get_round(dracula_view *dv) { return gv_get_round(dv->gv); }

int dv_get_score(dracula_view *dv) { return gv_get_score(dv->gv); }

int dv_get_health(dracula_view *dv, enum player player) {
  return gv_get_health(dv->gv, player);
}

location_t dv_get_location(dracula_view *dv, enum player player) {
  return gv_get_location(dv->gv, player);
}

void dv_get_player_move(dracula_view *dv, enum player player, location_t *start,
                        location_t *end) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return;
}

void dv_get_locale_info(dracula_view *dv, location_t where, int *n_traps,
                        int *n_vamps) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return;
}

void dv_get_trail(dracula_view *dv, enum player player,
                  location_t trail[TRAIL_SIZE]) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *dv_get_dests(dracula_view *dv, size_t *n_locations, bool road,
                         bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}

location_t *dv_get_dests_player(dracula_view *dv, size_t *n_locations,
                                enum player player, bool road, bool rail,
                                bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}
