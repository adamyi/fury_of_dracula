////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_game_view.c: test the GameView ADT
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_log.h"
#include "ac_test.h"

#include "game_view.h"

TEST_SET_FIXTURE(sampleFixtureTest) {
  int *x = malloc(3 * sizeof(int));
  x[0] = 0;
  x[1] = 1;
  x[2] = 2;
  return x;
}

TEST(basicTest, replicate_dryrun1) {

  char *trail = "";
  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_LORD_GODALMING, "gv_get_player(gv) == Godalming");
  ac_compare_int(gv_get_round(gv), 0, "round is 0");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD), GAME_START_HUNTER_LIFE_POINTS, "Dr Steward has correct starting life");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS, "gv_get_health(gv, Dracula) == 40 (start)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE, "gv_get_score(gv) == 366");
  ac_compare_int(gv_get_location(gv, PLAYER_LORD_GODALMING), UNKNOWN_LOCATION, "Godalming is in correct start location");

  gv_drop(gv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
      "GLO.... SMR.... HCF.... MMA.... DTOT..."
      "GPL.... SMS.... HMR.... MGR....";

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_DRACULA,
                 "gv_get_player(gv) == Dracula");
  ac_compare_int(gv_get_round(gv), 3, "round is 3");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS - 4,
                 "Dr Steward has correct life (5)");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 10,
                 "gv_get_health(gv, Dracula) == 30 (start - encounter)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE - 3,
                 "gv_get_score(gv) == 366 - 3");
  ac_compare_int(gv_get_location(gv, PLAYER_DR_SEWARD),
                 MEDITERRANEAN_SEA, "Dr S is in Med Sea");
  gv_drop(gv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.."
      "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
      "GLO.... SMR.... HCF.... MMA.... DTOT..."
      "GPL.... SMS.... HMR.... MGR.... DBAT..."
      "GEC.... SBATD.. HGO.... MMA....";

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_DRACULA,
                 "gv_get_player(gv) == Dracula");
  ac_compare_int(gv_get_round(gv), 4, "round is 4");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD), 0,
                 "Dr Steward has correct life (0)");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 20,
                 "gv_get_health(gv, Dracula) == 20 (start - 2*encounter)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE - 4 - 6,
                 "gv_get_score(gv) == 366 - 10");
  gv_drop(gv);
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
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_DRACULA,
                 "gv_get_player(gv) == Dracula");
  ac_compare_int(gv_get_round(gv), 5, "round is 5");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int(
      gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20 - 2,
      "gv_get_health(gv, Dracula) == 20 (start - 2*encounter - 1*sea)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE - 5 - 6,
                 "gv_get_score(gv) == 366 - 10");
  gv_drop(gv);
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
      "GLE.... SZA.... HTS.... MMS.... DBS..M."   // 7  -2
      "GLE.... SZA.... HTS.... MMS.... DCNT.M."   // 8
      "GLE.... SZA.... HTS.... MMS.... DGAT..."   // 8
      "GLE.... SZA.... HTS.... MMS.... DCDT...";  // 10

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_LORD_GODALMING,
                 "gv_get_player(gv) == Lord Godalming");
  ac_compare_int(gv_get_round(gv), 11, "round is 11");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 20 - 8 + 10,
                 "Dracula has correct life (22)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE - 11 - 6,
                 "gv_get_score(gv) == 366 - 17");
  ac_compare_int(gv_get_location(gv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "Dracula is in the Castle");

  gv_drop(gv);
}

TEST(castleDracTest, drac_heals_in_castle_to_max) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCD.V.."   // 0
      "GED.... SGE.... HZU.... MCA.... DHIT...";
  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_LORD_GODALMING,
                 "gv_get_player(gv) == Godalming");
  ac_compare_int(gv_get_round(gv), 2, "round is 2");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA), 60,
                 "Dracula has correct life (60) (no-max)");
  ac_compare_int(gv_get_health(gv, PLAYER_LORD_GODALMING),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(gv_get_health(gv, PLAYER_VAN_HELSING),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(gv_get_health(gv, PLAYER_MINA_HARKER),
                 GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE - 2,
                 "gv_get_score(gv) == 366 - 2");
  ac_compare_int(gv_get_location(gv, PLAYER_DRACULA), HIDE,
                 "Dracula is in hide location");
  gv_drop(gv);
}

TEST(vamp_mat_test, test_vamp_maturity_consequences){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DC?.V.."//0
    "GMN.... SCF.... HGE.... MLS.... DC?T..."//1
    "GLO.... SMR.... HCF.... MMA.... DC?T..."//2
    "GPL.... SMS.... HMR.... MGR.... DC?T..."//3
    "GEC.... SBA.... HGO.... MAL.... DC?T..."//4
    "GLE.... SZA.... HTS.... MMS.... DC?T..."//5
    "GPA.... SVI.... HIO.... MCG.... DC?T.V.";//6
                                              //7


  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_LORD_GODALMING, "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 7, "round is 7");

  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD), GAME_START_HUNTER_LIFE_POINTS, "Dr Steward has correct life (9)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS, "gv_get_health(gv, Dracula) == 40");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 7 - 13, "gv_get_score(gv) == 366 - 20");
  gv_drop(gv);
}

