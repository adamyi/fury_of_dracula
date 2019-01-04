////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter_view.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "game.h"
#include "game_view.h"
#include "hunter_view.h"
// #include "map.h" ... if you decide to use the Map ADT

typedef struct hunter_view {
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} hunter_view;

// Creates a new HunterView to summarise the current state of the game
hunter_view *newHunterView (char *pastPlays, player_message messages[])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	hunter_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate HunterView");

	return new;
}

// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView (hunter_view *hv)
{
	// COMPLETE THIS IMPLEMENTATION
	free (hv);
}

//// Functions to return simple information about the current state of the game

// Get the current round
round_t giveMeTheRound (hunter_view *hv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the id of current player
enum player whoAmI (hunter_view *hv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current score
int giveMeTheScore (hunter_view *hv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current health points for a given player
int howHealthyIs (hunter_view *hv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current location id of a given player
enum location_id whereIs (hunter_view *hv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail (
	hunter_view *hv, enum player player,
	enum location_id trail[TRAIL_SIZE])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
enum location_id *whereCanIgo (
	hunter_view *hv, int *numLocations,
	bool road, bool rail, bool sea)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numLocations = 0;
	return NULL;
}

// What are the specified player's next possible moves
enum location_id *whereCanTheyGo (
	hunter_view *hv, int *numLocations, enum player player,
	bool road, bool rail, bool sea)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numLocations = 0;
	return NULL;
}
