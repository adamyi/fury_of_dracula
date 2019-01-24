////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// #include <Python.h>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"
#include "internal_game_view.h"
#include "mapdata.h"
#include "myplayer.h"

static inline int weighted_spdist(int spdist) {
  if (spdist == 0)
    return -100;
  else if (spdist == 1)
    return -1;
  return spdist;
}

void decide_dracula_move(DraculaView dv) {
  // TODO(unassigned): Replace this with something better!
  struct timeval t1;
  gettimeofday(&t1, NULL);
  unsigned int ts = (t1.tv_usec % 5000) * (t1.tv_sec % 5000);
  srand(ts);
  size_t round = dv_get_round(dv);
  location_t ret = NOWHERE;
  size_t num = 0;
  location_t *possible;
  possible = dv_get_possible_moves(dv, &num);
  int dist[NUM_MAP_LOCATIONS];
  bool cango[NUM_MAP_LOCATIONS];
  memset(dist, 0, sizeof(dist));
  memset(cango, 0, sizeof(cango));
  location_t rev[110];
  player_t *dracp = dv_get_player_class(dv, PLAYER_DRACULA);
  for (int i = 0; i < num; i++) {
    if (possible[i] >= HIDE) {
      location_t res = player_resolve_move_location(dracp, possible[i]);
      rev[res] = possible[i];
      possible[i] = res;
    } else
      rev[possible[i]] = possible[i];
    cango[possible[i]] = true;
  }
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
          if (rev[possible[j]] != possible[j])
            dist[possible[j]] +=
                0.75 * weighted_spdist(SPDIST[loc][possible[j]]);
          else
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
        dist[i] *= 0.65;
    }
    if (cango[i] && dist[i] > maxdist) {
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
  strncpy(name, location_get_abbrev(rev[ret]), 3);
  register_best_play(name, "random location!");
}
