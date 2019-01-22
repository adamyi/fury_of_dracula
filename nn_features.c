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
#include "rollingarray.h"

static inline void oneHotEncoding(location_t t) {
  /*
  CITY_UNKNOWN = 100,
  SEA_UNKNOWN = 101,
  HIDE = 102,
  DOUBLE_BACK_1 = 103,
  DOUBLE_BACK_2 = 104,
  DOUBLE_BACK_3 = 105,
  DOUBLE_BACK_4 = 106,
  DOUBLE_BACK_5 = 107,
  TELEPORT = 108,
  */
  int MAX = 9 + MAX_MAP_LOCATION;
  if (t == NOWHERE) {
    for (int i = MIN_MAP_LOCATION; i <= MAX; i++) printf("0, ");
  } else {
    int loc = (int)t;
    if (t >= CITY_UNKNOWN) loc += MAX_MAP_LOCATION - CITY_UNKNOWN + 1;
    for (int i = MIN_MAP_LOCATION; i < loc; i++) printf("0, ");
    printf("1, ");
    for (int i = loc + 1; i <= MAX; i++) printf("0, ");
  }
}

static inline int weight_dist(int dist) {
  if (dist == 0) return -64;
  if (dist == 1) return -12;
  if (dist == 2) return -2;
  if (dist < 5) return -1;
  return 1;
}

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
                             location_t *rl, const char *move) {
  printf("\"move\": \"");
  if (strcmp(move, "....") != 0) {
    printf("%s", move);
  } else if (p == PLAYER_DRACULA) {
    char placement = '.', left = '.';
    if (location_get_type(*rl) != SEA) {
      int enc_count = rollingarray_size(gv->traps[*rl]);
      if (gv->vampire == *rl) enc_count++;
      if (gv->trail_last_loc == *rl) enc_count--;
      if (enc_count < 3) {
        if (round % 13 == 0) {
          placement = 'V';
          printf(".V");
        } else {
          placement = 'T';
          printf("T.");
        }
      }
    }
    if (placement == '.') printf("..");
    if (gv->trail_last_loc != NOWHERE && gv->vampire == gv->trail_last_loc &&
        round % 13 == 6)
      left = 'V';
    else if (gv->trail_last_loc != NOWHERE &&
             rollingarray_size(gv->traps[gv->trail_last_loc]) > 0 &&
             rollingarray_get_item(gv->traps[gv->trail_last_loc], 0) ==
                 round - 6)
      left = 'M';
    putchar(left);
    putchar('.');
    parse_dracula_minion_placement(gv, *rl, placement);
    parse_dracula_minion_left_trail(gv, left);
  } else {
    int c = 0;
    while (rollingarray_size(gv->traps[*rl]) > 0) {
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
    oneHotEncoding(loc);
  }
  printf("%d, ", _gv_get_round(gv));
  for (int i = 0; i < 5; i++) {
    location_t trail[TRAIL_SIZE];
    _gv_get_move_history(gv, i, trail);
    for (int j = 0; j < 6; j++) oneHotEncoding(trail[j]);
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
      if (cango[i] > MAX_MAP_LOCATION)
        printf("%d", cango[i] + MAX_MAP_LOCATION - HIDE + 1);
      else
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
  /* for (int i = 0; i <= MAX_MAP_LOCATION; i++)
    printf("%d: \"%s\", ", i, location_get_abbrev(i));
  for (int i = 102; i <= 108; i++)
    printf("%d: \"%s\", ", i, location_get_abbrev(i)); */
  bool track_minions = false;
  if (argc > 1) track_minions = argv[1][0] - '0';

  char *past_plays = NULL;
  // fgets(past_plays, MAXL, stdin);
  size_t len;
  getline(&past_plays, &len, stdin);
  // getline's len is wrong sometimes... don't know why
  len = strlen(past_plays);
  while (past_plays[len - 1] != '.' &&
         (past_plays[len - 1] < 'A' || past_plays[len - 1] > 'Z') && len > 0)
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
  printMove(gv, p, round, &rl, &past_plays[len - 4]);
  printf(", ");
  printFeatures(gv);
  printf(", \"player\": %d, ", p);

  int dist = 0;
  if (rl >= MIN_MAP_LOCATION && rl <= MAX_MAP_LOCATION) {
    for (int i = 0; i < PLAYER_DRACULA; i++) {
      location_t l = _gv_get_real_location(gv, i);
      if (l >= MIN_MAP_LOCATION && l <= MAX_MAP_LOCATION)
        dist += weight_dist(SPDIST[_gv_get_real_location(gv, i)][rl]);
    }
  }
  printf("\"weighted_distance\": %d, ", dist);
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
