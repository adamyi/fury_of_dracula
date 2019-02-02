////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// #include <Python.h>

#include "game.h"
#include "hunter.h"
#include "hunter_view.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"
#include "myplayer.h"
#include "places.h"
#include "rollingarray.h"

#include "ac_log.h"
#include "ac_memory.h"

// #define DEBUG_AS_ERROR

#ifdef HUNTER_SEARCH_FAST_MODE  // for testing
#define MAX_SCENARIOS 10000
#define CHECK_TIME_INTERVAL 255
#define SEARCH_ALLOWED_TIME 100000  // usec - 0.1s
#else                               // prod settings
#define MAX_SCENARIOS 500000
#define CHECK_TIME_INTERVAL 32767
#define SEARCH_ALLOWED_TIME 900000  // usec - 0.9s
#endif

#ifdef DEBUG_AS_ERROR
#define HUNT_LOG AC_LOG_ERROR
#else
#define HUNT_LOG AC_LOG_INFO
#endif

typedef struct scenario {
  player_t *player;
  struct scenario *prev;
  struct scenario *next;
} scenario_t;

static inline bool isValidLoc(location_t loc) {
  return loc >= MIN_MAP_LOCATION && loc <= MAX_MAP_LOCATION;
}

static inline int randint(int max) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
  return rand() % max;
}

static int probabilities[NUM_MAP_LOCATIONS];

static struct timeval start_time;

static location_t resolve_loc_backwards(player_t *player,
                                        const location_t *hist, round_t round,
                                        location_t move) {
  location_t ret = player_resolve_move_location(player, move);
  if (ret != NOWHERE) return ret;
  while (move == HIDE || (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)) {
    round -= (move == HIDE) ? 1 : (move - DOUBLE_BACK_1 + 1);
    move = hist[round];
  }
  if (move == TELEPORT) return CASTLE_DRACULA;
  return move;
}

