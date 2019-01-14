////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_hunter_view.c: test the HunterView ADT
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hunter_view.h"

int main(void) {
  do {  ////////////////////////////////////////////////////////////////
    puts("Test basic empty initialisation");
    char *trail = "";
    player_message messages[] = {};
    HunterView hv = hv_new(trail, messages);

    assert(hv_get_player(hv) == PLAYER_LORD_GODALMING);
    assert(hv_get_round(hv) == 0);
    assert(hv_get_health(hv, PLAYER_DR_SEWARD) ==
           GAME_START_HUNTER_LIFE_POINTS);
    assert(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
    assert(hv_get_score(hv) == GAME_START_SCORE);
    assert(hv_get_location(hv, PLAYER_LORD_GODALMING) == UNKNOWN_LOCATION);

    puts("passed");
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts("Test for Dracula trail and basic functions");

    char *trail = "GST.... SAO.... HZU.... MBB.... DC?....";
    player_message messages[] = {"Hello", "Rubbish", "Stuff", "", "Mwahahah"};
    HunterView hv = hv_new(trail, messages);

    assert(hv_get_player(hv) == PLAYER_LORD_GODALMING);
    assert(hv_get_round(hv) == 1);
    assert(hv_get_location(hv, PLAYER_LORD_GODALMING) == STRASBOURG);
    assert(hv_get_location(hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
    assert(hv_get_location(hv, PLAYER_VAN_HELSING) == ZURICH);
    assert(hv_get_location(hv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
    assert(hv_get_location(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
    assert(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);

    puts("passed");
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts("Test for encountering Dracula and hunter history");
    char *trail =
        "GST.... SAO.... HCD.... MAO.... DGE.... "
        "GGED...";
    player_message messages[] = {"Hello", "Rubbish",  "Stuff",
                                 "",      "Mwahahah", "Aha!"};
    HunterView hv = hv_new(trail, messages);

    assert(hv_get_location(hv, PLAYER_DRACULA) == GENEVA);
    assert(hv_get_health(hv, PLAYER_LORD_GODALMING) == 5);
    assert(hv_get_health(hv, PLAYER_DRACULA) == 30);
    assert(hv_get_location(hv, PLAYER_LORD_GODALMING) == GENEVA);

    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    assert(history[0] == GENEVA);
    assert(history[1] == UNKNOWN_LOCATION);

    hv_get_trail(hv, PLAYER_LORD_GODALMING, history);
    assert(history[0] == GENEVA);
    assert(history[1] == STRASBOURG);
    assert(history[2] == UNKNOWN_LOCATION);

    hv_get_trail(hv, PLAYER_DR_SEWARD, history);
    assert(history[0] == ATLANTIC_OCEAN);
    assert(history[1] == UNKNOWN_LOCATION);

    puts("passed");
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts(
        "Test for Dracula doubling back at sea, "
        "and losing blood points (Hunter View)");
    char *trail =
        "GGE.... SGE.... HGE.... MGE.... DS?.... "
        "GST.... SST.... HST.... MST.... DD1....";
    player_message messages[] = {"Hello", "Rubbish", "Stuff", "", "Mwahahah",
                                 "Aha!",  "",        "",      "", "Back I go"};
    HunterView hv = hv_new(trail, messages);

    assert(hv_get_player(hv) == 0);
    assert(hv_get_location(hv, PLAYER_DRACULA) == DOUBLE_BACK_1);
    assert(hv_get_health(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS - 4);

    location_t history[TRAIL_SIZE];
    hv_get_trail(hv, PLAYER_DRACULA, history);
    assert(history[0] == DOUBLE_BACK_1);
    assert(history[1] == SEA_UNKNOWN);

    puts("passed");
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts("Checking Galatz road connections");
    char *trail = "GGA....";
    player_message messages[] = {"Gone to Galatz"};
    HunterView hv = hv_new(trail, messages);

    size_t size;
    location_t *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                            true, false, false);
    bool seen[NUM_MAP_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) seen[edges[i]] = true;

    assert(size == 5);
    assert(seen[GALATZ]);
    assert(seen[CONSTANTA]);
    assert(seen[BUCHAREST]);
    assert(seen[KLAUSENBURG]);
    assert(seen[CASTLE_DRACULA]);

    puts("passed");
    free(edges);
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts("Checking Ionian Sea sea connections");
    char *trail = "GIO....";
    player_message messages[] = {"Sailing the Ionian"};
    HunterView hv = hv_new(trail, messages);

    size_t size;
    location_t *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                            false, false, true);
    bool seen[NUM_MAP_LOCATIONS] = {false};
    for (int i = 0; i < size; i++) seen[edges[i]] = true;

    assert(size == 7);
    assert(seen[IONIAN_SEA]);
    assert(seen[BLACK_SEA]);
    assert(seen[ADRIATIC_SEA]);
    assert(seen[TYRRHENIAN_SEA]);
    assert(seen[ATHENS]);
    assert(seen[VALONA]);
    assert(seen[SALONICA]);

    puts("passed");
    free(edges);
    hv_drop(hv);
  } while (0);

  do {  ////////////////////////////////////////////////////////////////
    puts("Checking Athens rail connections (none)");

    char *trail = "GAT....";
    player_message messages[] = {"Leaving Athens by train"};
    HunterView hv = hv_new(trail, messages);

    size_t size;
    location_t *edges = hv_get_dests_player(hv, &size, PLAYER_LORD_GODALMING,
                                            false, true, false);

    assert(size == 1);
    assert(edges[0] == ATHENS);

    puts("passed");
    free(edges);
    hv_drop(hv);
  } while (0);

  return EXIT_SUCCESS;
}
