////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"

void decide_dracula_move(DraculaView dv) {
  // TODO(unassigned): Replace this with something better!
  srand(time(0));
  size_t round = dv_get_round(dv);
  location_t ret;
  if (round == 0) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    ret = rand() % MAX_MAP_LOCATION;
    while (ret == HOSPITAL_LOCATION) {
      // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
      ret = rand() % MAX_MAP_LOCATION;
    }
  } else {
    size_t num = 0;
    location_t *possible = dv_get_possible_moves(dv, &num);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    int t = rand() % num;
    ret = possible[t];
  }
  char name[3];
  strncpy(name, location_get_abbrev(ret), 3);
  register_best_play(name, "random location!");
}
