////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "game.h"
#include "game_view.h"
// #include "map.h" ... if you decide to use the Map ADT

typedef struct game_view {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} game_view;

game_view *gv_new(char *past_plays, player_message messages[]) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  game_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate GameView");

  return new;
}

void gv_drop(game_view *gv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  free(gv);
}

round_t gv_get_round(game_view *gv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

enum player gv_get_player(game_view *gv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

int gv_get_score(game_view *gv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

int gv_get_health(game_view *gv, enum player player) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

location_t gv_get_location(game_view *gv, enum player player) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

void gv_get_history(game_view *gv, enum player player,
                    location_t trail[TRAIL_SIZE]) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *gv_get_connections(game_view *gv, size_t *n_locations,
                               location_t from, enum player player,
                               round_t round, bool road, bool rail, bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}