bool getPossibleDraculaLocations(player_t *players[], round_t round) {
  memset(probabilities, 0, sizeof(probabilities));
  round_t last_known_round;
  location_t last_known_location = NOWHERE;
  for (last_known_round = round - 1;
       (!isValidLoc(last_known_location)) && last_known_round > -1;
       last_known_round--) {
    last_known_location =
        players[PLAYER_DRACULA]->all_location_history[last_known_round];
    if (last_known_location == TELEPORT) last_known_location = CASTLE_DRACULA;
  }
  ac_log(AC_LOG_DEBUG, "last_known_round %d last_known_location %s",
         last_known_round, location_get_abbrev(last_known_location));
  if (!isValidLoc(last_known_location)) return false;
  scenario_t *start = ac_malloc(sizeof(scenario_t), "scenario");
  start->player = new_player(PLAYER_DRACULA, false);
  player_move_to(start->player, last_known_location, last_known_location);
  start->prev = start->next = NULL;
  scenario_t *end = start;
  int scount = 1;
  int iterations = 1;
  for (last_known_round += 2; last_known_round < round; last_known_round++) {
    location_t loc =
        players[PLAYER_DRACULA]->all_location_history[last_known_round];
    // scenario_t **l = &start;
    bool cont = true;
    if (scount == 0) {
      ac_log(AC_LOG_ERROR, "scount = 0");
      return false;
    }
    ac_log(AC_LOG_DEBUG, "lkround : %d loc: %s", last_known_round,
           location_get_abbrev(loc));
    ac_log(AC_LOG_DEBUG, "oend %p", end);
    for (scenario_t *oend = end, *i = start; cont && i != oend->next;
         iterations++) {
      // NOTES: CHECK_TIME_INTERVAL must be power of 2 - 1
      if (!(iterations & CHECK_TIME_INTERVAL)) {
        static struct timeval ct;
        gettimeofday(&ct, NULL);
        // NOTES: won't overflow since sec difference is at most 1 or 2.
        int time_elapsed = (ct.tv_sec - start_time.tv_sec) * 1000000 +
                           ct.tv_usec - start_time.tv_usec;
        // ac_log(AC_LOG_ERROR, "time: %d", time_elapsed);
        if (time_elapsed >= SEARCH_ALLOWED_TIME) {
          for (scenario_t *td = start; td != NULL; td = td->next) {
            if (td->prev != NULL) {
              destroy_player(td->prev->player);
              free(td->prev);
            }
          }
          destroy_player(end->player);
          free(end);
          ac_log(AC_LOG_ERROR, "reached SEARCH_ALLOWED_TIME");
          return false;
        }
      }
      int plays = 0;
      location_t firstPlay;
      if (loc == HIDE || (loc >= DOUBLE_BACK_1 && loc <= DOUBLE_BACK_5)) {
        location_t rl = resolve_loc_backwards(
            i->player, players[PLAYER_DRACULA]->all_location_history,
            last_known_round, loc);
        // printf("%s %s\n", location_get_abbrev(loc), location_get_abbrev(rl));
        if (rl >= MIN_MAP_LOCATION && rl <= MAX_MAP_LOCATION) {
          if ((loc == HIDE && location_get_type(rl) == LAND) ||
              (loc > HIDE &&
               (isConnectedVia(i->player->location, rl, 5) ||
                rl == i->player->location))) {  // 101: boat and road
            ac_log(AC_LOG_DEBUG, "record firstPlay=%s",
                   location_get_abbrev(loc));
            firstPlay = loc;
            plays++;
          }
        } else {
          loc = rl;
        }
      }
      if (loc == SEA_UNKNOWN || loc == CITY_UNKNOWN) {
        size_t n_locations = 0;
        location_t *moves;
        /*
location_t *_gv_do_get_connections(player_t *pobj, size_t *n_locations,
                                   location_t from, enum player player,
                                   round_t round, bool road, bool rail,
                                   bool sea, bool trail, bool stay, bool hide);
                                   */
        if (loc == SEA_UNKNOWN) {
          moves = _gv_do_get_connections(
              i->player, &n_locations, i->player->location, PLAYER_DRACULA,
              last_known_round, false, false, true, true, false, false);
        } else {
          // NOTES: boat is set to true (go to city from sea via boat)
          moves = _gv_do_get_connections(
              i->player, &n_locations, i->player->location, PLAYER_DRACULA,
              last_known_round, true, false, true, true, false, false);
        }
        ac_log(AC_LOG_DEBUG, "_gv_do_get_connections ret: %d", n_locations);
        for (int j = 0; j < n_locations; j++) {
          ac_log(AC_LOG_DEBUG, "Consider %s", location_get_abbrev(moves[j]));
          if ((location_get_type(moves[j]) == LAND &&
               (loc == SEA_UNKNOWN ||
                (moves[j] ==
                 players[0]->all_location_history[last_known_round]) ||
                (moves[j] ==
                 players[1]->all_location_history[last_known_round]) ||
                (moves[j] ==
                 players[2]->all_location_history[last_known_round]) ||
                (moves[j] ==
                 players[3]->all_location_history[last_known_round]) ||
                (moves[j] ==
                 players[0]->all_location_history[last_known_round + 1]) ||
                (moves[j] ==
                 players[1]->all_location_history[last_known_round + 1]) ||
                (moves[j] ==
                 players[2]->all_location_history[last_known_round + 1]) ||
                (moves[j] ==
                 players[3]->all_location_history[last_known_round + 1]))) ||
              (loc == CITY_UNKNOWN && location_get_type(moves[j]) == SEA))
            continue;
          if (plays == 0) {
            ac_log(AC_LOG_DEBUG, "record firstPlay=%s",
                   location_get_abbrev(moves[j]));
            firstPlay = moves[j];
          } else {
            scenario_t *s = ac_malloc(sizeof(scenario_t), "scenario");
            s->player = clone_player(i->player);
            player_move_to(s->player, moves[j], moves[j]);
            s->next = NULL;
            s->prev = end;
            end->next = s;
            end = s;
            scount++;
            ac_log(AC_LOG_DEBUG, "clone scenario %p to %s", s,
                   location_get_abbrev(moves[j]));
            // NOTES (adamyi):
            // here we keep track of all possible scenarios without
            // merging them since trail is a great way to cut down
            // possibilities, and even though dracula is at the same
            // location, it doesn't mean he has the same trail.
            // One way to optimize this is to merge scenarios when two
            // share the exact same trail. Such an optimization could
            // be done with a hash table.
          }
          plays++;
          if (scount > MAX_SCENARIOS) {
            for (scenario_t *td = start; td != NULL; td = td->next) {
              if (td->prev != NULL) {
                destroy_player(td->prev->player);
                free(td->prev);
              }
            }
            destroy_player(end->player);
            free(end);
            free(moves);
            ac_log(AC_LOG_ERROR, "reached MAX_SCENARIOS");
            return false;
          }
        }
        free(moves);
      }
      if (plays == 0) {
        ac_log(AC_LOG_DEBUG, "destroy scenario %p", i);
        ac_log(AC_LOG_DEBUG, "scenario count: %d", scount);
        scenario_t *nxt = i->next;
        if (i->prev != NULL)
          i->prev->next = i->next;
        else
          start = i->next;
        if (i->next != NULL)
          i->next->prev = i->prev;
        else
          end = i->prev;
        if (i == oend) {
          ac_log(AC_LOG_INFO, "nocont!");
          cont = false;
        }
        destroy_player(i->player);
        free(i);
        i = nxt;
        scount--;
      } else {
        ac_log(AC_LOG_DEBUG, "play scenario %p to %s", i,
               location_get_abbrev(firstPlay));
        player_move_to(
            i->player,
            resolve_loc_backwards(i->player,
                                  players[PLAYER_DRACULA]->all_location_history,
                                  last_known_round, firstPlay),
            firstPlay);
        ac_log(AC_LOG_DEBUG, "i bef %p", i);
        i = i->next;
        ac_log(AC_LOG_DEBUG, "i aft %p", i);
      }
      // ac_log(AC_LOG_DEBUG, "l %p\n", l);
    }
  }
  if (start == NULL) return false;
  if (scount == 0) return false;
  scenario_t *n;
  for (; start != NULL; start = n) {
    n = start->next;
    probabilities[start->player->location]++;
    destroy_player(start->player);
    free(start);
  }
  return true;
}

