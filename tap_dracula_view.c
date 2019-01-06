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

#define NDEBUG // use TAP please
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dracula_view.h"

#define NUM_ALL_LOCATIONS ((int)TELEPORT + 1)
#define eprintf(msg, ...) \
        fprintf (stderr, msg, ##__VA_ARGS__)

#define IS_AT(who,where) \
	TEST (dv_get_location (dv, (who)) == (where), \
		"dv_get_location(dv, %s) == %s %s", \
		player_to_s (who), \
		location_get_abbrev (where), \
		location_get_name (where))
#define HISTORY_IS(who,n,where) \
	TEST (history[(n)] == (where), \
		"dv_get_trail(dv, %s): [%d] %s %s", \
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
	puts ("1..135");

	do {////////////////////////////////////////////////////////////////
		char *symbols[] = {
			"dv_new", "dv_drop", "dv_get_round", "dv_get_score",
			"dv_get_health", "dv_get_location", "dv_get_player_move",
			"dv_get_locale_info", "dv_get_trail", "dv_get_dests",
			"dv_get_dests_player"
		};
		for (size_t i = 0; i < nitems(symbols); i++)
			TEST (have_symbol (symbols[i]), "have '%s'", symbols[i]);
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Test for basic functions, just before Dracula's first move

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", ""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		TEST (dv_get_round (dv) == 0,
			"dv_get_round(dv) == 0");
		TEST (dv_get_health (dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS,
			"dv_get_health(dv, Dracula) == 40 (start)");
		IS_AT (PLAYER_LORD_GODALMING, STRASBOURG);
		IS_AT (PLAYER_DR_SEWARD, ATLANTIC_OCEAN);
		IS_AT (PLAYER_VAN_HELSING, ZURICH);
		IS_AT (PLAYER_MINA_HARKER, BAY_OF_BISCAY);
		IS_AT (PLAYER_DRACULA, UNKNOWN_LOCATION);

		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			  "dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Test for encountering Dracula and hunter history

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.... "
			"GGED... SAO.... HCD.... MAO....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "" };
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		TEST (dv_get_round (dv) == 1,
			"dv_get_round(dv) == 1");
		IS_AT (PLAYER_DRACULA, GENEVA);
		IS_AT (PLAYER_LORD_GODALMING, GENEVA);
		TEST (dv_get_health (dv, PLAYER_LORD_GODALMING) == 5,
			"dv_get_health(dv, Godalming) == 5");
		TEST (dv_get_health (dv, PLAYER_DRACULA) == 30,
			"dv_get_health(dv, Dracula) == 30");

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, GENEVA);
		HISTORY_IS (PLAYER_DRACULA, 1, UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_LORD_GODALMING, history);
		HISTORY_IS (PLAYER_LORD_GODALMING, 0, GENEVA);
		HISTORY_IS (PLAYER_LORD_GODALMING, 1, STRASBOURG);
		HISTORY_IS (PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_DR_SEWARD, history);
		HISTORY_IS (PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN);
		HISTORY_IS (PLAYER_DR_SEWARD, 1, ATLANTIC_OCEAN);
		HISTORY_IS (PLAYER_DR_SEWARD, 2, UNKNOWN_LOCATION);

		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Test for Dracula leaving minions

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST....";
		player_message messages[] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Drop a V",
			"Party in Strasbourg", "Party", "Party", "Party"};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		int nT, nV;
		dv_get_locale_info (dv, EDINBURGH, &nT, &nV);
		TEST (nT == 0, "dv_get_locale_info(dv, ED Edinburgh): 0 trap(s)");
		TEST (nV == 1, "dv_get_locale_info(dv, ED Edinburgh): 1 vampire(s)");
		dv_get_locale_info (dv, MANCHESTER, &nT, &nV);
		TEST (nT == 1, "dv_get_locale_info(dv, MN Manchester): 1 trap(s)");
		TEST (nV == 0, "dv_get_locale_info(dv, MN Manchester): 0 vampire(s)");
		IS_AT (PLAYER_DRACULA, MANCHESTER);

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, MANCHESTER);
		HISTORY_IS (PLAYER_DRACULA, 1, EDINBURGH);
		HISTORY_IS (PLAYER_DRACULA, 2, UNKNOWN_LOCATION);

		dv_get_trail (dv, PLAYER_MINA_HARKER, history);
		HISTORY_IS (PLAYER_MINA_HARKER, 0, STRASBOURG);
		HISTORY_IS (PLAYER_MINA_HARKER, 1, STRASBOURG);
		HISTORY_IS (PLAYER_MINA_HARKER, 2, GENEVA);
		HISTORY_IS (PLAYER_MINA_HARKER, 3, UNKNOWN_LOCATION);

		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Checking Galatz road connections

		char *trail = "GGA....";
		player_message messages[] = {"Gone to Galatz"};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, true, false, false);
		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 5,
			"dv_get_dests_player(dv, Godalming, road): 5 locations");
		EXPECT_TO_SEE (GALATZ);
		EXPECT_TO_SEE (CONSTANTA);
		EXPECT_TO_SEE (BUCHAREST);
		EXPECT_TO_SEE (KLAUSENBURG);
		EXPECT_TO_SEE (CASTLE_DRACULA);

		free (edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Checking Ionian Sea sea connections

		char *trail = "GIO....";
		player_message messages[] = {"Sailing the Ionian"};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, false, false, true);
		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 7,
			"dv_get_dests_player(dv, Godalming, sea): 7 locations");
		EXPECT_TO_SEE (IONIAN_SEA);
		EXPECT_TO_SEE (BLACK_SEA);
		EXPECT_TO_SEE (ADRIATIC_SEA);
		EXPECT_TO_SEE (TYRRHENIAN_SEA);
		EXPECT_TO_SEE (ATHENS);
		EXPECT_TO_SEE (VALONA);
		EXPECT_TO_SEE (SALONICA);

		free (edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Checking Athens rail connections (none)

		char *trail = "GAT....";
		player_message messages[] = {"Leaving Athens by train"};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		size_t n_edges;
		location_t *edges = dv_get_dests_player (
			dv, &n_edges, PLAYER_LORD_GODALMING, false, true, false);

		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 1,
			"dv_get_dests_player(dv, Godalming, rail): 1 location");
		EXPECT_TO_SEE (ATHENS);

		free (edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);


	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital
		// and for dracula hiding

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU....";

		player_message messages[24] = {"Going somewhere",""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		IS_AT (PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS);

		location_t from, to;
		dv_get_player_move (dv, PLAYER_LORD_GODALMING, &from, &to);
		TEST (from == SARAGOSSA && to == MADRID,
			"dv_get_player_move(Godalming): Saragossa -> Madrid");

		IS_AT (PLAYER_DRACULA, MADRID);

		dv_get_player_move (dv, PLAYER_DRACULA, &from, &to);
		TEST (from == MADRID && to == HIDE,
			"dv_get_player_move(dracula): Madrid -> Hide");

		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where is and lastmove for players sent to the hospital
		// and for dracula hiding and doubling back

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU.... DD1T... "
			"GZA.... SCF.... HGA....";

		player_message messages[28] = {"Going somewhere",""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		IS_AT (PLAYER_LORD_GODALMING, ZAGREB);

		location_t from, to;
		dv_get_player_move (dv, PLAYER_LORD_GODALMING, &from, &to);
		TEST (from == MADRID && to == ZAGREB,
			"dv_get_player_move(Godalming): Madrid (via Hospital) -> Zagreb");

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_LORD_GODALMING, history);
		HISTORY_IS (PLAYER_LORD_GODALMING, 0, ZAGREB);
		HISTORY_IS (PLAYER_LORD_GODALMING, 1, MADRID);
		HISTORY_IS (PLAYER_LORD_GODALMING, 2, SARAGOSSA);
		HISTORY_IS (PLAYER_LORD_GODALMING, 3, SANTANDER);
		HISTORY_IS (PLAYER_LORD_GODALMING, 4, MADRID);
		HISTORY_IS (PLAYER_LORD_GODALMING, 5, LISBON);

		IS_AT (PLAYER_DRACULA, MADRID);

		dv_get_player_move (dv, PLAYER_DRACULA, &from, &to);
		TEST (from == HIDE && to == DOUBLE_BACK_1,
			"dv_get_player_move(dracula): Hide -> Double Back 1");

		dv_get_trail (dv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, MADRID);
		HISTORY_IS (PLAYER_DRACULA, 1, MADRID);
		HISTORY_IS (PLAYER_DRACULA, 2, MADRID);
		HISTORY_IS (PLAYER_DRACULA, 3, SARAGOSSA);
		HISTORY_IS (PLAYER_DRACULA, 4, SANTANDER);
		HISTORY_IS (PLAYER_DRACULA, 5, UNKNOWN_LOCATION);

		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU....";

		player_message messages[24] = {"Going somewhere",""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		size_t n_edges;
		location_t *edges = dv_get_dests (dv, &n_edges, true, true);
		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 5,
			"dv_get_dests(dv, Dracula, road,sea): 5 locations");
		EXPECT_TO_SEE (ALICANTE);
		EXPECT_TO_SEE (CADIZ);
		EXPECT_TO_SEE (GRANADA);
		EXPECT_TO_SEE (LISBON);
		EXPECT_TO_SEE (MADRID);

		free(edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go

		char *trail =
			"GLS.... SMR.... HCD.... MAM.... DSN.V.. "
			"GMA.... SCF.... HGA.... MCO.... DSRT... "
			"GSNV... SMR.... HCD.... MAM.... DMAT... "
			"GSRT... SCF.... HGA.... MBU.... DHIT... "
			"GMATTD. SCF.... HGA.... MBU.... DD1T... "
			"GZA.... SCF.... HGA....";

		player_message messages[28] = {"Going somewhere",""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		size_t n_edges;
		location_t *edges = dv_get_dests (dv, &n_edges, true, true);
		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 5,
			"dv_get_dests(dv, Dracula, road,sea): 5 locations");
		EXPECT_TO_SEE (ALICANTE);
		EXPECT_TO_SEE (CADIZ);
		EXPECT_TO_SEE (GRANADA);
		EXPECT_TO_SEE (LISBON);
		EXPECT_TO_SEE (MADRID);

		free(edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
	} while (0);

	do {////////////////////////////////////////////////////////////////
		// Checking where dracula can go with TP

		char *trail =  "GMA.... SMN.... HAM.... MGA.... DGA.V.. "
					   "GAL.... SED.... HBU.... MGAVD.. DCDT... "
					   "GMS.... SNS.... HCO.... MCDTD.. DKLT... "
					   "GMR.... SHA.... HFR.... MCD.... DD2T... "
					   "GGO.... SBR.... HBR.... MCDTD.. DHIT... "
					   "GVE.... SPR.... HBD.... MJM.... DTPT... "
					   "GBD.... SBD.... HKLT... MBE....";

		player_message messages[34] = {"Going somewhere",""};
		DraculaView dv = dv_new (trail, messages);

		TEST (dv != NULL, "dv := dv_new(\"%s\")", trail);
		TEST (! mem_address_is_poisoned (dv), "dv is a valid allocation");

		IS_AT (PLAYER_DRACULA, CASTLE_DRACULA);

		int from, to;
		dv_get_player_move (dv, PLAYER_DRACULA, &from, &to);
		TEST (from == HIDE && to == TELEPORT,
			"dv_get_player_move(dracula): Hide -> Teleport");

		location_t history[TRAIL_SIZE];
		dv_get_trail (dv, PLAYER_DRACULA, history);
		HISTORY_IS (PLAYER_DRACULA, 0, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_DRACULA, 1, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_DRACULA, 2, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_DRACULA, 3, KLAUSENBURG);
		HISTORY_IS (PLAYER_DRACULA, 4, CASTLE_DRACULA);
		HISTORY_IS (PLAYER_DRACULA, 5, GALATZ);

		size_t n_edges;
		location_t *edges = dv_get_dests (dv, &n_edges, true, true);
		bool seen[NUM_ALL_LOCATIONS] = {false};
		for (size_t i = 0; i < n_edges; i++) {
			if (! valid_location_p (edges[i]))
				eprintf ("... invalid location '%d'", edges[i]);
			seen[edges[i]] = true;
		}

		TEST (n_edges == 2,
			"dv_get_dests(dv, Dracula, road,sea): 2 locations");
		EXPECT_TO_SEE (GALATZ);
		EXPECT_TO_SEE (CASTLE_DRACULA);

		free(edges);
		dv_drop (dv);
		TEST (mem_address_is_poisoned (dv),
			"dv is no longer a valid allocation");
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
