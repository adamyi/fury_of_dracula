////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// internal_game_view.c: GameView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <math.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "ac_log.h"
#include "ac_memory.h"

#include "game.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"
#include "myplayer.h"

static inline int get_rail_travel_dist(round_t round, enum player player);

static inline void hunter_lose_health(_game_view *gv, enum player player,
                                      int lose) {
  if (player_lose_health(gv->players[player], lose)) {
    gv->players[player]->location = HOSPITAL_LOCATION;
    gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
  }
}

void parse_dracula_minion_placement(_game_view *gv, location_t real_loc,
                                    char minion) {
  if (minion == 'T') {
    ac_log(AC_LOG_DEBUG, "placed trap at %s(%s)", location_get_abbrev(real_loc),
           location_get_name(real_loc));
    if (gv->track_minions) {
      gv->traps[real_loc]++;
      ac_log(AC_LOG_DEBUG, "there are %d traps there", gv->traps[real_loc]);
    }
  } else if (minion == 'V') {
    ac_log(AC_LOG_DEBUG, "placed vampire as %s(%s)",
           location_get_abbrev(real_loc), location_get_name(real_loc));
    if (gv->track_minions) gv->vampire = real_loc;
  }
}

void parse_dracula_minion_left_trail(_game_view *gv, char left) {
  if (left == 'M') {
    ac_log(AC_LOG_DEBUG, "trap invalidates");
    if (gv->track_minions && gv->round >= TRAIL_SIZE) {
      gv->traps[gv->trail_last_loc]--;
      ac_log(AC_LOG_DEBUG, "at %s(%s), there are %d left",
             location_get_abbrev(gv->trail_last_loc),
             location_get_name(gv->trail_last_loc),
             gv->traps[gv->trail_last_loc]);
    }
  } else if (left == 'V') {
    ac_log(AC_LOG_DEBUG, "vampire matures");
    if (gv->track_minions) gv->vampire = NOWHERE;
    gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
  }
}

static inline char *parse_dracula_move(char *move, _game_view *gv,
                                       enum player pid, bool is_sea,
                                       location_t real_loc) {
  if (is_sea) player_lose_health(gv->players[pid], LIFE_LOSS_SEA);
  if (real_loc == CASTLE_DRACULA)
    gv->players[pid]->health += LIFE_GAIN_CASTLE_DRACULA;
  gv->rests = 0;

  // parse minion
  parse_dracula_minion_placement(gv, real_loc, *(move++));
  parse_dracula_minion_placement(gv, real_loc, *(move++));

  // parse left trail
  parse_dracula_minion_left_trail(gv, *move);
  move += 2;

  gv->round++;
  gv->current_player = 0;
  gv->score -= SCORE_LOSS_DRACULA_TURN;

  return move;
}

void parse_hunter_encounter(_game_view *gv, enum player pid,
                            location_t real_loc, char encounter) {
  switch (encounter) {
    case 'T':
      ac_log(AC_LOG_DEBUG, "encounter trap");
      if (gv->track_minions) gv->traps[real_loc]--;
      hunter_lose_health(gv, pid, LIFE_LOSS_TRAP_ENCOUNTER);
      break;
    case 'V':
      ac_log(AC_LOG_DEBUG, "encounter immature vampire");
      if (gv->track_minions) gv->vampire = NOWHERE;
      break;
    case 'D':
      ac_log(AC_LOG_DEBUG, "encounter dracula");
      player_lose_health(gv->players[PLAYER_DRACULA],
                         LIFE_LOSS_HUNTER_ENCOUNTER);
      hunter_lose_health(gv, pid, LIFE_LOSS_DRACULA_ENCOUNTER);
      break;
    default:
      // encountered nothing
      break;
  }
}