static location_t sp_go_to(player_t *p, location_t dest, int round) {
  ac_log(HUNT_LOG, "sp_go_to %s", location_get_abbrev(dest));
  if (p->location == dest) return dest;
  size_t n_locations = 0;
  size_t count = 1;
  location_t moves[NUM_MAP_LOCATIONS];
  location_t loc[NUM_MAP_LOCATIONS];
  round_t rounds[NUM_MAP_LOCATIONS];
  bool seen[NUM_MAP_LOCATIONS];
  memset(seen, 0, sizeof(seen));
  moves[0] = NOWHERE;
  loc[0] = p->location;
  rounds[0] = round;
  seen[p->location] = true;

  int i;
  // NOTES(adamyi): disabled linting due to false positive
  // loc[i] is never used before initializing
  // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
  for (i = 0; loc[i] != dest; i++) {
    location_t *ds =
        _gv_do_get_connections(p, &n_locations, loc[i], p->id, rounds[i], true,
                               true, true, false, false, false);
    for (int j = 0; j < n_locations; j++) {
      if (seen[ds[j]]) continue;
      seen[ds[j]] = true;
      if (moves[i] == NOWHERE)
        moves[count] = ds[j];
      else
        moves[count] = moves[i];
      loc[count] = ds[j];
      rounds[count] = rounds[i] + 1;
      count++;
    }
    free(ds);
  }
  return moves[i];
}

static inline double weighted_spdist(int spdist) {
  if (spdist == 1) return 1.8;
  if (spdist == 2) return 1.5;
  if (spdist == 3) return 1.2;
  return 1;
}

location_t decode_location_from_msg(const char *msg, round_t round,
                                    enum player player) {
  int ret = 0;
  int i = 0;
  for (; msg[i] != '\0'; i += 3, ret++) {
    if (msg[i] == 'C' && msg[i + 1] == 'D') {
      i += 3;
      break;
    }
  }
  for (; msg[i] != '\0'; i += 3, ret += 10) {
    if (msg[i] == 'C' && msg[i + 1] == 'D') {
      i += 3;
      break;
    }
  }
  for (; msg[i] != '\0'; i += 3, ret += 100) continue;
  ac_log(HUNT_LOG, "decoded location - %s (%d/%d)",
         location_get_abbrev(ret - round - player), round, player);
  return ret - round - player;
}

void encode_msg_from_location(char *msg, location_t loc, round_t round,
                              enum player player) {
  ac_log(AC_LOG_INFO, "encoding location %s (%d/%d)", location_get_abbrev(loc),
         round, player);
  int l = loc + round + player;
  int idx = 0;
  for (int i = l % 10; i > 0; i--) {
    location_t loc = randint(NUM_MAP_LOCATIONS);
    while (loc == CASTLE_DRACULA) loc = randint(NUM_MAP_LOCATIONS);
    strncpy(msg + idx, location_get_abbrev(loc), 3);
    msg[idx + 2] = ' ';
    idx += 3;
  }
  msg[idx++] = 'C';
  msg[idx++] = 'D';
  msg[idx++] = ' ';
  l /= 10;
  for (int i = l % 10; i > 0; i--) {
    location_t loc = randint(NUM_MAP_LOCATIONS);
    while (loc == CASTLE_DRACULA) loc = randint(NUM_MAP_LOCATIONS);
    strncpy(msg + idx, location_get_abbrev(loc), 3);
    msg[idx + 2] = ' ';
    idx += 3;
  }
  msg[idx++] = 'C';
  msg[idx++] = 'D';
  msg[idx++] = ' ';
  l /= 10;
  for (int i = l % 10; i > 0; i--) {
    location_t loc = randint(NUM_MAP_LOCATIONS);
    while (loc == CASTLE_DRACULA) loc = randint(NUM_MAP_LOCATIONS);
    strncpy(msg + idx, location_get_abbrev(loc), 3);
    msg[idx + 2] = ' ';
    idx += 3;
  }
}

