////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// internal_game_view.h: the Game View ADT
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#ifndef FOD__INTERNAL_GAME_VIEW_H_
#define FOD__INTERNAL_GAME_VIEW_H_

#include <stdbool.h>

#include "game.h"
#include "myplayer.h"
#include "places.h"

typedef struct _game_view *_GameView;

typedef struct _game_view {
  round_t round;
  enum player current_player;
  int score;
  player_t *players[NUM_PLAYERS];
  // int traps[NUM_MAP_LOCATIONS];
  rollingarray_t *traps[NUM_MAP_LOCATIONS];
  location_t vampire;
  bool track_minions;
  int rests;  // for neural network feature generator
  location_t trail_last_loc;
  player_message messages[NUM_PLAYERS];
} _game_view;

/**
 * Creates a new view to summarise the current state of the game.
 *
 * @param past_plays is a string of all plays made in the game so far by
 *    all players (including Dracula) from earliest to most recent.
 *
 * @param messages is an array containing a `player_message` for each
 *    play in the game so far.  It will have exactly the same number of
 *    elements as there are plays in `past_plays`.  The message from the
 *    first play will be at index 0, and so on.  The contents of each
 *    `player_message` will be exactly as provided by the player.
 *
 * The "player_message" type is defined in game.h.
 * You are free to ignore messages if you wish.
 */

_GameView _gv_new(char *past_plays, player_message messages[],
                  bool track_minions);

char *_gv_get_msg(_game_view *gv, enum player player);

/**
 * Frees all resources allocated for `gv`.
 * After this has been called, `gv` should not be accessed.
 */
void _gv_drop(_GameView gv);

/**
 * Get the current round
 */
round_t _gv_get_round(_GameView gv);

/**
 * Get the current player; effectively, whose turn is it?
 */
enum player _gv_get_player(_GameView gv);

/**
 * Get the current score, a positive integer between 0 and 366.
 */
int _gv_get_score(_GameView gv);

/**
 * Get the current health points for a given player.
 * @param player specifies which players's life/blood points to return;
 * @returns a value between 0..9 for Hunters, or >0 for Dracula
 */
int _gv_get_health(_GameView gv, enum player player);

/**
 * Get the current location of a given player.
 *
 * May be `UNKNOWN_LOCATION` if the player has not had a turn yet
 * (i.e., at the beginning of the game when the round is 0)
 *
 * Possible values include:
 * - in the interval 0..70, if the player was (known to be) in a
 *   particular city or on a particular sea;
 * - `CITY_UNKNOWN`, if Dracula was known to be in a city;
 * - `SEA_UNKNOWN`, if Dracula was known to be at sea;
 * - `HIDE`, if Dracula was known to have made a hide move;
 * - `DOUBLE_BACK_n`, where n is [1...5], if Dracula was known to have
 *   made a double back move _n_ positions back in the trail; e.g.,
 *   `DOUBLE_BACK_1` is the last place place he visited; or
 * - `TELEPORT`, if Dracula apparated back to Castle Dracula.
 */
location_t _gv_get_location(_GameView gv, enum player player);

// same as above but replace HIDE, DOUBLEBACK, TELEPORT with actural loc
location_t _gv_get_real_location(_GameView gv, enum player player);

/**
 * Fills the trail array with the locations of the last 6 turns for the
 * given player.
 *
 * If the move does not exist yet (i.e., the start of the game),
 * the value should be UNKNOWN_LOCATION (-1).
 *
 * For example after 2 turns the array may have the contents
 *
 *     {29, 12, -1, -1, -1, -1}
 *
 * This would mean in the first move the player started on location 12
 * then moved to the current location of 29.
 */
void _gv_get_move_history(_GameView gv, enum player player,
                          location_t trail[TRAIL_SIZE]);
void _gv_get_location_history(_GameView gv, enum player player,
                              location_t trail[TRAIL_SIZE]);

/**
 * Return an array of `location_t`s giving all of the locations that the
 * given `player` could reach from their current location, assuming it's
 * currently `round`.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The player's current location should be included in the array.
 *
 * `road`, `rail`, and `sea` connections should only be considered
 * if the `road`, `rail`, `sea` parameters are true, respectively.
 *
 * The function must take into account the current round and player for
 * determining how far `player` can travel by rail.
 *
 * When `player` is `PLAYER_DRACULA`, the function must take into
 * account (many of) the rules around Dracula's movements, such as that
 * Dracula may not go to the hospital, and may not travel by rail.
 * It need not take into account the trail restriction.
 */
location_t *_gv_get_connections(_GameView gv, size_t *n_locations,
                                location_t from, enum player player,
                                round_t round, bool road, bool rail, bool sea);
location_t *_gv_get_connections_with_trail(_GameView gv, size_t *n_locations,
                                           location_t from, enum player player,
                                           round_t round, bool road, bool rail,
                                           bool sea);

location_t *_gv_do_get_connections(player_t *pobj, size_t *n_locations,
                                   location_t from, enum player player,
                                   round_t round, bool road, bool rail,
                                   bool sea, bool trail, bool stay, bool hide);

void _gv_get_locale_info(_GameView gv, location_t where, int *n_traps,
                         int *n_vamps);

void parse_dracula_minion_placement(_game_view *gv, location_t real_loc,
                                    char minion);
void parse_dracula_minion_left_trail(_game_view *gv, char left);
void parse_hunter_encounter(_game_view *gv, enum player pid,
                            location_t real_loc, char encounter);

player_t *_gv_get_player_class(_GameView gv, enum player player);

#endif  // !defined (FOD__INTERNAL_GAME_VIEW_H_)
