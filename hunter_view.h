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

// newHunterView() creates a new game view to summarise the current
// state of the game.
//
// pastPlays is a string of all the plays made in the game so far by all
// players (including Dracula) from earliest to most recent.
//
// For Dracula
// * the pastPlays string should contain full details of all moves
// * the last move should always be by Mina Harker
//
// messages is an array containing a playerMessage for each play in
// the game so far. It will have exactly the same number of elements
// as there are plays in pastPlays. The message from the first play
// will be at index 0, and so on.  The contents of each playerMessage
// will be exactly as provided by the player.
//
// The "PlayerMessage" type is defined in Game.h.
// You are free to ignore messages if you wish.

HunterView newHunterView (char *pastPlays, player_message messages[]);


// disposeHunterView() frees all memory previously allocated for the HunterView
// toBeDeleted. toBeDeleted should not be accessed after the call.

void disposeHunterView (HunterView toBeDeleted);


// Functions to return simple information about the current state of the game

// Get the current round

round_t giveMeTheRound (HunterView hv);

// Get the id of current player - ie whose turn is it?

enum player whoAmI (HunterView hv);

// Get the current score
// Returns a positive integer [0...366]

int giveMeTheScore (HunterView hv);

// Get the current health points for a given player
// 'player' specifies which players's life/blood points to return
//    and must be a value in the interval [0...4] (see 'player' type)

int howHealthyIs (HunterView hv, enum player player);

// Get the current location id of a given player
// May be UNKNOWN_LOCATION if the player has not had a turn yet
//   (ie at the beginning of the game when the round is 0)
// If the enum player is Dracula, and the currentPlayer doesn't
//   know Dracula's precise location, return the appropriate
//   X_UNKNOWN (where X is CITY or SEA) if that much can be determined,
//   else simply return UNKNOWN_LOCATION

enum location_id whereIs (HunterView hv, enum player player);


//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
//   for the given player, including Dracula (if he asks about his own trail)
// For any player if the move does not exist yet (i.e, the start of the game),
//   the value should be UNKNOWN_LOCATION (-1)
// For example after 2 turns the array may have the contents
//   {29, 12, -1, -1, -1, -1}
// This would mean in the first move the player started on location 12
//   then moved to the current location of 29
// Hunters only get approximate information about where Dracula has been
//   (e.g. SEA_UNKNOWN) unless Dracula is in Castle Dracula or has been encountered

void giveMeTheTrail (
	HunterView hv, enum player player, enum location_id trail[TRAIL_SIZE]);


//// Functions that query the map to find information about connectivity

// whereCanIgo() returns an array of enum location_ids giving
//   all locations that the current player could reach in his next move
// road, rail and/or sea connections should only be considered
//   if the road, rail and/or sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// The current location should be included in the array (could rest)

enum location_id *whereCanIgo (
	HunterView hv, int *numLocations, bool road, bool rail, bool sea);

// whereCanTheyGo() returns an array of enum location_ids giving all of the
//   locations that the given Player could reach from their current location
// road, rail and sea are connections should only be considered
//   if the road, rail, sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// If the given player is Dracula, sets numLocations to 0, unless you
//   know Dracula's location precisely
// The player's current location should be included in the array (could rest)

enum location_id *whereCanTheyGo (
	HunterView hv, int *numLocations,
	enum player player, bool road, bool rail, bool sea);


#endif // !defined (FOD__HUNTER_VIEW_H_)
