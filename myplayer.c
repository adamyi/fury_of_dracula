#include <memory.h>
#include <stdbool.h>
#include <string.h>

#include "ac_log.h"
#include "ac_memory.h"

#include "game.h"
#include "myplayer.h"
#include "places.h"
#include "rollingarray.h"

player_t *new_player(enum player id, bool track_all_history) {
  player_t *player = ac_malloc(sizeof(player_t), "new player");
  player->id = id;
  if (id == PLAYER_DRACULA)
    player->health = GAME_START_BLOOD_POINTS;
  else
    player->health = GAME_START_HUNTER_LIFE_POINTS;
  player->location = UNKNOWN_LOCATION;
  player->trail = new_rollingarray(TRAIL_SIZE);
  player->location_history = new_rollingarray(TRAIL_SIZE);
  if (track_all_history) {
    player->all_location_history = ac_malloc(MAX_ROUNDS * sizeof(location_t),
                                             "player->all_location_history");
    memset(player->all_location_history, NOWHERE,
           MAX_ROUNDS * sizeof(location_t));  // -1
    player->all_history_size = 0;
  } else {
    player->all_history_size = -1;
  }
  return player;
}

player_t *clone_player(player_t *p) {
  player_t *new = ac_malloc(sizeof(player_t), "clone player");
  new->id = p->id;
  new->health = p->health;
  new->location = p->location;
  new->trail = clone_rollingarray(p->trail);
  new->location_history = clone_rollingarray(p->location_history);
  new->all_history_size = p->all_history_size;
  if (new->all_history_size >= 0) {
    // TODO(adamyi): memcpy
  }
  return new;
}

void destroy_player(player_t *player) {
  destroy_rollingarray(player->trail);
  destroy_rollingarray(player->location_history);
  if (player->all_history_size >= 0) free(player->all_location_history);
  free(player);
}

int player_get_health(player_t *player) { return player->health; }

bool player_lose_health(player_t *player, int lose) {
  ac_log(AC_LOG_DEBUG, "Player %d with health %d loses health %d", player->id,
         player->health, lose);
  player->health -= lose;
  if (player->health <= 0) {
    player->health = 0;
    return true;
  }
  return false;
}

location_t player_get_location(player_t *player) { return player->location; }

void player_get_trail(player_t *player, location_t trail[TRAIL_SIZE]) {
  rollingarray_to_array(player->trail, trail, true);
}

void player_get_location_history(player_t *player,
                                 location_t history[TRAIL_SIZE]) {
  rollingarray_to_array(player->location_history, history, true);
}

// NOTE: this doesn't take care of value check, losing blood, and spawning
// minions
void player_move_to(player_t *player, location_t location, location_t move) {
  // player->location = location;
  player->location = location;
  rollingarray_add_item(player->trail, move);
  rollingarray_add_item(player->location_history, location);
  if (player->all_history_size >= 0) {
    // player->all_location_history[player->all_history_size++] = location;
    player->all_location_history[player->all_history_size++] = move;
  }
}

location_t player_resolve_move_location(player_t *player, location_t move) {
  if (move == HIDE) return player->location;
  if (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)
    return rollingarray_get_item_backwards(player->location_history,
                                           move - DOUBLE_BACK_1);
  if (move == TELEPORT) return CASTLE_DRACULA;
  return move;
}

enum player player_id_from_char(char player) {
  switch (player) {
    case 'G':
      return PLAYER_LORD_GODALMING;
    case 'S':
      return PLAYER_DR_SEWARD;
    case 'H':
      return PLAYER_VAN_HELSING;
    case 'M':
      return PLAYER_MINA_HARKER;
    case 'D':
      return PLAYER_DRACULA;
    default:
      ac_log(AC_LOG_FATAL, "Unknown player");
  }
  __builtin_unreachable();
}
