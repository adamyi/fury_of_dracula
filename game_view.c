////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// game_view.c: GameView ADT implementation
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "ac_log.h"

#include "game.h"
#include "game_view.h"
#include "map.h"
#include "mapdata.h"
#include "player.h"

typedef struct game_view {
  round_t round;
  enum player current_player;
  int score;
  player_t *players[NUM_PLAYERS];
  // TODO: minions
  // TODO(hunt): messages
} game_view;

// parse 7
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
  if (pid == PLAYER_DRACULA) loc = special_location_find_by_abbrev(move);
  if (loc == NOWHERE) loc = location_find_by_abbrev(move);

  move += 2;

  if (pid == PLAYER_DRACULA) {
    // parse minion
    if (move[0] == 'T') {
      ac_log(AC_LOG_DEBUG, "placed trap");
      // TODO: placd trap
    } else if (move[1] == 'V') {
      ac_log(AC_LOG_DEBUG, "placed vampire");
      // TODO: placed vampire
    }
    move += 2;

    // parse left trail
    if (*move == 'M') {
      ac_log(AC_LOG_DEBUG, "trap invalidates");
      // TODO; Trap invalidates
    } else if (*move == 'V') {
      ac_log(AC_LOG_DEBUG, "vampire matures");
      // TODO: vampire matures
    }
    move += 2;
  } else {
    // parse encounter
    for (int i = 0; i < 4; i++) {
      switch (*move) {
        case 'T':
          // TODO: encountered trap
          break;
        case 'V':
          // TODO: encountered immature vampire
          break;
        case 'D':
          // TODO: encountered Dracula
          break;
        default:
          // encountered nothing
          break;
      }
      move++;
    }
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
  rollingarray_to_array(gv->players[player]->trail, trail);
}

location_t *gv_get_connections(game_view *gv, size_t *n_locations,
                               location_t from, enum player player,
                               round_t round, bool road, bool rail, bool sea) {
  // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
  *n_locations = 0;
  return NULL;
}
