////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula_view.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sysexits.h>

#include "dracula_view.h"
#include "game.h"
#include "game_view.h"
// #include "map.h" ... if you decide to use the Map ADT

typedef struct dracula_view {
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} dracula_view;

// Creates a new DraculaView to summarise the current state of the game
dracula_view *newDraculaView (char *past_plays, player_message messages[])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	dracula_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate DraculaView");

	return new;
}

// Frees all memory previously allocated for the DraculaView toBeDeleted
void disposeDraculaView (dracula_view *toBeDeleted)
{
	// COMPLETE THIS IMPLEMENTATION
	free (toBeDeleted);
}

//// Functions to return simple information about the current state of the game

// Get the current round
round_t giveMeTheRound (dracula_view *dv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current score
int giveMeTheScore (dracula_view *dv)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current health points for a given player
int howHealthyIs (dracula_view *dv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the current location id of a given player
enum location_id whereIs (dracula_view *dv, enum player player)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// Get the most recent move of a given player
void lastMove (
	dracula_view *dv, enum player player,
	enum location_id *start, enum location_id *end)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return;
}

// Find out what minions are placed at the specified location
void whatsThere (
	dracula_view *dv, enum location_id where,
	int *numTraps, int *numVamps)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail (
	dracula_view *dv, enum player player,
	enum location_id trail[TRAIL_SIZE])
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
enum location_id *whereCanIgo (
	dracula_view *dv, int *numLocations, bool road, bool sea)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NULL;
}

// What are the specified player's next possible moves
enum location_id *whereCanTheyGo (
	dracula_view *dv, int *numLocations, enum player player,
	bool road, bool rail, bool sea)
{
	// REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numLocations = 0;
	return NULL;
}
