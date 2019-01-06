////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#define NDEBUG // use TAP please
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_view.h"

#define NUM_ALL_LOCATIONS ((int)TELEPORT + 1)
#define eprintf(msg, ...) \
        fprintf (stderr, msg, ##__VA_ARGS__)

#define IS_AT(who,where) \
	TEST (gv_get_location (gv, (who)) == (where), \
		"gv_get_location(gv, %s) == %s %s", \
		player_to_s (who), \
		location_get_abbrev (where), \
		location_get_name (where))
#define HISTORY_IS(who,n,where) \
	TEST (history[(n)] == (where), \
		"gv_get_history(gv, %s): [%d] %s %s", \
		player_to_s (who), n, \
		location_get_abbrev (where), \
		location_get_name (where))
#define EXPECT_TO_SEE(where) \
	TEST (seen[where], "... %s %s", \
		location_get_abbrev (where), \
		location_get_name (where))

static const char *player_to_s (enum player);

int main (void)
{
	puts ("1..116");

	do {////////////////////////////////////////////////////////////////
		char *symbols[] = {
			"gv_new", "gv_drop", "gv_get_round", "gv_get_player",
			"gv_get_score", "gv_get_health", "gv_get_location",
			"gv_get_history", "gv_get_connections"
		};

		for (size_t i = 0; i < nitems(symbols); i++)
			TEST (have_symbol (symbols[i]), "have '%s'", symbols[i]);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		TEST (gv_get_player (gv) == PLAYER_LORD_GODALMING,
			"gv_get_player(gv) == Godalming");
		TEST (gv_get_round (gv) == 0,
			"gv_get_round(gv) == 0");
		TEST (gv_get_health (gv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS,
			"gv_get_health(gv, Seward) == 9 (start)");
		TEST (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS,
			"gv_get_health(gv, Dracula) == 40 (start)");
		TEST (gv_get_score (gv) == GAME_START_SCORE,
			"gv_get_score(gv) == 366");
		IS_AT (PLAYER_LORD_GODALMING, UNKNOWN_LOCATION);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		TEST (gv_get_player (gv) == PLAYER_LORD_GODALMING,
			"gv_get_player(gv) == Godalming");
		TEST (gv_get_round (gv) == 1,
			"gv_get_round(gv) == 1");

		IS_AT (PLAYER_LORD_GODALMING, STRASBOURG);
		IS_AT (PLAYER_DR_SEWARD, ATLANTIC_OCEAN);
		IS_AT (PLAYER_VAN_HELSING, ZURICH);
		IS_AT (PLAYER_MINA_HARKER, BAY_OF_BISCAY);
		IS_AT (PLAYER_DRACULA, CITY_UNKNOWN);

		TEST (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS,
			"gv_get_health(gv, Dracula) == 40 (start)");

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED...";
		player_message messages[] = {
			"Hello", "Rubbish",	 "Stuff", "", "Mwahahah",
			"Aha!"};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		IS_AT (PLAYER_DRACULA, GENEVA);
		TEST (gv_get_health (gv, PLAYER_LORD_GODALMING) == 5,
			"gv_get_health(gv, Godalming) == 5");
		TEST (gv_get_health (gv, PLAYER_DRACULA) == 30,
			"gv_get_health(gv, Dracula) == 30");
		IS_AT (PLAYER_LORD_GODALMING, GENEVA);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		HISTORY_IS (PLAYER_LORD_GODALMING, 0, GENEVA);
		HISTORY_IS (PLAYER_LORD_GODALMING, 1, STRASBOURG);
		HISTORY_IS (PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DR_SEWARD, history);
		HISTORY_IS (PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN);
		HISTORY_IS (PLAYER_DR_SEWARD, 1, UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_VAN_HELSING, history);
		HISTORY_IS (PLAYER_VAN_HELSING, 0, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_VAN_HELSING, 1, UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_MINA_HARKER, history);
		HISTORY_IS (PLAYER_MINA_HARKER, 0, ATLANTIC_OCEAN);
		HISTORY_IS (PLAYER_MINA_HARKER, 1, UNKNOWN_LOCATION);

		gv_get_history (gv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, GENEVA);
		HISTORY_IS (PLAYER_DRACULA, 1, UNKNOWN_LOCATION);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		TEST (gv_get_player (gv) == PLAYER_LORD_GODALMING,
			"gv_get_player(gv) == Godalming");
		TEST (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4,
			"gv_get_health(gv, Dracula) == 36");
		IS_AT (PLAYER_DRACULA, DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, DOUBLE_BACK_1);
		HISTORY_IS (PLAYER_DRACULA, 1, SEA_UNKNOWN);
		HISTORY_IS (PLAYER_DRACULA, 2, UNKNOWN_LOCATION);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		TEST (gv_get_player (gv) == PLAYER_LORD_GODALMING,
			"gv_get_player(gv) == Godalming");
		TEST (gv_get_health (gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4,
			"gv_get_health (gv, Dracula) == 36");
		IS_AT (PLAYER_DRACULA, DOUBLE_BACK_1);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, DOUBLE_BACK_1);
		HISTORY_IS (PLAYER_DRACULA, 1, ENGLISH_CHANNEL);
		HISTORY_IS (PLAYER_DRACULA, 2, UNKNOWN_LOCATION);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		char *trail = "";
		player_message messages[] = {};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		do {
			size_t n_edges;
			location_t *edges = gv_get_connections (gv, &n_edges,
				GALATZ, PLAYER_LORD_GODALMING, 0, true, false, false);

			bool seen[NUM_ALL_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				if (! valid_location_p (edges[i]))
					eprintf ("... invalid location '%d'", edges[i]);
				seen[edges[i]] = true;
			}

			TEST (n_edges == 5,
				"connections(gv, GA Galatz, Godalming@0, road): 5 locations");
			EXPECT_TO_SEE (GALATZ);
			EXPECT_TO_SEE (CONSTANTA);
			EXPECT_TO_SEE (BUCHAREST);
			EXPECT_TO_SEE (KLAUSENBURG);
			EXPECT_TO_SEE (CASTLE_DRACULA);

			free (edges);
		} while (0);

		do {
			size_t n_edges;
			location_t *edges = gv_get_connections (gv, &n_edges,
				IONIAN_SEA, PLAYER_LORD_GODALMING, 0,
				false, false, true);

			bool seen[NUM_ALL_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				if (! valid_location_p (edges[i]))
					eprintf ("... invalid location '%d'", edges[i]);
				seen[edges[i]] = true;
			}

			TEST (n_edges == 7,
				"connections(gv, IO Ionian, Godalming@0, sea): 7 locations");
			EXPECT_TO_SEE (IONIAN_SEA);
			EXPECT_TO_SEE (BLACK_SEA);
			EXPECT_TO_SEE (ADRIATIC_SEA);
			EXPECT_TO_SEE (TYRRHENIAN_SEA);
			EXPECT_TO_SEE (ATHENS);
			EXPECT_TO_SEE (VALONA);
			EXPECT_TO_SEE (SALONICA);

			free (edges);
		} while (0);

		do {
			size_t n_edges;
			location_t *edges = gv_get_connections (gv, &n_edges,
				ATHENS, PLAYER_LORD_GODALMING, 0,
				false, true, false);

			bool seen[NUM_ALL_LOCATIONS] = {false};
			for (size_t i = 0; i < n_edges; i++) {
				if (! valid_location_p (edges[i]))
					eprintf ("... invalid location '%d'", edges[i]);
				seen[edges[i]] = true;
			}

			TEST (n_edges == 1,
				"connections(gv, AT Athens, Godalming@0, rail): 1 location");
			EXPECT_TO_SEE (ATHENS);

			free (edges);
		} while (0);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			  "gv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU....";

		player_message messages[24] = {"Going somewhere",""};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		IS_AT (PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS);
		IS_AT (PLAYER_DRACULA, HIDE);
		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			"gv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU.... DD1T... "
			"GZA.... SCF.... HGA....";

		player_message messages[28] = {"Going somewhere",""};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		IS_AT (PLAYER_LORD_GODALMING, ZAGREB);

		location_t history[TRAIL_SIZE];
		gv_get_history (gv, PLAYER_LORD_GODALMING, history);
		HISTORY_IS (PLAYER_LORD_GODALMING, 0, ZAGREB);
		HISTORY_IS (PLAYER_LORD_GODALMING, 1, MADRID);
		HISTORY_IS (PLAYER_LORD_GODALMING, 2, SARAGOSSA);
		HISTORY_IS (PLAYER_LORD_GODALMING, 3, SANTANDER);
		HISTORY_IS (PLAYER_LORD_GODALMING, 4, MADRID);
		HISTORY_IS (PLAYER_LORD_GODALMING, 5, LISBON);

		IS_AT (PLAYER_DRACULA, DOUBLE_BACK_1);
		gv_get_history (gv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, DOUBLE_BACK_1);
		HISTORY_IS (PLAYER_DRACULA, 1, HIDE);
		HISTORY_IS (PLAYER_DRACULA, 2, MADRID);
		HISTORY_IS (PLAYER_DRACULA, 3, SARAGOSSA);
		HISTORY_IS (PLAYER_DRACULA, 4, SANTANDER);
		HISTORY_IS (PLAYER_DRACULA, 5, UNKNOWN_LOCATION);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			"hv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking location and history for dracula TP moves

		char *trail =
			"GMA.... SMN.... HAM.... MGA.... DGA.V.. "
			"GAL.... SED.... HBU.... MGAVD.. DCDT... "
			"GMS.... SNS.... HCO.... MCDTD.. DKLT... "
			"GMR.... SHA.... HFR.... MCD.... DD2T... "
			"GGO.... SBR.... HBR.... MCDTD.. DHIT... "
			"GVE.... SPR.... HBD.... MJM.... DTPT... "
			"GBD.... SBD.... HKLT... MBE....";

		player_message messages[34] = {"Going somewhere",""};
		GameView gv = gv_new (trail, messages);

		TEST (gv != NULL, "gv := gv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (gv), "gv is a valid allocation");

		location_t history[TRAIL_SIZE];

		IS_AT (PLAYER_DRACULA, TELEPORT);
		gv_get_history (gv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, TELEPORT);
		HISTORY_IS (PLAYER_DRACULA, 1, HIDE);
		HISTORY_IS (PLAYER_DRACULA, 2, DOUBLE_BACK_2);
		HISTORY_IS (PLAYER_DRACULA, 3, KLAUSENBURG);
		HISTORY_IS (PLAYER_DRACULA, 4, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_DRACULA, 5, GALATZ);

		gv_drop (gv);
		TEST (mem_address_is_poisoned (gv),
			"hv is no longer a valid allocation");
	} while (0);


	return EXIT_SUCCESS;
}


static const char *player_to_s (enum player p)
{
	switch (p) {
	case PLAYER_LORD_GODALMING: return "Godalming";
	case PLAYER_DR_SEWARD: return "Seward";
	case PLAYER_VAN_HELSING: return "vHelsing";
	case PLAYER_MINA_HARKER: return "Harker";
	case PLAYER_DRACULA: return "Dracula";
	}
}
