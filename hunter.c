////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include "places.h"

void decide_hunter_move(HunterView hv) {
  // TODO(unassigned): Replace this with something better!
  srand(time(0));
  size_t round = hv_get_round(hv);
  location_t ret;
  if (round == 0) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    ret = rand() % MAX_MAP_LOCATION;
  } else {
    size_t num = 0;
    location_t *possible = hv_get_dests(hv, &num, true, true, true);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    int t = rand() % num;
    ret = possible[t];
  }
  char name[3];
  strncpy(name, location_get_abbrev(ret), 3);
  register_best_play(name, "random location!");
}
