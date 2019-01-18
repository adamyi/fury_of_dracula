////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include "places.h"

void decide_hunter_move(HunterView state __attribute__((unused))) {
  // TODO(unassigned): Replace this with something better!
  size_t round = hv_get_round(state);
  location_t ret;
  if (round == 0)
    ret = arc4random_uniform(MAX_MAP_LOCATION);
  else {
    size_t num = 0;
    location_t *possible = hv_get_dests(state, &num, true, true, true);
    int t = arc4random_uniform(num);
    ret = possible[t];
  }
  char name[3];
  strncpy(name, location_get_abbrev(ret), 3);
  register_best_play(name, "random location!");
}