void decide_hunter_move(HunterView hv) {
  gettimeofday(&start_time, NULL);
  unsigned int ts = (start_time.tv_usec % 5000) * (start_time.tv_sec % 5000);
  srand(ts);
  ac_log(AC_LOG_ERROR,
         "This function will print out where hunter thinks the dracula is "
         "in the player messages. This is very dangerous. I'm so scared the "
         "Dracula will figure out and move to other locations... Hope Dracula "
         "never finds out...");  // sarcastically just for fuun
  // srand(time(0));
  round_t round = hv_get_round(hv);
  location_t ret;
  enum player cp = hv_get_player(hv);
  player_t *players[NUM_PLAYERS];
  char msg[100];
  memset(msg, 0, sizeof(msg));
  location_t previousTargets[NUM_PLAYERS];
  for (int i = 0; i < cp; i++)
    previousTargets[i] = decode_location_from_msg(hv_get_msg(hv, i), round, i);
  if (round == 0) {
    ret = randint(NUM_MAP_LOCATIONS);
  } else {
    for (int i = 0; i < NUM_PLAYERS; i++) {
      players[i] = hv_get_player_class(hv, i);
    }
    // size_t num = 0;
    // location_t *possible = hv_get_dests(hv, &num, true, true, true);
    bool guessDracula = getPossibleDraculaLocations(players, round);
    ac_log(HUNT_LOG, "getprob: %d", guessDracula);
    double maxprob = 0;
    int actionSpaceSize = 0;
    location_t maxprobl = NOWHERE;
    for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
      if (probabilities[i] > 0) {
        actionSpaceSize += probabilities[i];
        // printf("%s %s\n", location_get_abbrev(i),
        // location_get_abbrev(players[cp]->location));
        double prob = weighted_spdist(SPDIST[i][players[cp]->location]) *
                      probabilities[i];
        for (int j = 0; j < cp; j++) {
          if (i == previousTargets[j]) prob *= 0.90;
        }
        ac_log(HUNT_LOG, "%s: %d -> %lf", location_get_abbrev(i),
               probabilities[i], prob);
        if (prob > maxprob) {
          maxprob = prob;
          maxprobl = i;
        }
      }
    }
    ac_log(AC_LOG_INFO, "maxprob %lf actionSpace %d\n", maxprob,
           actionSpaceSize);
    // if (maxprob * 10 < actionSpaceSize)  // if we are not 10% sure where Drac
    // is
    //  guessDracula = false;
    // if (actionSpaceSize > MAX_SCENARIOS && maxprob * 11 < actionSpaceSize)
    //    guessDracula = false;
    ac_log(AC_LOG_INFO, "%s %s",
           location_get_abbrev(hv_get_location(hv, PLAYER_DRACULA)),
           location_get_abbrev(hv_get_location(hv, cp)));
    ac_log(AC_LOG_INFO, "health %d", hv_get_health(hv, cp));
    if (hv_get_location(hv, PLAYER_DRACULA) == hv_get_location(hv, cp) ||
        hv_get_health(hv, cp) <= 4 || (!guessDracula)) {
      if ((!guessDracula) && round < 6) {
        size_t num = 0;
        location_t *possible = hv_get_dests(hv, &num, true, true, true);
        ret = possible[randint(num)];
        free(possible);
      } else {
        ret = hv_get_location(hv, cp);
      }
    } else {
      ret = sp_go_to(players[cp], maxprobl, round);
      encode_msg_from_location(msg, maxprobl, round, cp);
    }
  }
  char name[3];
  strncpy(name, location_get_abbrev(ret), 3);
  if (msg[0] == '\0') encode_msg_from_location(msg, ret, round, cp);
  register_best_play(name, msg);
}
