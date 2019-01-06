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

#include "game.h"
#include "game_view.h"
#include "hunter_view.h"
// #include "map.h" ... if you decide to use the Map ADT

typedef struct hunter_view {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} hunter_view;

hunter_view *hv_new(char *past_plays, player_message messages[]) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  hunter_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate HunterView");

  return new;
}

void hv_drop(hunter_view *hv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  free(hv);
}

round_t hv_get_round(hunter_view *hv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

enum player hv_get_player(hunter_view *hv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

int hv_get_score(hunter_view *hv) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

int hv_get_health(hunter_view *hv, enum player player) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

location_t hv_get_location(hunter_view *hv, enum player player) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  return 0;
}

void hv_get_trail(hunter_view *hv, enum player player,
                  location_t trail[TRAIL_SIZE]) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *hv_get_dests(hunter_view *hv, size_t *n_locations, bool road,
                         bool rail, bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}

location_t *hv_get_dests_player(hunter_view *hv, size_t *n_locations,
                                enum player player, bool road, bool rail,
                                bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}
