////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test basic empty initialisation");

		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert (gv_get_score (gv) == GAME_START_SCORE);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula trail and basic functions");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == PLAYER_LORD_GODALMING);
		assert (gv_get_round (gv) == 1);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (gv_get_location (gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (gv_get_location (gv, PLAYER_VAN_HELSING) == ZURICH);
		assert (gv_get_location (gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (gv_get_location (gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",  "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_location (gv, PLAYER_DRACULA) == GENEVA);
		assert (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5);
		assert (gv_get_health (gv, PLAYER_DRACULA) == 30);
		assert (gv_get_location (gv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[1] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == UNKNOWN_LOCATION);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Hunter View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == SEA_UNKNOWN);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula doubling back at sea, "
			  "and losing blood points (Dracula View)");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		assert (gv_get_player (gv) == 0);
		assert (gv_get_health (gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert (gv_get_location (gv, PLAYER_DRACULA) == DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		assert (history[0] == DOUBLE_BACK_1);
		assert (history[1] == ENGLISH_CHANNEL);

		puts ("passed");
		gv_drop (gv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for connections");
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		do {
			puts ("Checking Galatz road connections");
			int size, *edges = gv_get_connections (
				gv, &size,
				GALATZ, PLAYER_LORD_GODALMING, 0,
				true, false, false
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (int i = 0; i < size; i++)
				seen[edges[i]] = true;

			assert (size == 5);
			assert (seen[GALATZ]);
			assert (seen[CONSTANTA]);
			assert (seen[BUCHAREST]);
			assert (seen[KLAUSENBURG]);
			assert (seen[CASTLE_DRACULA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Ionian Sea sea connections");
			int size, *edges = gv_get_connections (
				gv, &size,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true
			);

			bool seen[NUM_MAP_LOCATIONS] = {false};
			for (int i = 0; i < size; i++)
				seen[edges[i]] = true;

			assert (size == 7);
			assert (seen[IONIAN_SEA]);
			assert (seen[BLACK_SEA]);
			assert (seen[ADRIATIC_SEA]);
			assert (seen[TYRRHENIAN_SEA]);
			assert (seen[ATHENS]);
			assert (seen[VALONA]);
			assert (seen[SALONICA]);

			free (edges);
		} while (0);

		do {
			puts ("Checking Athens rail connections (none)");
			int size, *edges = gv_get_connections (
				gv, &size,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false
			);

			assert (size == 1);
			assert (edges[0] == ATHENS);

			free (edges);
		} while (0);

		puts ("passed");
		gv_drop (gv);
	} while (0);

	return EXIT_SUCCESS;
}
