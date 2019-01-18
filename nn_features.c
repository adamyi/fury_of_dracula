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

static inline void printRevealed(_game_view *gv, enum player p, int round,
                                 location_t rl) {
  printf("\"revealed\": [");
  int revealed[TRAIL_SIZE];
  int rc = 0;
  location_t trail[TRAIL_SIZE];
  _gv_get_move_history(gv, PLAYER_DRACULA, trail);
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
  putchar(']');
}

static inline void printMove(_game_view *gv, enum player p, int round,
                             location_t *rl) {
  printf("\"move\": \"");
  if (p == PLAYER_DRACULA) {
    char placement = '.', left = '.';
    if ((gv->vampire != *rl && gv->traps[*rl] < 3) ||
        (gv->vampire == *rl && gv->traps[*rl] < 2)) {
      if (round % 13 == 0) {
        placement = 'V';
        printf(".V");
      } else {
        placement = 'T';
        printf("T.");
      }
    }
    if (gv->trail_last_loc != NOWHERE && gv->vampire == gv->trail_last_loc)
      left = 'V';
    else if (gv->trail_last_loc != NOWHERE && gv->traps[gv->trail_last_loc] > 0)
      left = 'M';
    putchar(left);
    putchar('.');
    parse_dracula_minion_placement(gv, *rl, placement);
    parse_dracula_minion_left_trail(gv, left);
  } else {
    int c = 0;
    while (gv->traps[*rl] > 0) {
      gv->traps[*rl]--;
      c++;
      putchar('T');
      parse_hunter_encounter(gv, p, *rl, 'T');
      *rl = _gv_get_real_location(gv, p);
    }
    if (gv->vampire == *rl) {
      putchar('V');
      c++;
      parse_hunter_encounter(gv, p, *rl, 'V');
      *rl = _gv_get_real_location(gv, p);
    }
    if (*rl == _gv_get_real_location(gv, PLAYER_DRACULA)) {
      putchar('D');
      c++;
      parse_hunter_encounter(gv, p, *rl, 'D');
      *rl = _gv_get_real_location(gv, p);
    }
    for (; c < 4; c++) putchar('.');
  }
  putchar('"');
}

static inline void printFeatures(_game_view *gv) {
  printf("\"features\": [");
  for (int i = 0; i < 5; i++) {
    // one-hot encoding of location
    location_t loc = _gv_get_real_location(gv, i);
    for (int j = MIN_MAP_LOCATION; j < loc; j++) printf("0, ");
    printf("1, ");
    for (int j = loc; j < MAX_MAP_LOCATION; j++) printf("0, ");
  }
  printf("%d, ", _gv_get_round(gv));
  for (int i = 0; i < 5; i++) {
    location_t trail[TRAIL_SIZE];
    _gv_get_move_history(gv, i, trail);
    for (int j = 0; j < 6; j++) printf("%d, ", trail[j]);
  }
  for (int i = 0; i < 5; i++) printf("%d, ", _gv_get_health(gv, i));
  printf("%d]", _gv_get_score(gv));
}

static inline void printActionSpace(_game_view *gv, enum player p,
                                    location_t rl, int round) {
  printf("\"actions\": [");
  if (round == 0) {
    for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
      if (p == PLAYER_DRACULA && i == HOSPITAL_LOCATION) continue;
      if (i != MIN_MAP_LOCATION) printf(", ");
      printf("%d", i);
    }
  } else {
    size_t n_locations = 0;
    location_t *cango;
    if (p == PLAYER_DRACULA)
      cango = _gv_do_get_connections(gv, &n_locations, rl, p, round, true, true,
                                     true, true, false, true);
    else
      cango = _gv_do_get_connections(gv, &n_locations, rl, p, round, true, true,
                                     true, true, true, false);
    for (int i = 0; i < n_locations; i++) {
      if (i != 0) printf(", ");
      printf("%d", cango[i]);
    }
    free(cango);
  }
  putchar(']');
}

// given past plays
// print out feature vector,
// round number of dracula moves that are revealed due to the latest
// move, and the correct string of this move
int main(int argc, const char *argv[]) {
  bool track_minions = false;
  if (argc > 1) track_minions = argv[1][0] - '0';

  char *past_plays = NULL;
  // fgets(past_plays, MAXL, stdin);
  size_t len;
  getline(&past_plays, &len, stdin);
  while (past_plays[len - 1] != '.' &&
         (past_plays[len - 1] < 'A' || past_plays[len - 1] > 'Z'))
    past_plays[--len] = '\0';

  struct _game_view *gv = _gv_new(past_plays, NULL, track_minions);

  enum player p = _gv_get_player(gv);
  if (p == 0)
    p = PLAYER_DRACULA;
  else
    p--;
  int round = _gv_get_round(gv);
  if (p == PLAYER_DRACULA) round--;
  location_t rl = _gv_get_real_location(gv, p);

  putchar('{');
  printRevealed(gv, p, round, rl);
  printf(", ");
  printMove(gv, p, round, &rl);
  printf(", ");
  printFeatures(gv);
  printf(", \"player\": %d, ", p);
  if (p == PLAYER_DRACULA) {
    p = 0;
    round++;
  } else {
    p++;
  }
  rl = _gv_get_real_location(gv, p);
  printActionSpace(gv, p, rl, round);
  puts("}");

  return EXIT_SUCCESS;
}