static inline char *parse_hunter_move(char *move, _game_view *gv,
                                      enum player pid, location_t old_loc,
                                      location_t real_loc) {
  ac_log(AC_LOG_ERROR, "oldloc %d newloc %d", old_loc, real_loc);
  if (old_loc == real_loc) {
    gv->players[pid]->health += LIFE_GAIN_REST;
    ac_log(AC_LOG_ERROR, "player %d gain health %d to %d", pid, LIFE_GAIN_REST, gv->players[pid]->health);

    gv->rests++;
  }
  // parse encounter
  for (int i = 0; i < 4; i++)
    parse_hunter_encounter(gv, pid, real_loc, *(move++));

  if (gv->players[pid]->health > GAME_START_HUNTER_LIFE_POINTS)
    gv->players[pid]->health = GAME_START_HUNTER_LIFE_POINTS;

  gv->current_player++;

  return move;
}

// parse 7-char move
char *parse_move(char *move, _game_view *gv) {
  // move format (hunter):
  // Player[1] Location[2] Encounter[4]
  // move format (dracula):
  // Player[1] Location[2] Minion[2] LeftTrail[1] .
  if (*move == ' ') move++;

  // parse player
  enum player pid = player_id_from_char(*move);
  move++;

  // parse location
  location_t loc = NOWHERE;
  bool is_sea = false;
  if (pid == PLAYER_DRACULA) {
    loc = special_location_find_by_abbrev(move);
    if (loc == SEA_UNKNOWN) is_sea = true;
  } else if (gv->players[pid]->health <= 0) {
    gv->players[pid]->health = GAME_START_HUNTER_LIFE_POINTS;
  }

  if (loc == NOWHERE) {
    loc = location_find_by_abbrev(move);
    is_sea = (location_get_type(loc) == SEA);
  }
  location_t real_loc = loc;

  if ((loc >= DOUBLE_BACK_1 && loc <= DOUBLE_BACK_5) || loc == HIDE) {
    int b = (loc == HIDE) ? 0 : loc - DOUBLE_BACK_1;  // 1: stay at current
    real_loc =
        rollingarray_get_item_backwards(gv->players[pid]->location_history, b);
    is_sea = (real_loc == SEA_UNKNOWN) ||
             (real_loc >= MIN_MAP_LOCATION && real_loc <= MAX_MAP_LOCATION &&
              location_get_type(real_loc) == SEA);
  } else if (loc == TELEPORT) {
    real_loc = CASTLE_DRACULA;
  }

  location_t old_loc = player_get_location(gv->players[pid]);

  if (gv->round >= TRAIL_SIZE) {
    gv->trail_last_loc =
        rollingarray_get_item(gv->players[pid]->location_history, 0);
  }

  ac_log(AC_LOG_INFO, "player %d at %s (%s) makes move %s (%s) to %s (%s)", pid,
         location_get_abbrev(old_loc), location_get_name(old_loc),
         location_get_abbrev(loc), location_get_name(loc),
         location_get_abbrev(real_loc), location_get_name(real_loc));
  player_move_to(gv->players[pid], real_loc, loc);

  move += 2;

  if (pid == PLAYER_DRACULA)
    return parse_dracula_move(move, gv, pid, is_sea, real_loc);
  return parse_hunter_move(move, gv, pid, old_loc, real_loc);
}

_game_view *_gv_new(char *past_plays,
                    player_message messages[] __attribute__((unused)),
                    bool track_minions) {
  ac_setLoggingTag("_game_view");
  ac_log(AC_LOG_DEBUG, "Creating new GameView based on past_plays string: %s",
         past_plays);
  _game_view *new = ac_malloc(sizeof *new, "new game view");
  new->round = 0;
  new->current_player = 0;
  new->score = GAME_START_SCORE;
  new->vampire = NOWHERE;
  new->rests = 0;
  new->track_minions = track_minions;
  new->trail_last_loc = NOWHERE;
  memset(new->traps, 0, NUM_MAP_LOCATIONS * sizeof(int));
  for (int i = 0; i < NUM_PLAYERS; i++) new->players[i] = new_player(i);
  while (*past_plays != '\0') past_plays = parse_move(past_plays, new);

  // TODO(adamyi): messages
  return new;
}

