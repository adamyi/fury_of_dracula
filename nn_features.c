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

// given past plays
// print out feature vector,
// round number of dracula moves that are revealed due to the latest
// move, and the correct string of this move
int main() {
  char past_plays[MAXL];
  fgets(past_plays, MAXL, stdin);
  struct _game_view *gv = _gv_new(past_plays, NULL, true);
  printf("{\"features\": [");
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
  printf("%d], \"revealed\": [", _gv_get_score(gv));
  enum player p = _gv_get_player(gv);
  if (p == 0)
    p = PLAYER_DRACULA;
  else
    p--;
  int revealed[TRAIL_SIZE];
  int rc = 0, round = _gv_get_round(gv);
  if (p == PLAYER_DRACULA) round--;
  location_t trail[TRAIL_SIZE];
  _gv_get_move_history(gv, PLAYER_DRACULA, trail);
  location_t rl = _gv_get_real_location(gv, p);
  if (p == PLAYER_DRACULA) {
    if (rl == CASTLE_DRACULA)
      revealed[rc++] = round;
    else if (rl == _gv_get_real_location(gv, 0) ||
             rl == _gv_get_real_location(gv, 1) ||
             rl == _gv_get_real_location(gv, 2) ||
             rl == _gv_get_real_location(gv, 3)) {
      int i = 0, li = -1;
      do {
        li = i;
        revealed[rc++] = round - i;
        if (trail[i] >= DOUBLE_BACK_1 && trail[i] <= DOUBLE_BACK_5)
          i += trail[i] - DOUBLE_BACK_1 + 1;
        else if (trail[i] == HIDE)
          i++;
      } while (i != li && i < TRAIL_SIZE);
    } else {
      location_t move = _gv_get_location(gv, p);
      if (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)
        revealed[rc++] = round;
    }
  } else {
    if (gv->rests == 3 && round >= 6) {
      revealed[rc++] = round - 6;
      // TODO(adamyi): wait for jashankj's answer for recursion.
    }
    for (int i = 0; i < TRAIL_SIZE; i++) {
      if (trail[i] == _gv_get_real_location(gv, p)) {
        revealed[rc++] = round - i - 1;
        break;
      }
    }
  }

  for (int i = 0; i < rc; i++) {
    if (i != 0) printf(", ");
    printf("%d", revealed[i]);
  }
  printf("], \"move\": \"");

  if (p == PLAYER_DRACULA) {
    if ((gv->vampire != rl && gv->traps[rl] < 3) ||
        (gv->vampire == rl && gv->traps[rl] < 2)) {
      if (round % 13 == 0)
        printf(".V");
      else
        printf("T.");
    }
    if (gv->vampire == gv->trail_last_loc)
      putchar('V');
    else if (gv->traps[gv->trail_last_loc] > 0)
      putchar('M');
    else
      putchar('.');
    putchar('.');
  } else {
    int c = 0;
    while (gv->traps[rl] > 0) {
      gv->traps[rl]--;
      c++;
      putchar('T');
    }
    if (gv->vampire == rl) {
      putchar('V');
      c++;
    }
    if (rl == _gv_get_real_location(gv, PLAYER_DRACULA)) {
      putchar('D');
      c++;
    }
    for (; c < 4; c++) putchar('.');
  }
  printf("\"}\n");
  return EXIT_SUCCESS;
}
