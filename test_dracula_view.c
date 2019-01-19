////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_dracula_view.c: test the GameView ADT
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

TEST(encounterTest, drS_encounters_drac_and_vamp){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DCF.V.."
    "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
    "GLO.... SMR.... HCF.... MMA.... DTOT..."
    "GPL.... SMS.... HMR.... MGR....";



  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_DRACULA, "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 3, "round is 3");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD), GAME_START_HUNTER_LIFE_POINTS - 4, "Dr Steward has correct life (5)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 10, "gv_get_health(gv, Dracula) == 30 (start - encounter)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 3, "gv_get_score(gv) == 366 - 3");
  ac_compare_int((int)gv_get_location(gv, PLAYER_DR_SEWARD), (int)MEDITERRANEAN_SEA, "Dr S is in Med Sea");
  gv_drop(gv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DCF.V.."
    "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
    "GLO.... SMR.... HCF.... MMA.... DTOT..."
    "GPL.... SMS.... HMR.... MGR.... DBAT..."
    "GEC.... SBATD.. HGO.... MMA....";


  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_DRACULA, "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 4, "round is 4");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD), 0, "Dr Steward has correct life (0)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20, "gv_get_health(gv, Dracula) == 20 (start - 2*encounter)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 4 - 6, "gv_get_score(gv) == 366 - 10");
  gv_drop(gv);
}

TEST(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DCF.V.."
    "GMN.... SCFVD.. HGE.... MLS.... DBOT..."
    "GLO.... SMR.... HCF.... MMA.... DTOT..."
    "GPL.... SMS.... HMR.... MGR.... DBAT..."
    "GEC.... SBATD.. HGO.... MAL.... DMS...."
    "GLE.... SZA.... HTS.... MMS....";


  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);


  ac_compare_int((int)gv_get_player(gv), PLAYER_DRACULA, "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 5, "round is 5");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD), GAME_START_HUNTER_LIFE_POINTS, "Dr Steward has correct life (9)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20 - 2, "gv_get_health(gv, Dracula) == 20 (start - 2*encounter - 1*sea)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 5 - 6, "gv_get_score(gv) == 366 - 10");
  gv_drop(gv);
}

TEST(castleDracTest, drac_heals_in_castle){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DCF.V.."//0
    "GMN.... SCFVD.. HGE.... MLS.... DBOT..."//1
    "GLO.... SMR.... HCF.... MMA.... DTOT..."//2
    "GPL.... SMS.... HMR.... MGR.... DBAT..."//3
    "GEC.... SBATD.. HGO.... MAL.... DMS...."//4  -2
    "GLE.... SZA.... HTS.... MMS.... DTS...."//5  -2
    "GLE.... SZA.... HTS.... MMS.... DIO...."//6  -2
    "GLE.... SZA.... HTS.... MMS.... DBS...."//7  -2
    "GLE.... SZA.... HTS.... MMS.... DCNT..."//8
    "GLE.... SZA.... HTS.... MMS.... DGAT..."//8
    "GLE.... SZA.... HTS.... MMS.... DCDT...";//10


  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_LORD_GODALMING, "gv_get_player(gv) == Lord Godalming");
  ac_compare_int((int)gv_get_round(gv), 11, "round is 11");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20 - 8 + 10, "Dracula has correct life (22)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 11 - 6, "gv_get_score(gv) == 366 - 17");
  ac_compare_int((int)gv_get_location(gv, PLAYER_DRACULA), (int)CASTLE_DRACULA, "Dracula is in the Castle");

  gv_drop(gv);
}

TEST(castleDracTest, drac_heals_in_castle_to_max){
  char *trail =
    "GED.... SGE.... HZU.... MCA.... DCF.V.."//0
    "GMN.... SCFVD.. HGE.... MLS.... DBOT..."//1
    "GLO.... SMR.... HCF.... MMA.... DTOT..."//2
    "GPL.... SMS.... HMR.... MGR.... DBAT..."//3
    "GEC.... SBATD.. HGO.... MAL.... DMS...."//4  -2
    "GLE.... SZA.... HTS.... MMS.... DTS...."//5  -2
    "GLE.... SZA.... HTS.... MMS.... DIO...."//6  -2
    "GLE.... SZA.... HTS.... MMS.... DBS...."//7  -2
    "GLE.... SZA.... HTS.... MMS.... DCNT..."//8
    "GLE.... SZA.... HTS.... MMS.... DGAT..."//9
    "GLE.... SZA.... HTS.... MMS.... DCDT..."//10
    "GLE.... SZA.... HTS.... MMS.... DCDT..."//11
    "GLE.... SZA.... HTS.... MMS.... DCDT..."//12
    "GLE.... SZA.... HTS.... MMS.... DCDT..."//13
    "GLE.... SZA.... HTS.... MMS.... DCDT..."//14
    "GLE.... SZA.... HTS.... MMS.... DCDT...";//15


  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_LORD_GODALMING, "gv_get_player(gv) == Lord Godalming");
  ac_compare_int((int)gv_get_round(gv), 16, "round is 16");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA), 72, "Dracula has correct life (72) (no-max)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_LORD_GODALMING), GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD), GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int((int)gv_get_health(gv, PLAYER_VAN_HELSING), GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int((int)gv_get_health(gv, PLAYER_MINA_HARKER), GAME_START_HUNTER_LIFE_POINTS, "player has full health");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 16 - 6, "gv_get_score(gv) == 366 - 22");
  ac_compare_int((int)gv_get_location(gv, PLAYER_DRACULA), (int)CASTLE_DRACULA, "Dracula is in the Castle");
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
