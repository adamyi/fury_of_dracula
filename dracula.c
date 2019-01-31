////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// #include <Python.h>

#include "ac_log.h"

#include "dracula.h"
#include "dracula_view.h"
#include "game.h"
#include "internal_game_view.h"
#include "mapdata.h"
#include "myplayer.h"

#define DEBUG_AS_ERROR

#define NEXT_Q_GAMMA 0.3
#define DB_HIDE_WEIGHT 0.75
#define NORMAL_SEA_WEIGHT 0.4
#define CRASH_CHECK_THRESHOLD 5

#ifdef DEBUG_AS_ERROR
#define DRAC_LOG AC_LOG_ERROR
#else
#define DRAC_LOG AC_LOG_INFO
#endif

static double action_q[NUM_MAP_LOCATIONS], next_q[NUM_MAP_LOCATIONS];

static inline int weighted_spdist(int spdist) {
  if (spdist == 0) return -100;
  if (spdist == 1) return -15;
  if (spdist == 2) return -5;
  if (spdist == 3) return -1;
  if (spdist > 6) return 6;
  return spdist;
}

static inline double weighted_cddist(int spdist, int health, int hDistToCP) {
  double weight = 1;
  if (health < 10)
    weight = 10;
  else if (health < 20)
    weight = 1;
  else if (health > 40)
    weight = 0.6;
  else if (health >= 50)
    weight = 0.3;
  else if (health >= 60)
    weight = 0.1;
  if (hDistToCP <= -200)
    weight = 0;
  else if (hDistToCP < 0)
    weight = -1.2;  // weight *= 0.01;
  else if (hDistToCP <= 12)
    weight = -0.8;  // weight *= 0.6;
  else if (hDistToCP > 16)
    weight *= 1.2;
  else if (hDistToCP > 32)
    weight *= 2;
  if (spdist == 0) return 10 * weight;
  if (spdist == 1) return 3 * weight;
  if (spdist == 2) return 2 * weight;
  if (spdist == 3) return 1 * weight;
  return -(spdist - 3) * weight / 2;
}

static inline double apply_weight(double x, double weight) {
  if (x > 0) return (x * weight);
  if (weight < 0) return (-x / weight);
  return (x / weight);
}

static inline void addQ(location_t loc, double action, double next,
                        const char *reason) {
  ac_log(DRAC_LOG, "add (%lf,%lf) to Q[%s] for %s", action, next,
         location_get_abbrev(loc), reason);
  action_q[loc] += action;
  next_q[loc] += next;
}

static inline void setQ(location_t loc, double action, double next,
                        const char *reason) {
  ac_log(DRAC_LOG, "set (%lf,%lf) to Q[%s] for %s", action, next,
         location_get_abbrev(loc), reason);
  action_q[loc] = action;
  next_q[loc] = next;
}

static inline void applyWeightQ(location_t loc, double action, double next,
                                const char *reason) {
  ac_log(DRAC_LOG, "apply weight (%lf,%lf) to Q[%s] for %s", action, next,
         location_get_abbrev(loc), reason);
  action_q[loc] = apply_weight(action_q[loc], action);
  next_q[loc] = apply_weight(next_q[loc], next);
}

static inline void printQMatrix(location_t *possible, size_t sz) {
  for (size_t i = 0; i < sz; i++)
    ac_log(DRAC_LOG, "%s: %lf %lf", location_get_abbrev(possible[i]),
           action_q[possible[i]], next_q[possible[i]]);
}

