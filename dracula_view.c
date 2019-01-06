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
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
} dracula_view;

dracula_view *dv_new (char *past_plays, player_message messages[])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	dracula_view *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate DraculaView");

	return new;
}

void dv_drop (dracula_view *dv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free (dv);
}

round_t dv_get_round (dracula_view *dv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int dv_get_score (dracula_view *dv)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int dv_get_health (dracula_view *dv, enum player player)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

location_t dv_get_location (dracula_view *dv, enum player player)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

void dv_get_player_move (
	dracula_view *dv, enum player player,
	location_t *start, location_t *end)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return;
}

void dv_get_locale_info (
	dracula_view *dv, location_t where,
	int *n_traps, int *n_vamps)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return;
}

void dv_get_trail (
	dracula_view *dv, enum player player,
	location_t trail[TRAIL_SIZE])
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

location_t *dv_get_dests (
	dracula_view *dv, size_t *n_locations, bool road, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}

location_t *dv_get_dests_player (
	dracula_view *dv, size_t *n_locations, enum player player,
	bool road, bool rail, bool sea)
{
	/// @todo REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*n_locations = 0;
	return NULL;
}
