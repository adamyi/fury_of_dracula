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

  ac_compare_int((int)gv_get_player(gv), PLAYER_LORD_GODALMING,
                 "gv_get_player(gv) == Godalming");
  ac_compare_int((int)gv_get_round(gv), 0, "round is 0");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct starting life");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS,
                 "gv_get_health(gv, Dracula) == 40 (start)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE,
                 "gv_get_score(gv) == 366");
  ac_compare_int((int)gv_get_location(gv, PLAYER_LORD_GODALMING),
                 (int)UNKNOWN_LOCATION,
                 "Godalming is in correct start location");

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

  ac_compare_int((int)gv_get_player(gv), PLAYER_DRACULA,
                 "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 3, "round is 3");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS - 4,
                 "Dr Steward has correct life (5)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 10,
                 "gv_get_health(gv, Dracula) == 30 (start - encounter)");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 3,
                 "gv_get_score(gv) == 366 - 3");
  ac_compare_int((int)gv_get_location(gv, PLAYER_DR_SEWARD),
                 (int)MEDITERRANEAN_SEA, "Dr S is in Med Sea");

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
