#include <stdbool.h>

#include "game.h"
#include "places.h"
#include "player.h"
#include "rollingarray.h"

player_t *new_player(enum player id) {
  player_t *player = malloc(sizeof(player_t));
  player->id = id;
  if (id == PLAYER_DRACULA)
    player->health = GAME_START_BLOOD_POINTS;
  else
    player->health = GAME_START_HUNTER_LIFE_POINTS;
  player->location = UNKNOWN_LOCATION;
  player->trail = new_rollingarray(TRAIL_SIZE);
  return player;
}

void destroy_player(player_t *player) {
  destroy_rollingarray(player->trail);
  free(player);
}

int player_get_health(player_t *player) { return player->health; }

bool player_lose_health(player_t *player, int lose) {
  player->health -= lose;
  return (player->health <= 0);
}

location_t player_get_location(player_t *player) { return player->location; }

void player_get_trail(player_t *player, location_t trail[TRAIL_SIZE]) {
  rollingarray_to_array(player->trail, trail);
}

// NOTE: this doesn't take care of value check, losing blood, and spawning
// minions
void player_move_to(player_t *player, location_t location) {
  player->location = location;
  rollingarray_add_item(player->trail, location);
}
