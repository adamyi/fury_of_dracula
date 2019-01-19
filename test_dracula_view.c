////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_dracula_view.c: test the DraculaView ADT
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_log.h"
#include "ac_test.h"

#include "dracula_view.h"

TEST_SET_FIXTURE(sampleFixtureTest) {
  int *x = malloc(3 * sizeof(int));
  x[0] = 0;
  x[1] = 1;
  x[2] = 2;
  return x;
}

TEST(encounterTest, drS_encounters_drac_and_vamp) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
      "GLO.... SMR.... HCF.... MMA.... DTOT..."
      "GPL.... SMS.... HMR.... MGR....";

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 3, "round is 3");
  ac_compare_int(dv_get_health(dv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS - 4,
                 "Dr Steward has correct life (5)");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 10,
                 "dv_get_health(dv, Dracula) == 30 (start - encounter)");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 3,
                 "dv_get_score(dv) == 366 - 3");
  ac_compare_int(dv_get_location(dv, PLAYER_DR_SEWARD), MEDITERRANEAN_SEA,
                 "Dr S is in Med Sea");
  dv_drop(dv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
      "GLO.... SMR.... HCF.... MMA.... DTOT..."
      "GPL.... SMS.... HMR.... MGR.... DBAT..."
      "GEC.... SBATD.. HGO.... MMA....";

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 4, "round is 4");
  ac_compare_int(dv_get_health(dv, PLAYER_DR_SEWARD), 0,
                 "Dr Steward has correct life (0)");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 20,
                 "dv_get_health(dv, Dracula) == 20 (start - 2*encounter)");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 4 - 6,
                 "dv_get_score(dv) == 366 - 10");
  dv_drop(dv);
}

TEST(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
      "GLO.... SMR.... HCF.... MMA.... DTOT..."
      "GPL.... SMS.... HMR.... MGR.... DBAT..."
      "GEC.... SBATD.. HGO.... MAL.... DMS...."
      "GLE.... SZA.... HTS.... MMS....";

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 5, "round is 5");
  ac_compare_int(dv_get_health(dv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int(
      dv_get_health(dv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20 - 2,
      "dv_get_health(dv, Dracula) == 20 (start - 2*encounter - 1*sea)");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 5 - 6,
                 "dv_get_score(dv) == 366 - 10");
  dv_drop(dv);
}

TEST(castleDracTest, drac_heals_in_castle) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."   // 0
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."   // 1
      "GLO.... SMR.... HCF.... MMA.... DTOT..."   // 2
      "GPL.... SMS.... HMR.... MGR.... DBAT..."   // 3
      "GEC.... SBATD.. HGO.... MAL.... DMS...."   // 4  -2
      "GLE.... SZA.... HTS.... MMS.... DTS...."   // 5  -2
      "GLE.... SZA.... HTS.... MMS.... DIO...."   // 6  -2
      "GLE.... SZA.... HTS.... MMS.... DBS...."   // 7  -2
      "GLE.... SZA.... HTS.... MMS.... DCNT..."   // 8
      "GLE.... SZA.... HTS.... MMS.... DGAT..."   // 8
      "GLE.... SZA.... HTS.... MMS.... DCDT...";  // 10

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 11, "round is 11");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 20 - 8 + 10,
                 "Dracula has correct life (22)");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 11 - 6,
                 "dv_get_score(dv) == 366 - 17");
  ac_compare_int(dv_get_location(dv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "Dracula is in the Castle");

  dv_drop(dv);
}

TEST(castleDracTest, drac_heals_check_traps_check_locs) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCD.V.."  // 0
      "GED.... SGE.... HZU.... MCA.... DHIT..."  // 1
      "GED.... SGE.... HZU.... MCA....";         // 2
  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 2, "round is 2");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA), 60,
                 "Dracula has correct life (60) (no-max)");
  ac_compare_int(dv_get_health(dv, PLAYER_LORD_GODALMING),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(dv_get_health(dv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(dv_get_health(dv, PLAYER_VAN_HELSING),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(dv_get_health(dv, PLAYER_MINA_HARKER),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 2,
                 "gv_get_score(dv) == 366 - 2");
  ac_compare_int(dv_get_location(dv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "Dracula is in the Castle");
  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, CASTLE_DRACULA, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 1, "n_traps == 1");
  ac_compare_int(n_vamps, 1, "n_vamps == 1");

  location_t start = -1;
  location_t end = -1;
  dv_get_player_move(dv, PLAYER_MINA_HARKER, &start, &end);
  ac_compare_int(start, CADIZ, "Mina starts her move in Geneva");
  ac_compare_int(end, CADIZ, "Mina ends her move in Geneva");

  dv_get_player_move(dv, PLAYER_DRACULA, &start, &end);
  ac_compare_int(start, CASTLE_DRACULA, "Dracula starts his move in Geneva");
  ac_compare_int(end, HIDE, "Dracula ends his move in Geneva");

  size_t n_locations = 0;
  location_t *dests =
      dv_get_dests_player(dv, &n_locations, PLAYER_DRACULA, true, true, true);
  ac_compare_int(dests[0], CASTLE_DRACULA, "Casle Dracula in dests");
  ac_compare_int(dests[1], GALATZ, "GA in dests");
  ac_compare_int(dests[2], KLAUSENBURG, "KL in dests");
  ac_compare_int(n_locations, 3, "3 reachable locations from the Castle");
  free(dests);

  dv_drop(dv);
}

TEST(sampleTest, intTest) {
  ac_compare_int(1, 1, "value a");
  ac_compare_int(2, 2, "value b");
  ac_compare_int(3, 3, "value c");
}

TEST(sampleTest, strTest) {
  ac_compare_string("a", "a", "test string");
  ac_compare_string("hello", "hello", "anotther test string");
}

TEST_F(sampleFixtureTest, zeroTest) {
  int *x = (int *)fixture;
  ac_compare_int(x[0], 0, "x[0]");
  free(x);
}

TEST_F(sampleFixtureTest, oneTest) {
  int *x = (int *)fixture;
  ac_compare_int(x[1], 1, "x[1]");
  free(x);
}

TEST_F(sampleFixtureTest, twoTest) {
  int *x = (int *)fixture;
  ac_compare_int(x[2], 2, "x[2]");
  free(x);
}

TEST_F(sampleFixtureTest, exampleFailTest) {
  int *x = (int *)fixture;
  ac_compare_int(x[2], x[1], "x should be identical");
  free(x);
}

// register all tests
// tests will run in the same order they are registered.
static void regAllTests() {
  ac_regTest(sampleTest, intTest);
  ac_regTest(sampleTest, strTest);
  ac_regTest(sampleFixtureTest, zeroTest);
  ac_regTest(sampleFixtureTest, oneTest);
  ac_regTest(sampleFixtureTest, twoTest);
  ac_regTest(sampleFixtureTest, exampleFailTest);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap);
  ac_regTest(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg);
  ac_regTest(castleDracTest, drac_heals_in_castle);
  ac_regTest(castleDracTest, drac_heals_check_traps_check_locs);
}

int main() {
  ac_setLoggingTag("INIT");

  ac_log(AC_LOG_INFO, "Hello world");

  regAllTests();
  ac_runAllTests();

  ac_setLoggingTag("BYE");
  ac_log(AC_LOG_INFO, "Have a nice day!");
  return EXIT_SUCCESS;
}