void _gv_drop(_game_view *gv) {
  for (int i = 0; i < NUM_PLAYERS; i++) destroy_player(gv->players[i]);
  free(gv);
}

round_t _gv_get_round(_game_view *gv) { return gv->round; }

enum player _gv_get_player(_game_view *gv) { return gv->current_player; }

int _gv_get_score(_game_view *gv) { return gv->score; }

int _gv_get_health(_game_view *gv, enum player player) {
  return player_get_health(gv->players[player]);
}

location_t _gv_get_location(_game_view *gv, enum player player) {
  location_t last_move = rollingarray_last_item(gv->players[player]->trail);
  if (last_move >= MIN_MAP_LOCATION && last_move <= MAX_MAP_LOCATION)
    return _gv_get_real_location(gv, player);
  return last_move;
}

location_t _gv_get_real_location(_game_view *gv, enum player player) {
  return player_get_location(gv->players[player]);
}

void _gv_get_move_history(_game_view *gv, enum player player,
                          location_t trail[TRAIL_SIZE]) {
  player_get_trail(gv->players[player], trail);
}

void _gv_get_location_history(_game_view *gv, enum player player,
                              location_t trail[TRAIL_SIZE]) {
  player_get_location_history(gv->players[player], trail);
}

static inline int get_rail_travel_dist(round_t round, enum player player) {
  if (player == PLAYER_DRACULA) return 0;
  return (round + player) % 4;
}

static void _gv_get_connections_rec(_game_view *gv, size_t *n_locations,
                                    bool *can_go, location_t from,
                                    enum player player, round_t round,
                                    bool road, bool rail, bool sea,
                                    int max_rail_dist) {
  ac_log(AC_LOG_DEBUG,
         "invoking _gv_get_connections_rec with settings: road %d rail %d sea "
         "%d max_rail_dist %d",
         road, rail, sea, max_rail_dist);
  if ((!road) && ((!rail) || max_rail_dist <= 0) && (!sea)) return;

  struct adj_connection *conns = getConnections(from);

  for (int i = 0; i < ADJLIST_COUNT[from]; i++) {
    if (player == PLAYER_DRACULA && conns[i].v == HOSPITAL_LOCATION) continue;
    if (conns[i].type == ROAD && road) {
      if (!can_go[conns[i].v]) {
        can_go[conns[i].v] = true;
        (*n_locations)++;
      }
    } else if (conns[i].type == BOAT && sea) {
      if (!can_go[conns[i].v]) {
        can_go[conns[i].v] = true;
        (*n_locations)++;
      }
    } else if (conns[i].type == RAIL && rail && max_rail_dist > 0 &&
               conns[i].v != from) {
      if (!can_go[conns[i].v]) {
        can_go[conns[i].v] = true;
        (*n_locations)++;
      }
      _gv_get_connections_rec(gv, n_locations, can_go, conns[i].v, player,
                              round, false, rail, false, max_rail_dist - 1);
    }
  }
}