void decide_dracula_move(DraculaView dv) {
  struct timeval t1;
  gettimeofday(&t1, NULL);
  unsigned int ts = (t1.tv_usec % 5000) * (t1.tv_sec % 5000);
  srand(ts);
  round_t round = dv_get_round(dv);
  location_t ret = NOWHERE;
  size_t num = 0;
  location_t *possible;
  possible = dv_get_possible_moves(dv, &num);
  bool cango[NUM_MAP_LOCATIONS];
  bool crashed[PLAYER_DRACULA];
  memset(action_q, 0, sizeof(action_q));
  memset(next_q, 0, sizeof(next_q));
  memset(cango, 0, sizeof(cango));
  memset(crashed, 0, sizeof(crashed));
  location_t rev[110];
  memset(rev, -1, sizeof(rev));
  player_t *dracp = dv_get_player_class(dv, PLAYER_DRACULA);
  int health = dv_get_health(dv, PLAYER_DRACULA);
  for (int i = 0; i < PLAYER_DRACULA; i++) {
    ac_log(DRAC_LOG, "Player %d seems to have crashed", i);
    crashed[i] =
        (dv_get_player_class(dv, i)->staycount >= CRASH_CHECK_THRESHOLD);
  }
  for (size_t i = 0; i < num; i++) {
    if (possible[i] >= HIDE) {
      location_t res = player_resolve_move_location(dracp, possible[i]);
      if (possible[i] == HIDE && cango[res] &&
          rev[res] >= DOUBLE_BACK_1)  // prefer DB to HIDE
        continue;
      rev[res] = possible[i];
      possible[i] = res;
    } else
      rev[possible[i]] = possible[i];
    cango[possible[i]] = true;
  }
  int hDistSp = 0;
  for (int i = 0; i < PLAYER_DRACULA; i++) {
    location_t hl = dv_get_location(dv, i);
    if (crashed[i]) {
      if (hl == CASTLE_DRACULA)
        hDistSp += weighted_spdist(0);
      else
        hDistSp += weighted_spdist(10);
      continue;
    }
    int sr = NUM_MAP_LOCATIONS;
    for (int j = 0; j < 4; j++) {
      if (SPROUND[hl][CASTLE_DRACULA][j] < sr)
        sr = SPROUND[hl][CASTLE_DRACULA][j];
    }
    hDistSp += weighted_spdist(sr);
  }

  for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
    // if (!cango[i]) continue;
    int md = NUM_MAP_LOCATIONS;
    int distToCD = SPDIST[i][CASTLE_DRACULA];
    for (int j = 0; j < PLAYER_DRACULA; j++) {
      location_t hl = dv_get_location(dv, j);
      if (crashed[j]) {
        if (hl == i)
          addQ(i, weighted_spdist(0), weighted_spdist(0),
               "encounter crashed hunter");
        else if (SPDIST[hl][i] == 1)
          addQ(i, weighted_spdist(3), weighted_spdist(3),
               "don't want to go near crashed hunter (might have to encounter "
               "next turn)");
        continue;
      }
      int dst = SPDIST[i][hl] + SPDIST[CASTLE_DRACULA][hl] - distToCD;
      if (dst < md) md = dst;
      int hunter_max_trail = (round + 1 + j) % 4;
      addQ(i, weighted_spdist(SPROUND[hl][i][hunter_max_trail]),
           weighted_spdist((int)fmax(SPROUND[hl][i][hunter_max_trail] - 1, 0)),
           "distance to hunter");
    }
    if (md == 0) {
      addQ(i, apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.5),
           apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.5),
           "how good to go to CD (*0.5)");
    } else if (md == 1) {
      addQ(i, apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.7),
           apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.7),
           "how good to go to CD (*0.7)");
    } else if (md == 2) {
      addQ(i, apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.8),
           apply_weight(weighted_cddist(distToCD, health, hDistSp), 0.8),
           "how good to go to CD (*0.8)");
    } else {
      addQ(i, weighted_cddist(distToCD, health, hDistSp),
           weighted_cddist(distToCD, health, hDistSp),
           "how good to go to CD (*1.0)");
    }
    if (location_get_type(i) == SEA) {
      if (action_q[i] < 0) {
        if (health <= 8)
          applyWeightQ(i, 1.0 / 0.8, 1.0 / 0.8,
                       "will die soon on SEA (hunter close by)");
        else if (health > 20)
          applyWeightQ(i, 1.0 / 0.1, 1.0 / 0.1,
                       "plenty of life on SEA (hunter close by)");
        else
          applyWeightQ(i, 1.0 / 0.35, 1.0 / 0.35,
                       "plenty of life on SEA (hunter close by)");
        // action_q[i] -= 3;
      } else if (round % 13 == 0 || round % 13 == 12)
        applyWeightQ(i, 0.25, 0.25, "unable to place vampire on SEA");
      else
        applyWeightQ(i, NORMAL_SEA_WEIGHT, NORMAL_SEA_WEIGHT,
                     "we don't favor SEA (no hunter nearby)");
      if (health <= 2) setQ(i, -100, -100, "will die on SEA");
    }
    if (cango[i] && rev[i] != i)  // DB or HIDE
      applyWeightQ(i, DB_HIDE_WEIGHT, 1, "we don't favor DB/HIDE");
  }
  printQMatrix(possible, num);
  for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
    if (!cango[i]) continue;
    player_t *tmpp = clone_player(dracp);
    tmpp->all_history_size = -1;
    player_move_to(tmpp, i, rev[i]);
    size_t tmps = 0;
    location_t *tmpl =
        _gv_do_get_connections(tmpp, &tmps, i, PLAYER_DRACULA, round + 1, true,
                               false, true, true, false, true);
    double bstNextMove = -10000;
    double bstNextHideDbMove = -10000;
    size_t nxtLand = 0;
    size_t nxtSea = 0;
    size_t nxtHides = 0;
    size_t nxtDbs = 0;
    for (int j = 0; j < tmps; j++) {
      if (tmpl[j] == HIDE || tmpl[j] == DOUBLE_BACK_1) {
        nxtHides++;
      } else if (tmpl[j] >= DOUBLE_BACK_2 && tmpl[j] <= DOUBLE_BACK_5) {
        nxtDbs++;
      } else if (tmpl[j] != TELEPORT && location_get_type(tmpl[j]) == SEA) {
        nxtSea++;
      } else {
        nxtLand++;
      }
    }
    for (int j = 0; j < tmps; j++) {
      location_t l = tmpl[j];
      double w = 1;
      if (l >= HIDE && l <= DOUBLE_BACK_5) {
        l = player_resolve_move_location(tmpp, l);
        w = DB_HIDE_WEIGHT;
        if (l == HIDE || l == DOUBLE_BACK_1) {
          if (next_q[CASTLE_DRACULA] < 0) {
            w = -1;  // we don't want to teleport (e.g. forced to hide then db
                     // then tp)
          }
        }
      } else if (l == TELEPORT) {
        l = CASTLE_DRACULA;
        if (dracp->location == CASTLE_DRACULA)
          w = -1;  // we don't want to be locked in at CD
      }
      w = apply_weight(next_q[l], w);
      printf("%s %lf\n", location_get_abbrev(l), w);
      if (tmpl[j] >= HIDE && tmpl[j] <= DOUBLE_BACK_5) {
        if (w > bstNextHideDbMove) bstNextHideDbMove = w;
      } else {
        if (w > bstNextMove) bstNextMove = w;
      }
    }
    if (bstNextHideDbMove > bstNextMove) {
      if (nxtLand == 0 && nxtSea == 0) bstNextMove = bstNextHideDbMove;
    }
    printf("%s bstNextMove %lf\n", location_get_abbrev(i), bstNextMove);
    free(tmpl);
    destroy_player(tmpp);
    addQ(i, NEXT_Q_GAMMA * (bstNextMove - action_q[i]), 0, "next step bias");
    // if (nxtMoves >= 4) addQ(i, 0.1 * (nxtMoves - 4) + 1, 0, "next step action
    // space reward");
    if (action_q[i] < 0) {
      addQ(i, 0.5 * nxtLand, 0,
           "next step (land) action space reward (hunter closeby)");
      addQ(i, 0.4 * nxtSea, 0,
           "next step (sea) action space reward (hunter closeby)");
    } else if (nxtLand >= 4) {
      addQ(i, 0.1 * (nxtLand - 4) + 1, 0,
           "next step (land) action space reward (hunter far away)");
    }
  }

  double maxdist = -10000;
  for (int i = MIN_MAP_LOCATION; i <= MAX_MAP_LOCATION; i++) {
    if (cango[i])
      ac_log(DRAC_LOG, "%s: %lf", location_get_abbrev(i), action_q[i]);
    if (cango[i] && action_q[i] > maxdist) {
      maxdist = action_q[i];
      ret = i;
    }
  }
  if (ret == NOWHERE) {
    ac_log(DRAC_LOG, "random move");
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.rand)
    ret = possible[rand() % num];
  }
  free(possible);
  char name[3];
  strncpy(name, location_get_abbrev(rev[ret]), 3);
  register_best_play(name, "random location!");
}
