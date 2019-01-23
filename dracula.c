////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Python.h>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"
#include "mapdata.h"

static inline int weighted_spdist(int spdist) {
  int ret = spdist;
  if (spdist == 0)
    ret = -100;
  else if (spdist == 1)
    ret = -1;
  return ret;
}

void decide_dracula_move(DraculaView dv) {
  // TODO(unassigned): Replace this with something better!
  srand(time(0));
  size_t round = dv_get_round(dv);
  location_t ret = NOWHERE;
  size_t num = 0;
  location_t *possible;
  possible = dv_get_possible_moves(dv, &num);
  int dist[NUM_MAP_LOCATIONS];
  memset(dist, 0, sizeof(dist));
  for (int i = 0; i < PLAYER_DRACULA; i++) {
    location_t loc = dv_get_location(dv, i);
    if (round == 0) {
      for (int j = MIN_MAP_LOCATION; j <= MAX_MAP_LOCATION; j++) {
        if (j == HOSPITAL_LOCATION) continue;
        dist[j] += weighted_spdist(SPDIST[loc][j]);
      }
    } else {
      for (int j = 0; j < num; j++) {
        if (possible[j] >= MIN_MAP_LOCATION &&
            possible[j] <= MAX_MAP_LOCATION) {
          dist[possible[j]] += weighted_spdist(SPDIST[loc][possible[j]]);
        }
      }
    }
  }
  int maxdist = 0;
  for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
    if (location_get_type(i) == SEA) {
      if (dist[i] < 0)
        dist[i] = 1;
      else
        dist[i] *= 0.75;
    }
    if (dist[i] > maxdist) {
      maxdist = dist[i];
      ret = i;
    }
  }
  if (ret == NOWHERE) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    ret = possible[rand() % num];
  }
  free(possible);
  char name[3];
  strncpy(name, location_get_abbrev(ret), 3);
  register_best_play(name, "random location!");
}
