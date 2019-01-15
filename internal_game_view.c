////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// internal_game_view.c: GameView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "ac_log.h"

#include "game.h"
#include "internal_game_view.h"
#include "map.h"
#include "mapdata.h"
#include "player.h"

static inline int get_rail_travel_dist(round_t round, enum player player);

typedef struct _game_view {
  round_t round;
  enum player current_player;
  int score;
  player_t *players[NUM_PLAYERS];
  int traps[NUM_MAP_LOCATIONS];
  location_t vampire;
  bool track_minions;
} _game_view;

static inline void hunter_lose_health(_game_view *gv, enum player player,
                                      int lose) {
  if (player_lose_health(gv->players[player], lose)) {
    gv->players[player]->location = ST_JOSEPH_AND_ST_MARYS;
    gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
  }
}

static inline char *parse_dracula_move(char *move, _game_view *gv,
                                       enum player pid, bool is_sea,
                                       location_t real_loc) {
  if (is_sea) player_lose_health(gv->players[pid], LIFE_LOSS_SEA);

  // parse minion
  if (move[0] == 'T') {
    ac_log(AC_LOG_DEBUG, "placed trap");
    if (gv->track_minions) {
      if (gv->traps[real_loc] >= 3)
        ac_log(AC_LOG_FATAL, "shouldn't place a trap because already 3!");
      gv->traps[real_loc]++;
    }
  } else if (move[1] == 'V') {
    ac_log(AC_LOG_DEBUG, "placed vampire");
    if (gv->track_minions) gv->vampire = real_loc;
  }
  move += 2;

  // parse left trail
  if (*move == 'M') {
    ac_log(AC_LOG_DEBUG, "trap invalidates");
    if (gv->track_minions && gv->round >= TRAIL_SIZE)
      gv->traps[rollingarray_get_item(gv->players[pid]->location_history, 0)]--;
  } else if (*move == 'V') {
    ac_log(AC_LOG_DEBUG, "vampire matures");
    if (gv->track_minions) gv->vampire = NOWHERE;
    gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
  }
  move += 2;

  gv->round++;
  gv->current_player = 0;
  gv->score -= SCORE_LOSS_DRACULA_TURN;

  return move;
}

static inline char *parse_hunter_move(char *move, _game_view *gv,
                                      enum player pid, location_t old_loc,
                                      location_t real_loc) {
  if (old_loc == real_loc) gv->players[pid]->health += LIFE_GAIN_REST;
  // parse encounter
  for (int i = 0; i < 4; i++) {
    switch (*move) {
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
    move++;
  }

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
  _game_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate GameView");
  new->round = 0;
  new->current_player = 0;
  new->score = GAME_START_SCORE;
  new->vampire = NOWHERE;
  new->track_minions = track_minions;
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
  if ((!road) && ((!rail) || max_rail_dist <= 0) && (!sea)) return;

  struct adj_connection *conns = getConnections(from);

  for (int i = 0; i < ADJLIST_COUNT[from]; i++) {
    if (player == PLAYER_DRACULA && conns[i].v == ST_JOSEPH_AND_ST_MARYS)
      continue;
    if (conns[i].type == ROAD && road) {
      can_go[conns[i].v] = true;
      (*n_locations)++;
    } else if (conns[i].type == BOAT && sea) {
      can_go[conns[i].v] = true;
      (*n_locations)++;
    } else if (conns[i].type == RAIL && rail && max_rail_dist > 0 &&
               conns[i].v != from) {
      can_go[conns[i].v] = true;
      (*n_locations)++;
      _gv_get_connections_rec(gv, n_locations, can_go, conns[i].v, player,
                              round, false, rail, false, max_rail_dist - 1);
    }
  }
}

location_t *_gv_do_get_connections(_game_view *gv, size_t *n_locations,
                                   location_t from, enum player player,
                                   round_t round, bool road, bool rail,
                                   bool sea, bool trail) {
  if (from < MIN_MAP_LOCATION ||
      from > MAX_MAP_LOCATION)  // don't know exact loc
    return 0;

  if (player == PLAYER_DRACULA) rail = false;

  bool can_go[NUM_MAP_LOCATIONS];
  memset(can_go, 0, NUM_MAP_LOCATIONS);

  *n_locations = 0;

  int max_rail_dist = 0;
  if (rail) {
    max_rail_dist = get_rail_travel_dist(round, player);
  }

  _gv_get_connections_rec(gv, n_locations, can_go, from, player, round, road,
                          rail, sea, max_rail_dist);

  if (trail) {
    location_t hist[TRAIL_SIZE];
    player_get_trail(gv->players[player], hist);
    for (int i = TRAIL_SIZE - 2; i >= 0; i--) {
      if (hist[i] >= MIN_MAP_LOCATION && hist[i] <= MAX_MAP_LOCATION) {
        if (can_go[hist[i]]) {
          can_go[hist[i]] = false;
          (*n_locations)--;
        }
      }
    }
  }

  if (!can_go[from]) {
    can_go[from] = true;
    (*n_locations)++;
  }

  location_t *valid_conns = malloc(sizeof(location_t) * (*n_locations));
  for (size_t i = 0, j = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (can_go[i]) valid_conns[j++] = i;
  }

  return valid_conns;
}

location_t *_gv_get_connections(_game_view *gv, size_t *n_locations,
                                location_t from, enum player player,
                                round_t round, bool road, bool rail, bool sea) {
  return _gv_do_get_connections(gv, n_locations, from, player, round, road,
                                rail, sea, false);
}

location_t *_gv_get_connections_with_trail(_game_view *gv, size_t *n_locations,
                                           location_t from, enum player player,
                                           round_t round, bool road, bool rail,
                                           bool sea) {
  return _gv_do_get_connections(gv, n_locations, from, player, round, road,
                                rail, sea, true);
}

void _gv_get_locale_info(_game_view *gv, location_t where, int *n_traps,
                         int *n_vamps) {
  assert(gv->track_minions);
  *n_vamps = (gv->vampire == where);
  *n_traps = gv->traps[where];
}
