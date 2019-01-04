////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
// #include "map.h" ... if you decide to use the Map ADT

typedef struct game_view {
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} game_view;

// Creates a new GameView to summarise the current state of the game
game_view *newGameView (char *pastPlays, player_message messages[])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	game_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate GameView");

	return new;
}

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView (game_view *toBeDeleted)
{
// COMPLETE THIS IMPLEMENTATION
	free (toBeDeleted);
}

//// Functions to return simple information about the current state of the game

// Get the current round
round_t getRound (game_view *gv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the id of current player - ie whose turn is it?
enum player getCurrentPlayer (game_view *gv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current score
int getScore (game_view *gv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current health points for a given player
int getHealth (game_view *gv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current location id of a given player
enum location_id getLocation (game_view *gv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory (
	game_view *gv, enum player player, enum location_id trail[TRAIL_SIZE])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// Returns an array of enum location_ids for all directly connected locations

enum location_id *connectedLocations (
	game_view *gv, int *numLocations,
	enum location_id from, enum player player, round_t round,
	bool road, bool rail, bool sea)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numLocations = 0;
	return NULL;
}