location_t *_gv_do_get_connections(_game_view *gv, size_t *n_locations,
                                   location_t from, enum player player,
                                   round_t round, bool road, bool rail,
                                   bool sea, bool trail, bool stay, bool hide) {
  ac_log(AC_LOG_DEBUG,
         "invoking _gv_do_get_connections with settings: road %d rail %d sea "
         "%d trail %d stay %d hide %d",
         road, rail, sea, trail, stay, hide);
  if (from < MIN_MAP_LOCATION ||
      from > MAX_MAP_LOCATION)  // don't know exact loc
    return 0;

  if (player == PLAYER_DRACULA) rail = false;

  bool can_go[NUM_MAP_LOCATIONS];
  bool doublebacks[5];
  memset(can_go, 0, NUM_MAP_LOCATIONS);
  memset(doublebacks, 0, 5);

  *n_locations = 0;
  int dbs = 0;

  int max_rail_dist = 0;
  if (rail) {
    max_rail_dist = get_rail_travel_dist(round, player);
  }

  _gv_get_connections_rec(gv, n_locations, can_go, from, player, round, road,
                          rail, sea, max_rail_dist);

  bool candb = hide && player == PLAYER_DRACULA;
  bool canhide = candb && location_get_type(from) != SEA;
  if (trail) {
    location_t hist[TRAIL_SIZE];
    player_get_trail(gv->players[player], hist);
    for (int i = TRAIL_SIZE - 2; i >= 0; i--) {
      if (hist[i] >= MIN_MAP_LOCATION && hist[i] <= MAX_MAP_LOCATION) {
        if (can_go[hist[i]]) {
          can_go[hist[i]] = false;
          (*n_locations)--;
        }
        ac_log(AC_LOG_DEBUG, "candb consider: %d %d %d", hist[i], candb, from);
        ac_log(AC_LOG_DEBUG, "candb consider: %s %d %s", location_get_name(hist[i]), candb, location_get_name(from));
        if (candb && (from == hist[i] || isConnected(hist[i], from))) {
          ac_log(AC_LOG_DEBUG, "candb: %d", i);
          doublebacks[i] = true;
          dbs++;
        }
      } else if (hist[i] == HIDE) {
        canhide = false;
      } else if (hist[i] >= DOUBLE_BACK_1 && hist[i] <= DOUBLE_BACK_5) {
        candb = false;
      }
    }
    /* if (hist[0] >= MIN_MAP_LOCATION && hist[0] <= MAX_MAP_LOCATION &&
        location_get_type(hist[0]) == SEA)
      canhide = false; */
  }

  if (canhide) (*n_locations)++;

  if (stay && (!can_go[from])) {
    can_go[from] = true;
    (*n_locations)++;
  }

  if (candb)
    *n_locations += dbs;

  if (hide && (*n_locations) == 0)
    *n_locations = 1;

  location_t *valid_conns = ac_malloc(sizeof(location_t) * (*n_locations),
                                      "connections array for gv");
  size_t j = 0;
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (can_go[i]) valid_conns[j++] = i;
  }
  if (canhide) valid_conns[j++] = HIDE;
  if (candb) {
    for (int i = 0, k = DOUBLE_BACK_1; i < 5; i++, k++) {
      if (doublebacks[i]) {
        valid_conns[j++] = k;
      }
    }
  }
  if (hide && j == 0) {
    valid_conns[0] = TELEPORT;
  }

  for (int i = 0; i < *n_locations; i++) {
    ac_log(AC_LOG_ERROR, "! %d", valid_conns[i]);
    ac_log(AC_LOG_ERROR, "! %s", location_get_name(valid_conns[i]));
     //printf("%s ", location_get_name(valid_conns[i]));
  }

  return valid_conns;
}

location_t *_gv_get_connections(_game_view *gv, size_t *n_locations,
                                location_t from, enum player player,
                                round_t round, bool road, bool rail, bool sea) {
  return _gv_do_get_connections(gv, n_locations, from, player, round, road,
                                rail, sea, false, true, false);
}

location_t *_gv_get_connections_with_trail(_game_view *gv, size_t *n_locations,
                                           location_t from, enum player player,
                                           round_t round, bool road, bool rail,
                                           bool sea) {
  return _gv_do_get_connections(gv, n_locations, from, player, round, road,
                                rail, sea, true, true, false);
}

void _gv_get_locale_info(_game_view *gv, location_t where, int *n_traps,
                         int *n_vamps) {
  assert(gv->track_minions);
  *n_vamps = (gv->vampire == where);
  *n_traps = gv->traps[where];
}
