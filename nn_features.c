#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sysexits.h>
#include <unistd.h>

#include "game.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"
#include "places.h"

#define MAXL 1000

int main() {
  char past_plays[MAXL];
  fgets(past_plays, MAXL, stdin);
  struct _game_view *gv = _gv_new(past_plays, NULL, true);
  printf("[");
  for (int i = 0; i < 5; i++) {
    // one-hot encoding of location
    location_t loc = _gv_get_real_location(gv, i);
    for (int j = MIN_MAP_LOCATION; j < loc; j++) printf("0, ");
    printf("1, ");
    for (int j = loc; j < MAX_MAP_LOCATION; j++) printf("0, ");
  }
  for (int i = 0; i < 5; i++) {
    location_t trail[TRAIL_SIZE];
    _gv_get_move_history(gv, i, trail);
    for (int j = 0; j < 6; j++) printf("%d, ", trail[j]);
  }
  printf("%d, ", _gv_get_round(gv));
  for (int i = 0; i < 5; i++) printf("%d, ", _gv_get_health(gv, i));
  printf("%d]\n", _gv_get_score(gv));
  enum player p = _gv_get_player(gv);
  int revealed[TRAIL_SIZE];
  int rc = 0, round = _gv_get_round(gv);
  if (p == PLAYER_DRACULA) {
    location_t rl = _gv_get_real_location(gv, p);
    if (rl == CASTLE_DRACULA)
      revealed[rc++] = round;
    else if (rl == _gv_get_real_location(gv, 0) ||
             rl == _gv_get_real_location(gv, 1) ||
             rl == _gv_get_real_location(gv, 2) ||
             rl == _gv_get_real_location(gv, 3)) {
      revealed[rc++] = round;
      // TODO(adamyi): recursive HIDE/DOUBLE_BACK
    } else {
      location_t move = _gv_get_location(gv, p);
      if (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)
        revealed[rc++] = round;
    }
  } else {
    if (gv->rests == 3 && _gv_get_round(gv) >= 6) {
      revealed[rc++] = round - 6;
      // TODO(adamyi): recursive HIDE/DOUBLE_BACK
    }
    location_t trail[TRAIL_SIZE];
    _gv_get_move_history(gv, PLAYER_DRACULA, trail);
    for (int i = 0; i < TRAIL_SIZE; i++) {
      if (trail[i] == _gv_get_real_location(gv, p)) {
        revealed[rc++] = round - i - 1;
        break;
      }
    }
  }

  putchar('[');
  for (int i = 0; i < rc; i++) {
    if (i != 0) printf(", ");
    printf("%d", revealed[i]);
  }
  puts("]");
}
