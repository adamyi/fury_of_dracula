////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// testHunterView.c: test the HunterView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#define NDEBUG  // use TAP please
#include "test1511.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hunter_view.h"

#define NUM_ALL_LOCATIONS ((int)TELEPORT + 1)
#define eprintf(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)

#define IS_AT(who, where)                                    \
  TEST(hv_get_location(hv, (who)) == (where),                \
       "hv_get_location(hv, %s) == %s %s", player_to_s(who), \
       location_get_abbrev(where), location_get_name(where))
#define HISTORY_IS(who, n, where)                                   \
  TEST(history[(n)] == (where), "hv_get_trail(hv, %s): [%d] %s %s", \
       player_to_s(who), n, location_get_abbrev(where),             \
       location_get_name(where))
#define EXPECT_TO_SEE(where)                                 \
  TEST(seen[where], "... %s %s", location_get_abbrev(where), \
       location_get_name(where))

static const char *player_to_s(enum player);

int main(void) {
  puts("1..137");

  do {  ////////////////////////////////////////////////////////////////
    char *symbols[] = {"hv_new",          "hv_drop",
                       "hv_get_round",    "hv_get_player",
                       "hv_get_score",    "hv_get_health",
                       "hv_get_location", "hv_get_trail",
                       "hv_get_dests",    "hv_get_dests_player"};
    for (size_t i = 0; i < nitems(symbols); i++)
      TEST(have_symbol(symbols[i]), "have '%s'", symbols[i]);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    char *trail = "";
    player_message messages[] = {};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    TEST(hv_get_player(hv) == PLAYER_LORD_GODALMING,
         "hv_get_player(hv) == Godalming");
    TEST(hv_get_round(hv) == 0, "hv_get_round(hv) == 0");
    TEST(hv_get_health(hv, PLAYER_DR_SEWARD) == GAME_START_HUNTER_LIFE_POINTS,
         "hv_get_health(hv, Seward) == 9 (start)");
    TEST(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS,
         "hv_get_health(hv, Dracula) == 40 (start)");
    TEST(hv_get_score(hv) == GAME_START_SCORE, "hv_get_score(hv) == 366");
    IS_AT(PLAYER_LORD_GODALMING, UNKNOWN_LOCATION);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Test for Dracula trail and basic functions

    char *trail = "GST.... SAO.... HZU.... MBB.... DC?....";
    player_message messages[] = {"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    TEST(hv_get_player(hv) == PLAYER_LORD_GODALMING,
         "hv_get_player(hv) == Godalming");
    TEST(hv_get_round(hv) == 1, "hv_get_round(hv) == 1");
    IS_AT(PLAYER_LORD_GODALMING, STRASBOURG);
    IS_AT(PLAYER_DR_SEWARD, ATLANTIC_OCEAN);
    IS_AT(PLAYER_VAN_HELSING, ZURICH);
    IS_AT(PLAYER_MINA_HARKER, BAY_OF_BISCAY);
    IS_AT(PLAYER_DRACULA, CITY_UNKNOWN);
    TEST(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS,
         "hv_get_health(hv, Dracula) == 40 (start)");

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Test for encountering Dracula and hunter history
    char *trail =
        "GST.... SAO.... HCD.... MAO.... DGE.... "
        "GGED...";
    player_message messages[] = {"Hello", "Rubbish",  "Stuff",
                                 "",      "Mwahahah", "Aha!"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_DRACULA, GENEVA);
    IS_AT(PLAYER_LORD_GODALMING, GENEVA);
    TEST(hv_get_health(hv, PLAYER_LORD_GODALMING) == 5,
         "hv_get_health(hv, Godalming) == 5");
    TEST(hv_get_health(hv, PLAYER_DRACULA) == 30,
         "hv_get_health(hv, Dracula) == 30");

    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, GENEVA);
    HISTORY_IS(PLAYER_DRACULA, 1, UNKNOWN_LOCATION);

    hv_get_trail(hv, PLAYER_LORD_GODALMING, history);
    HISTORY_IS(PLAYER_LORD_GODALMING, 0, GENEVA);
    HISTORY_IS(PLAYER_LORD_GODALMING, 1, STRASBOURG);
    HISTORY_IS(PLAYER_LORD_GODALMING, 2, UNKNOWN_LOCATION);

    hv_get_trail(hv, PLAYER_DR_SEWARD, history);
    HISTORY_IS(PLAYER_DR_SEWARD, 0, ATLANTIC_OCEAN);
    HISTORY_IS(PLAYER_DR_SEWARD, 1, UNKNOWN_LOCATION);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Test for Dracula doubling back at sea, and losing blood
    // points (Hunter View)

    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DS?.... "
        "GST.... SST.... HST.... MST.... DD1....";
    player_message messages[] = {"Hello", "Rubbish", "Stuff", "", "Mwahahah",
                                 "Aha!",  "",        "",      "", "Back I go"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    TEST(hv_get_player(hv) == PLAYER_LORD_GODALMING,
         "hv_get_player(hv) == Godalming");
    TEST(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4,
         "hv_get_health(hv, Dracula) == 36");
    IS_AT(PLAYER_DRACULA, DOUBLE_BACK_1);

    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, DOUBLE_BACK_1);
    HISTORY_IS(PLAYER_DRACULA, 1, SEA_UNKNOWN);
    HISTORY_IS(PLAYER_DRACULA, 2, UNKNOWN_LOCATION);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking Galatz road connections
    char *trail = "GGA....";
    player_message messages[] = {"Gone to Galatz"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    int size, *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                           true, false, false);
    bool seen[NUM_ALL_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) {
      if (!valid_location_p(edges[i]))
        eprintf("... invalid location '%d'", edges[i]);
      seen[edges[i]] = true;
    }

    TEST(size == 5, "hv_get_dests_player(hv, Godalming, road): 5 locations");
    EXPECT_TO_SEE(GALATZ);
    EXPECT_TO_SEE(CONSTANTA);
    EXPECT_TO_SEE(BUCHAREST);
    EXPECT_TO_SEE(KLAUSENBURG);
    EXPECT_TO_SEE(CASTLE_DRACULA);

    free(edges);
    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking Ionian Sea sea connections

    char *trail = "GIO....";
    player_message messages[] = {"Sailing the Ionian"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    int size, *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                           false, false, true);
    bool seen[NUM_ALL_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) {
      if (!valid_location_p(edges[i]))
        eprintf("... invalid location '%d'", edges[i]);
      seen[edges[i]] = true;
    }

    TEST(size == 7, "hv_get_dests_player(hv, Godalming, sea): 7 locations");
    EXPECT_TO_SEE(IONIAN_SEA);
    EXPECT_TO_SEE(BLACK_SEA);
    EXPECT_TO_SEE(ADRIATIC_SEA);
    EXPECT_TO_SEE(TYRRHENIAN_SEA);
    EXPECT_TO_SEE(ATHENS);
    EXPECT_TO_SEE(VALONA);
    EXPECT_TO_SEE(SALONICA);

    free(edges);
    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking Athens rail connections (none)

    char *trail = "GAT....";
    player_message messages[] = {"Leaving Athens by train"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    int size, *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                           false, true, false);

    bool seen[NUM_ALL_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) {
      if (!valid_location_p(edges[i]))
        eprintf("... invalid location '%d'", edges[i]);
      seen[edges[i]] = true;
    }

    TEST(size == 1, "hv_get_dests_player(hv, Godalming, rail): 1 location");
    EXPECT_TO_SEE(ATHENS);

    free(edges);
    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking where is and trail for players sent to the hospital

    char *trail =
        "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
        "GMA.... SCF.... HGA.... MCO.... DSRT... "
        "GSNV... SMR.... HCD.... MAM.... DMAT... "
        "GSRT... SCF.... HGA.... MBU.... DHIT... "
        "GMATTD. SCF.... HGA.... MBU....";

    player_message messages[24] = {"Going somewhere"};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_LORD_GODALMING, ST_JOSEPH_AND_ST_MARYS);
    TEST(hv_get_health(hv, PLAYER_LORD_GODALMING) == 0, "Hunter is killed");

    IS_AT(PLAYER_DRACULA, MADRID);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking where is and trail for players sent to the hospital

    char *trail =
        "GLS.... SMR.... HCD.... MAM.... DSN.V.. "
        "GMA.... SCF.... HGA.... MCO.... DSRT... "
        "GSNV... SMR.... HCD.... MAM.... DMAT... "
        "GSRT... SCF.... HGA.... MBU.... DHIT... "
        "GMATTD. SCF.... HGA.... MBU.... DD1T... "
        "GZA.... SCF.... HGA....";

    player_message messages[28] = {"Going somewhere", ""};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_LORD_GODALMING, ZAGREB);
    TEST(hv_get_health(hv, PLAYER_LORD_GODALMING) == 9, "Godalming is revived");

    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_LORD_GODALMING, history);
    HISTORY_IS(PLAYER_LORD_GODALMING, 0, ZAGREB);
    HISTORY_IS(PLAYER_LORD_GODALMING, 1, MADRID);
    HISTORY_IS(PLAYER_LORD_GODALMING, 2, SARAGOSSA);
    HISTORY_IS(PLAYER_LORD_GODALMING, 3, SANTANDER);
    HISTORY_IS(PLAYER_LORD_GODALMING, 4, MADRID);
    HISTORY_IS(PLAYER_LORD_GODALMING, 5, LISBON);

    IS_AT(PLAYER_DRACULA, MADRID);
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, DOUBLE_BACK_1);
    HISTORY_IS(PLAYER_DRACULA, 1, HIDE);
    HISTORY_IS(PLAYER_DRACULA, 2, MADRID);
    HISTORY_IS(PLAYER_DRACULA, 3, SARAGOSSA);
    HISTORY_IS(PLAYER_DRACULA, 4, SANTANDER);
    HISTORY_IS(PLAYER_DRACULA, 5, UNKNOWN_LOCATION);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking where is an hv_get_dests_player for dracula

    char *trail =
        "GED.... SGE.... HZU.... MCA.... DCF.V.. "
        "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
        "GLO.... SMR.... HCF.... MMA.... DC?T... "
        "GPL.... SMS.... HMR.... MGR.... DBAT... "
        "GLO.... SBATD.. HMS.... MMA.... DC?T... "
        "GPL.... SSJ.... HBA.... MGR.... DD1T...";

    player_message messages[30] = {"Going somewhere", ""};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_DRACULA, DOUBLE_BACK_1);
    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, DOUBLE_BACK_1);
    HISTORY_IS(PLAYER_DRACULA, 1, CITY_UNKNOWN);
    HISTORY_IS(PLAYER_DRACULA, 2, BARCELONA);
    HISTORY_IS(PLAYER_DRACULA, 3, CITY_UNKNOWN);
    HISTORY_IS(PLAYER_DRACULA, 4, CITY_UNKNOWN);
    HISTORY_IS(PLAYER_DRACULA, 5, CLERMONT_FERRAND);

    int size, *edges = hv_get_dests_player(hv, &size, PLAYER_DRACULA, true,
                                           false, true);
    bool seen[NUM_ALL_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) {
      if (!valid_location_p(edges[i]))
        eprintf("... invalid location '%d'", edges[i]);
      seen[edges[i]] = true;
    }

    TEST(size == 0, "hv_get_dests_player(hv, Dracula, road, sea): 0 locations");

    free(edges);
    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking where is an hv_get_dests_player for dracula

    char *trail =
        "GED.... SCF.... HZU.... MCA.... DTO.V.. "
        "GMN.... STOVD.. HGE.... MLS.... DD1T... "
        "GLO.... SMR.... HCF.... MMA.... DC?T... "
        "GPL.... SMS.... HMR.... MGR.... DBAT... "
        "GLO.... SBATD..";

    player_message messages[22] = {"Going somewhere", ""};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_DRACULA, BARCELONA);
    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, BARCELONA);
    HISTORY_IS(PLAYER_DRACULA, 1, CITY_UNKNOWN);
    HISTORY_IS(PLAYER_DRACULA, 2, DOUBLE_BACK_1);
    HISTORY_IS(PLAYER_DRACULA, 3, TOULOUSE);
    HISTORY_IS(PLAYER_DRACULA, 4, UNKNOWN_LOCATION);
    HISTORY_IS(PLAYER_DRACULA, 5, UNKNOWN_LOCATION);

    int size, *edges = hv_get_dests_player(hv, &size, PLAYER_DRACULA, true,
                                           false, true);
    bool seen[NUM_ALL_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) {
      if (!valid_location_p(edges[i]))
        eprintf("... invalid location '%d'", edges[i]);
      seen[edges[i]] = true;
    }

    TEST(size == 3, "hv_get_dests_player(hv, Dracula, road, sea): 3 locations");

    EXPECT_TO_SEE(MEDITERRANEAN_SEA);
    EXPECT_TO_SEE(SARAGOSSA);
    EXPECT_TO_SEE(BARCELONA);

    free(edges);
    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    // Checking where is and trail for dracula with TP

    char *trail =
        "GMA.... SMN.... HAM.... MGA.... DGA.V.. "
        "GAL.... SED.... HBU.... MGAVD.. DCDT... "
        "GMS.... SNS.... HCO.... MCDTD.. DKLT... "
        "GMR.... SHA.... HFR.... MCD.... DD2T... "
        "GGO.... SBR.... HBR.... MCDTD.. DHIT... "
        "GVE.... SPR.... HBD.... MJM.... DTPT... "
        "GBD.... SBD.... HKLT...";

    player_message messages[33] = {"Going somewhere", ""};
    HunterView hv = hv_new(trail, messages);

    TEST(hv != NULL, "hv := hv_new(\"%s\")", trail);
    TEST(!mem_address_is_poisoned(hv), "hv is a valid allocation");

    IS_AT(PLAYER_DRACULA, CASTLE_DRACULA);
    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    HISTORY_IS(PLAYER_DRACULA, 0, TELEPORT);
    HISTORY_IS(PLAYER_DRACULA, 1, HIDE);
    HISTORY_IS(PLAYER_DRACULA, 2, DOUBLE_BACK_2);
    HISTORY_IS(PLAYER_DRACULA, 3, KLAUSENBURG);
    HISTORY_IS(PLAYER_DRACULA, 4, CASTLE_DRACULA);
    HISTORY_IS(PLAYER_DRACULA, 5, GALATZ);

    hv_drop(hv);
    TEST(mem_address_is_poisoned(hv), "hv is no longer a valid allocation");
  } while (0);

  return EXIT_SUCCESS;
}

static const char *player_to_s(enum player p) {
  switch (p) {
    case PLAYER_LORD_GODALMING:
      return "Godalming";
    case PLAYER_DR_SEWARD:
      return "Seward";
    case PLAYER_VAN_HELSING:
      return "vHelsing";
    case PLAYER_MINA_HARKER:
      return "Harker";
    case PLAYER_DRACULA:
      return "Dracula";
  }
}
