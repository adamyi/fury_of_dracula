////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.h: the HunterView ADT
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-11-30	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#ifndef FOD__HUNTER_VIEW_H_
#define FOD__HUNTER_VIEW_H_

#include <stdbool.h>

#include "game.h"
#include "game_view.h"
#include "places.h"

typedef struct hunter_view *HunterView;

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
HunterView hv_new (char *past_plays, player_message messages[]);

/**
 * Frees all resources allocated for `hv`.
 * After this has been called, `hv` should not be accessed.
 */
void hv_drop (HunterView hv);

/**
 * Get the current round.
 */
round_t hv_get_round (HunterView hv);

/**
 * Get the current player; effectively, whose turn is it?
 */
enum player hv_get_player (HunterView hv);

/**
 * Get the current score, a positive integer between 0 and 366.
 */
int hv_get_score (HunterView hv);

/**
 * Get the current health points for a given player.
 * @param player specifies which players's life/blood points to return.
 * @returns a value between 0..9 for Hunters, or >0 for Dracula
 */
int hv_get_health (HunterView hv, enum player player);

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
location_t hv_get_location (HunterView hv, enum player player);

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
 *
 * Hunters can only get approximate information about where Dracula has
 * been (e.g. `SEA_UNKNOWN`, etc.), unless Dracula is at Castle Dracula,
 * or has been encountered
 */
void hv_get_trail (
	HunterView hv, enum player player, location_t trail[TRAIL_SIZE]);

/**
 * Return an array of `location_t`s giving all locations that the
 * current player could reach in their next move.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The current location should be included in the array.
 *
 * Road, rail, or sea connections should only be considered
 * if the `road`, `rail`, or `sea` parameters are true, respectively.
 */
location_t *hv_get_dests (
	HunterView hv, size_t *n_locations, bool road, bool rail, bool sea);

/**
 * Return an array of `location_t`s giving all of the locations that the
 * given `player` could reach from their current location.
 *
 * The array can be in any order but must contain unique entries.
 * The array size is stored at the variable referenced by `n_locations`.
 * The player's current location should be included in the array.
 *
 * `road`, `rail`, and `sea` connections should only be considered
 * if the `road`, `rail`, `sea` parameters are true, respectively.
 *
 * The function must take into account the current round for determining
 * how far `player` can travel by rail.
 *
 * If the player is Dracula, `n_locations` is set to 0, unless you know
 * Dracula's location precisely.
 */
location_t *hv_get_dests_player (
	HunterView hv, size_t *n_locations,
	enum player player, bool road, bool rail, bool sea);

#endif // !defined (FOD__HUNTER_VIEW_H_)
