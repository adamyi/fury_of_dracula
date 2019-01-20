////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// test_hunter_view.c: test the GameView ADT
//
// Adam Yi <i@adamyi.com>, Simon Hanly-Jones <simon.hanly.jones@gmail.com>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_log.h"
#include "ac_test.h"

#include "hunter_view.h"

TEST_SET_FIXTURE(sampleFixtureTest) {
  int *x = malloc(3 * sizeof(int));
  x[0] = 0;
  x[1] = 1;
  x[2] = 2;
  return x;
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

TEST(basic_test, test_score_start_HP_and_sea_cons) {
  char *trail = "GED.... SGE.... HZU.... MCA.... DC?.V.."
                "GNS.... SPA.... HST.... MLS.... DC?T...";  // 0

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  size_t n_locations = 0;

  location_t *lordGSeaDests = hv_get_dests(hv, &n_locations, false, false, true);

  bool sea_destsLG_key[NUM_MAP_LOCATIONS];
  memset(sea_destsLG_key, false, NUM_MAP_LOCATIONS);
  sea_destsLG_key[4] = true;
  sea_destsLG_key[2] = true;
  sea_destsLG_key[31] = true;
  sea_destsLG_key[22] = true;
  sea_destsLG_key[23] = true;
  sea_destsLG_key[48] = true;

  bool sea_destsLG_ret[NUM_MAP_LOCATIONS];
  memset(sea_destsLG_ret, false, NUM_MAP_LOCATIONS);

  puts("LG SEA LOCS");
  for(size_t i = 0; i < n_locations; i++){
    printf("loc: %d\n", lordGSeaDests[i]);
  }
  for(size_t i = 0; i < n_locations; i++){
    sea_destsLG_ret[lordGSeaDests[i]] = true;
  }

  for(size_t i = 0; i < NUM_MAP_LOCATIONS; i++){
    ac_compare_int(sea_destsLG_key[i], sea_destsLG_ret[i], "location match");
  }
  free(lordGSeaDests);
  hv_drop(hv);



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
  ac_regTest(basic_test, test_score_start_HP_and_sea_cons);
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