TEST(conn_test, test_connections_from_start_locs){
  char *trail =
      "GED.... SGE.... HZU.... MCA....";   // 0

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  size_t n_locations = 0;
  location_t *destsLG = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING), PLAYER_LORD_GODALMING, gv_get_round(gv), true, false, false);

  ac_compare_int(destsLG[0], EDINBURGH, "current loc Ed");
  ac_compare_int(destsLG[1], MANCHESTER, "Man avaialbe by road");
  free(destsLG);
  n_locations = 0;
  location_t *destsLGrail = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING), PLAYER_LORD_GODALMING, gv_get_round(gv), false, true, false);
  /*puts("LG RAIL LOCS");
  printf("0%4: %d\n", 0%4);
  for(size_t i = 0; i < n_locations; i++){
    printf("loc: %d\n", destsLGrail[i]);
  }*/
  ac_compare_int(destsLGrail[0], EDINBURGH, "current loc Ed");
  free(destsLGrail);
  n_locations = 0;
  location_t *destsLGsea = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING), PLAYER_LORD_GODALMING, gv_get_round(gv), false, false, true);
  ac_compare_int(destsLGsea[0], EDINBURGH, "current loc Ed");
  ac_compare_int(destsLGsea[1], NORTH_SEA, "North Sea should be accessable");

  n_locations = 0;
  location_t *destsLGall = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING), PLAYER_LORD_GODALMING, gv_get_round(gv), true, true, true);
  ac_compare_int(destsLGall[0], EDINBURGH, "current loc Ed");
  ac_compare_int(destsLGall[1], MANCHESTER, "man is adjacent");
  ac_compare_int(destsLGall[2], NORTH_SEA, "NS is adjacent");
  ac_compare_int(n_locations, 3, "n_locations == 3");
  free(destsLGall);


  n_locations = 0;
  location_t *destsVHroad = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_VAN_HELSING), PLAYER_VAN_HELSING, gv_get_round(gv), true, false, true);
  ac_compare_int(destsVHroad[0], GENEVA, "Geneva");
  ac_compare_int(destsVHroad[1], MARSEILLES, "MARSEILLES");
  ac_compare_int(destsVHroad[2], MILAN, "MILAN");
  ac_compare_int(destsVHroad[3], MUNICH, "MUNICH");
  ac_compare_int(destsVHroad[4], STRASBOURG, "STRASBOURG");
  ac_compare_int(destsVHroad[5], ZURICH, "ZURICH");
  ac_compare_int(n_locations, 6, "6 == n_locations");
  free(destsVHroad);

  n_locations = 0;
  location_t *destsVHrail = gv_get_connections(gv, &n_locations, gv_get_location(gv, PLAYER_VAN_HELSING), PLAYER_VAN_HELSING, gv_get_round(gv), false, true, true);

  ac_compare_int(n_locations, 7, "7 == n_locations");
  ac_compare_int(destsVHrail[0], 24, "Florence");
  ac_compare_int(destsVHrail[1], 25, "Frankfurt");
  ac_compare_int(destsVHrail[2], 28, "Geneva");
  ac_compare_int(destsVHrail[3], 29, "Genoa");
  ac_compare_int(destsVHrail[4], 44, "Milan");
  ac_compare_int(destsVHrail[5], 60, "Strasbourg");
  ac_compare_int(destsVHrail[6], 70, "Zurich");

  puts("VH RAIL LOCS");
  for(size_t i = 0; i < n_locations; i++){
    printf("loc: %d\n", destsVHrail[i]);
  }
  free(destsVHrail);


  gv_drop(gv);
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
  ac_regTest(basicTest, replicate_dryrun1);
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
  ac_regTest(castleDracTest, drac_heals_in_castle_to_max);
  ac_regTest(vamp_mat_test, test_vamp_maturity_consequences);
  ac_regTest(conn_test, test_connections_from_start_locs);
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
