////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
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
#include "game_view.h"
#include "map.h"
#include "mapdata.h"
#include "player.h"

static void gv_get_connections_do(struct game_view *gv, size_t *n_locations,
                                  bool *can_go, location_t from,
                                  enum player player, round_t round, bool road,
                                  bool rail, bool sea, int max_rail_dist);

static inline int get_rail_travel_dist(struct game_view *gv, round_t round,
                                       enum player player);

typedef struct game_view {
  round_t round;
  enum player current_player;
  int score;
  player_t *players[NUM_PLAYERS];
  int traps[NUM_MAP_LOCATIONS];
  location_t vampire;
} game_view;

// parse 7-char move
char *parse_move(char *move, game_view *gv) {
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
    if (loc == SEA_UNKNOWN)
      is_sea = true;
    else if ((loc >= DOUBLE_BACK_1 &&
              loc <= DOUBLE_BACK_5)) {  // no need for HIDE
      int b = loc - DOUBLE_BACK_1;      // 1: stay at current
      location_t last_loc =
          rollingarray_get_item_backwards(gv->players[pid]->trail, b);
      is_sea = (last_loc == SEA_UNKNOWN) ||
               (last_loc >= MIN_MAP_LOCATION && last_loc <= MAX_MAP_LOCATION &&
                location_get_type(last_loc) == SEA);
    }
  }
  if (loc == NOWHERE) {
    loc = location_find_by_abbrev(move);
    is_sea = (location_get_type(loc) == SEA);
  }

  move += 2;

  // get first location in trail for trap to invalidate
  location_t loc_trail_first = NOWHERE;
  if (gv->round >= TRAIL_SIZE && pid == PLAYER_DRACULA)
    loc_trail_first = rollingarray_get_item(gv->players[pid]->trail, 0);

  player_move_to(gv->players[pid], loc);
  if (pid == PLAYER_DRACULA && is_sea)
    gv->players[pid]->health -=
        LIFE_LOSS_SEA;  // TODO(adamyi): check if game ends

  if (pid == PLAYER_DRACULA) {
    // parse minion
    if (move[0] == 'T') {
      ac_log(AC_LOG_DEBUG, "placed trap");
      if (loc >= MIN_MAP_LOCATION && loc <= MAX_MAP_LOCATION) gv->traps[loc]++;
    } else if (move[1] == 'V') {
      ac_log(AC_LOG_DEBUG, "placed vampire");
      gv->vampire = loc;
    }
    move += 2;

    // parse left trail
    if (*move == 'M') {
      ac_log(AC_LOG_DEBUG, "trap invalidates");
      if (loc_trail_first >= MIN_MAP_LOCATION &&
          loc_trail_first <= MAX_MAP_LOCATION)
        gv->traps[loc_trail_first]--;
    } else if (*move == 'V') {
      ac_log(AC_LOG_DEBUG, "vampire matures");
      gv->vampire = NOWHERE;
      gv->score -= 13;
    }
    move += 2;

    gv->round++;
    gv->current_player = 0;
    gv->score -= SCORE_LOSS_DRACULA_TURN;
  } else {
    // parse encounter
    for (int i = 0; i < 4; i++) {
      switch (*move) {
        case 'T':
          ac_log(AC_LOG_DEBUG, "encounter trap");
          gv->traps[loc]--;
          // TODO(adamyi): send to hospital
          gv->players[pid]->health -= 2;
          break;
        case 'V':
          ac_log(AC_LOG_DEBUG, "encounter immature vampire");
          gv->vampire = NOWHERE;
          break;
        case 'D':
          // TODO(adamyi): send to hospital/end game
          ac_log(AC_LOG_DEBUG, "encounter dracula");
          gv->players[PLAYER_DRACULA]->health -= LIFE_LOSS_HUNTER_ENCOUNTER;
          gv->players[pid]->health -= LIFE_LOSS_DRACULA_ENCOUNTER;
          break;
        default:
          // encountered nothing
          break;
      }
      move++;
    }

    gv->current_player++;
  }

  return move;
}

game_view *gv_new(char *past_plays, player_message messages[]) {
  ac_setLoggingTag("game_view");
  ac_log(AC_LOG_DEBUG, "Creating new GameView based on past_plays string: %s",
         past_plays);
  game_view *new = malloc(sizeof *new);
  if (new == NULL) err(EX_OSERR, "couldn't allocate GameView");
  new->round = 0;
  new->current_player = 0;
  new->score = GAME_START_SCORE;
  new->vampire = NOWHERE;
  memset(new->traps, 0, NUM_MAP_LOCATIONS * sizeof(int));
  for (int i = 0; i < NUM_PLAYERS; i++) new->players[i] = new_player(i);
  while (*past_plays != '\0') past_plays = parse_move(past_plays, new);

  return new;
}

void gv_drop(game_view *gv) {
  for (int i = 0; i < NUM_PLAYERS; i++) destroy_player(gv->players[i]);
  free(gv);
}

round_t gv_get_round(game_view *gv) { return gv->round; }

enum player gv_get_player(game_view *gv) { return gv->current_player; }

int gv_get_score(game_view *gv) { return gv->score; }

int gv_get_health(game_view *gv, enum player player) {
  return player_get_health(gv->players[player]);
}

location_t gv_get_location(game_view *gv, enum player player) {
  return player_get_location(gv->players[player]);
}

void gv_get_history(game_view *gv, enum player player,
                    location_t trail[TRAIL_SIZE]) {
  player_get_trail(gv->players[player], trail);
}

static inline int get_rail_travel_dist(game_view *gv, round_t round,
                                       enum player player) {
  if (player == PLAYER_DRACULA) {
    return 0;
  } else {
    return (round + player) % 4;
  }
}

void gv_get_connections_do(game_view *gv, size_t *n_locations, bool *can_go,
                           location_t from, enum player player, round_t round,
                           bool road, bool rail, bool sea, int max_rail_dist) {
  if ((!road) && ((!rail) || max_rail_dist <= 0) && (!sea)) return;

  struct adj_connection *conns = getConnections(from);

  for (int i = 0; i < ADJLIST_COUNT[from]; i++) {
    if (conns[i].type == ROAD && road) {
      can_go[conns[i].v]++;
      (*n_locations)++;
    } else if (conns[i].type == BOAT && sea) {
      can_go[conns[i].v]++;
      (*n_locations)++;
    } else if (conns[i].type == RAIL && rail && max_rail_dist > 0 &&
               conns[i].v != from) {
      can_go[conns[i].v]++;
      (*n_locations)++;
      gv_get_connections_do(gv, n_locations, can_go, conns[i].v, player, round,
                            false, rail, false, max_rail_dist - 1);
    }
  }
}

location_t *gv_get_connections(game_view *gv, size_t *n_locations,
                               location_t from, enum player player,
                               round_t round, bool road, bool rail, bool sea) {
  bool can_go[NUM_MAP_LOCATIONS];
  memset(can_go, 0, NUM_MAP_LOCATIONS);

  can_go[from] = true;
  *n_locations = 1;

  int max_rail_dist = 0;
  if (rail) {
    max_rail_dist = get_rail_travel_dist(gv, round, player);
  }

  gv_get_connections_do(gv, n_locations, can_go, from, player, round, road,
                        rail, sea, max_rail_dist);

  location_t *valid_conns = malloc(sizeof(location_t) * (*n_locations));
  for (size_t i = 0, j = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (can_go[i]) valid_conns[j++] = i;
  }

  return valid_conns;
}
