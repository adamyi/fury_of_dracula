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
                        location_t *end) {  // TODO TEST
  *end = gv_get_location(dv->gv, player);

  location_t trail[TRAIL_SIZE];
  gv_get_history(dv->gv, player, trail);
  round_t round = gv_get_round(dv->gv);

  if (round == 0) {
    *start = UNKNOWN_LOCATION;

  } else {
    *start = trail[0];
  }
  return;
}

// function to take the index of a trail move and determine the round that move
// was made in
static int get_round_trail_move_made(dracula_view *dv, int index) {
  int round = gv_get_round(dv->gv);
  return round - index;
}

void dv_get_locale_info(
    dracula_view *dv, location_t where, int *n_traps,
    int *n_vamps) {  // TODO traps and vampire tracking can be done in gameview
                     // in second part of assignment
  // TODO TEST
  int last_hunter_visit = TRAIL_SIZE;
  enum player hunters = 0;

  location_t trail[TRAIL_SIZE] = {-1};

  while (hunters <= PLAYER_MINA_HARKER) {
    gv_get_history(dv->gv, hunters, trail);
    int i = 0;
    while (i < TRAIL_SIZE) {
      if (trail[i] == where && i < last_hunter_visit) {
        last_hunter_visit = i;
        break;
      }
      i++;
    }
  }
  int j;
  if (last_hunter_visit == TRAIL_SIZE) {
    j = TRAIL_SIZE - 1;
  } else {
    j = last_hunter_visit;
  }

  gv_get_history(dv->gv, PLAYER_DRACULA, trail);

  while (j > 0) {
    if (trail[j] == where) {
      if (get_round_trail_move_made(dv, j) % 13 == 0) {
        (*n_vamps)++;
      } else {
        (*n_traps)++;
      }
    }
    j--;
  }

  return;
}

void dv_get_trail(dracula_view *dv, enum player player,
                  location_t trail[TRAIL_SIZE]) {
  gv_get_history(dv->gv, player, trail);
  // TODO check how doublebacks influence trails & test
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
