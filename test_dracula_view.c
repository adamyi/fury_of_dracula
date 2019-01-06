////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_dracula_view.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dracula_view.h"

int main (void)
{
	do {////////////////////////////////////////////////////////////////
		puts ("Test for basic functions, "
			  "just before Dracula's first move");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 0);
		assert (dv_get_location (dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert (dv_get_location (dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert (dv_get_location (dv, PLAYER_VAN_HELSING) == ZURICH);
		assert (dv_get_location (dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert (dv_get_location (dv, PLAYER_DRACULA) == UNKNOWN_LOCATION);
		assert (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for encountering Dracula and hunter history");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED... SAO.... HCD.... MAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "" };
		DraculaView dv = dv_new (trail, messages);

		assert (dv_get_round (dv) == 1);
		assert (dv_get_location (dv, PLAYER_DRACULA) == GENEVA);
		assert (dv_get_health (dv, PLAYER_LORD_GODALMING) == 5);
		assert (dv_get_health (dv, PLAYER_DRACULA) == 30);
		assert (dv_get_location (dv, PLAYER_LORD_GODALMING) == GENEVA);

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		assert (history[0] == GENEVA);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_LORD_GODALMING, history);
		assert (history[0] == GENEVA);
		assert (history[1] == STRASBOURG);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_DR_SEWARD, history);
		assert (history[0] == ATLANTIC_OCEAN);
		assert (history[1] == ATLANTIC_OCEAN);
		assert (history[2] == UNKNOWN_LOCATION);

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Test for Dracula leaving minions");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"};
		DraculaView dv = dv_new (trail, messages);

		int nT, nV;
		dv_get_locale_info (dv, EDINBURGH, &nT, &nV);
		assert (nT == 0 && nV == 1);
		dv_get_locale_info (dv, MANCHESTER, &nT, &nV);
		assert (nT == 1 && nV == 0);
		assert (dv_get_location (dv, PLAYER_DRACULA) == MANCHESTER);

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		assert (history[0] == MANCHESTER);
		assert (history[1] == EDINBURGH);
		assert (history[2] == UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_MINA_HARKER, history);
		assert (history[0] == STRASBOURG);
		assert (history[1] == STRASBOURG);
		assert (history[2] == GENEVA);
		assert (history[3] == UNKNOWN_LOCATION);

		puts ("passed");
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Galatz road connections");
		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		DraculaView dv = dv_new (trail, messages);

		int size, *edges = dv_get_dests_player (
			dv, &size, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_MAP_LOCATIONS] = {false};
		for (int i = 0; i < size; i++)
			seen[edges[i]] = true;

		assert (size == 5);
		assert (seen[GALATZ]);
		assert (seen[CONSTANTA]);
		assert (seen[BUCHAREST]);
		assert (seen[KLAUSENBURG]);
		assert (seen[CASTLE_DRACULA]);

		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Ionian Sea sea connections");
		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		DraculaView dv = dv_new (trail, messages);

		int size, *edges = dv_get_dests_player (
			dv, &size, PLAYER_LORD_GODALMING, false, false, true);
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

		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		puts ("Checking Athens rail connections (none)");

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		DraculaView dv = dv_new (trail, messages);

		int size, *edges = dv_get_dests_player (
			dv, &size, PLAYER_LORD_GODALMING, false, true, false);
		assert (size == 1);
		assert (edges[0] == ATHENS);


		puts ("passed");
		free (edges);
		dv_drop (dv);
	} while (0);

	return EXIT_SUCCESS;
}
