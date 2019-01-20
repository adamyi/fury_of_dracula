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

TEST(basicTest, replicate_dryrun1) {
  char *trail = "";
  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int(hv_get_player(hv), PLAYER_LORD_GODALMING,
                 "hv_get_player(hv) == Godalming");
  ac_compare_int(hv_get_round(hv), 0, "round is 0");
  ac_compare_int(hv_get_health(hv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct starting life");
  ac_compare_int(hv_get_health(hv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS,
                 "hv_get_health(hv, Dracula) == 40 (start)");
  ac_compare_int(hv_get_score(hv), GAME_START_SCORE, "hv_get_score(hv) == 366");
  ac_compare_int(hv_get_location(hv, PLAYER_LORD_GODALMING), UNKNOWN_LOCATION,
                 "Godalming is in correct start location");

  hv_drop(hv);
}

TEST(connsTest, test_score_start_HP_and_sea_cons) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DC?.V.. "  // 0
      "GNS.... SPA.... HST.... MLS.... DC?T...";  // 1
                                                  // 2
  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  size_t n_locations = 0;
  location_t *lordGSeaDests =
      hv_get_dests(hv, &n_locations, false, false, true);

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

  for (size_t i = 0; i < n_locations; i++) {
    sea_destsLG_ret[lordGSeaDests[i]] = true;
  }

  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    ac_compare_int(sea_destsLG_key[i], sea_destsLG_ret[i], "location match");
  }
  free(lordGSeaDests);

  n_locations = 0;
  location_t *lordGAllDests = hv_get_dests(hv, &n_locations, true, true, true);
  memset(sea_destsLG_ret, false, NUM_MAP_LOCATIONS);
  for (size_t i = 0; i < n_locations; i++) {
    sea_destsLG_ret[lordGAllDests[i]] = true;
  }
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    ac_compare_int(sea_destsLG_key[i], sea_destsLG_ret[i], "location match");
  }
  free(lordGAllDests);

  n_locations = 0;
  ac_compare_int(hv_get_round(hv), 2, "round == 2");
  location_t *minaRailLocs = hv_get_dests_player(
      hv, &n_locations, PLAYER_MINA_HARKER, false, true, false);
  ac_compare_int(n_locations, 2, "n_locations == 2");
  bool mina_dests_key[NUM_MAP_LOCATIONS];
  memset(mina_dests_key, false, NUM_MAP_LOCATIONS);
  mina_dests_key[LISBON] = true;
  mina_dests_key[MADRID] = true;

  bool mina_dests_ret[NUM_MAP_LOCATIONS];
  memset(mina_dests_ret, false, NUM_MAP_LOCATIONS);
  for (size_t i = 0; i < n_locations; i++) {
    mina_dests_ret[minaRailLocs[i]] = true;
  }
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    ac_compare_int(mina_dests_key[i], mina_dests_ret[i], "location match");
  }
  free(minaRailLocs);

  n_locations = 0;
  location_t *minaAllLocs = hv_get_dests_player(
      hv, &n_locations, PLAYER_MINA_HARKER, true, true, true);

  memset(mina_dests_ret, false, NUM_MAP_LOCATIONS);
  for (size_t i = 0; i < n_locations; i++) {
    mina_dests_ret[minaAllLocs[i]] = true;
  }
  mina_dests_key[ATLANTIC_OCEAN] = true;
  mina_dests_key[CADIZ] = true;
  mina_dests_key[SANTANDER] = true;
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    ac_compare_int(mina_dests_key[i], mina_dests_ret[i], "location match");
  }
  free(minaAllLocs);

  hv_drop(hv);
}

// adapt test from DV to HV to test trail
TEST(dracTrailTest, drac_teleports_from_castle_double_back_to_hide) {
  // use for hunter testcase
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCD.V.. "  // 0
      "GMN.... SMR.... HMI.... MLS.... DHIT... "  // 1
      "GED.... SGE.... HZU.... MCA.... DC?T... "  // 2
      "GMN.... SGE.... HMI.... MLS.... DC?T... "  // 3
      "GED.... SMR.... HZU.... MCA.... DC?T... "  // 4
      "GMN.... SGE.... HMI.... MLS.... DD4T... "  // 5
      "GED.... SMR.... HZU....";                  // 6

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int(hv_get_round(hv), 6, "round == 6");
  ac_compare_int(hv_get_player(hv), PLAYER_MINA_HARKER, "Player is Mina");
  ac_compare_int(hv_get_score(hv), GAME_START_SCORE - 6, "score is 366 - 6");
  ac_compare_int(hv_get_health(hv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS + 3 * LIFE_GAIN_CASTLE_DRACULA,
                 "Drac HP = 70");

  ac_compare_int(hv_get_location(hv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "location"
                 " CASTLE_DRACULA");

  location_t trail_drac[TRAIL_SIZE];
  hv_get_trail(hv, PLAYER_DRACULA, trail_drac);
  ac_compare_int(trail_drac[0], DOUBLE_BACK_4, "trail[0] correct");
  ac_compare_int(trail_drac[1], CITY_UNKNOWN, "trail[1] correct");
  ac_compare_int(trail_drac[2], CITY_UNKNOWN, "trail[2] correct");
  ac_compare_int(trail_drac[3], CITY_UNKNOWN, "trail[3] correct");
  ac_compare_int(trail_drac[4], HIDE, "trail[4] correct");
  ac_compare_int(trail_drac[5], CASTLE_DRACULA, "trail[5] correct");
  hv_drop(hv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
      "GLO.... SMR.... HCF.... MMA.... DC?T... "
      "GPL.... SMS.... HMR.... MGR.... DC?T...";

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int(hv_get_player(hv), PLAYER_LORD_GODALMING,
                 "hv_get_player(hv) == LordG");
  ac_compare_int(hv_get_round(hv), 4, "round is 4");
  ac_compare_int(hv_get_health(hv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS - 4,
                 "Dr Steward has correct life (5)");
  ac_compare_int(hv_get_health(hv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 10,
                 "hv_get_health(hv, Dracula) == 30 (start - encounter)");
  ac_compare_int(hv_get_score(hv), GAME_START_SCORE - 4,
                 "hv_get_score(hv) == 366 - 4");
  ac_compare_int(hv_get_location(hv, PLAYER_DR_SEWARD), MEDITERRANEAN_SEA,
                 "Dr S is in Med Sea");

  hv_drop(hv);
}

TEST(dracMoveTest, test_get_dests_player_on_drac) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
      "GLO.... SMR.... HCF.... MMA.... DC?T... "
      "GPL.... SMS.... HMR.... MGR.... DC?T...";

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  size_t n_locations = 0;
  location_t *dracDests =
      hv_get_dests_player(hv, &n_locations, PLAYER_DRACULA, true, true, true);
  ac_compare_int(n_locations, 0, "n_locations == 0");
  free(dracDests);
  hv_drop(hv);
}

TEST(hospitalTest, drS_teleports_to_hospital) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
      "GLO.... SMR.... HCF.... MMA.... DC?T... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
      "GEC.... SBATD.. HGO....";

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int(hv_get_player(hv), PLAYER_MINA_HARKER,
                 "hv_get_player(hv) == Mina");
  ac_compare_int(hv_get_round(hv), 4, "round is 4");
  ac_compare_int(hv_get_health(hv, PLAYER_DR_SEWARD), 0,
                 "Dr Steward has correct life (9)");
  ac_compare_int(hv_get_health(hv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS - 20,
                 "hv_get_health(hv, Dracula) == 20 (start - 2*encounter)");
  ac_compare_int(hv_get_score(hv), GAME_START_SCORE - 6 - 4,
                 "hv_get_score(hv) == 366 - 10");

  ac_compare_int(hv_get_location(hv, PLAYER_DR_SEWARD), ST_JOSEPH_AND_ST_MARYS,
                 "DrS is in hospital");
  ac_compare_int(hv_get_health(hv, PLAYER_DR_SEWARD), 0,
                 "Dr Steward has correct life (0)");

  hv_drop(hv);
}

TEST(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DC?T... "
      "GLO.... SMR.... HCF.... MMA.... DC?T... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
      "GEC.... SBATD.. HGO.... MAL.... DS?.... "
      "GLE.... SZA.... HTS.... MMS.... DC?T...";

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int(hv_get_player(hv), PLAYER_LORD_GODALMING,
                 "hv_get_player(hv) == LordG");
  ac_compare_int(hv_get_round(hv), 6, "round is 6");
  ac_compare_int(hv_get_health(hv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int(
      hv_get_health(hv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS - 20 - 2,
      "hv_get_health(hv, Dracula) == 20 (start - 2*encounter - 1*sea)");
  ac_compare_int(hv_get_score(hv), GAME_START_SCORE - 6 - 6,
                 "hv_get_score(hv) == 366 - 12");
  hv_drop(hv);
}

TEST(minionMatTest, test_minion_invalidates) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DC?.V.. "  // 0
      "GMN.... SCF.... HGE.... MLS.... DC?T... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DC?T... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DC?T... "  // 3
      "GEC.... SBA.... HGO.... MAL.... DC?T... "  // 4
      "GLE.... SZA.... HTS.... MMS.... DC?T... "  // 5
      "GPA.... SVI.... HIO.... MCG.... DC?T.V. "  // 6
      "GPA.... SVI.... HIO.... MCG.... DTPT.M.";  // 7
                                                  // 8

  player_message messages[] = {};
  HunterView hv = hv_new(trail, messages);

  ac_compare_int((int)hv_get_player(hv), PLAYER_LORD_GODALMING,
                 "hv_get_player(hv) == Dracula");
  ac_compare_int((int)hv_get_round(hv), 8, "round is 8");

  ac_compare_int((int)hv_get_health(hv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int((int)hv_get_health(hv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS + LIFE_GAIN_CASTLE_DRACULA,
                 "hv_get_health(hv, Dracula) == 50");
  ac_compare_int((int)hv_get_score(hv), GAME_START_SCORE - 8 - 13,
                 "hv_get_score(hv) == 366 - 21");
  hv_drop(hv);
}

// register all tests
// tests will run in the same order they are registered.
static void regAllTests() {
  ac_regTest(basicTest, replicate_dryrun1);
  ac_regTest(connsTest, test_score_start_HP_and_sea_cons);
  ac_regTest(dracTrailTest, drac_teleports_from_castle_double_back_to_hide);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp);
  ac_regTest(dracMoveTest, test_get_dests_player_on_drac);
  ac_regTest(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg);
  ac_regTest(hospitalTest, drS_teleports_to_hospital);
  ac_regTest(minionMatTest, test_minion_invalidates);
}

int main() {
  ac_setLoggingTag("INIT");

  ac_log(AC_LOG_INFO, "Hello world");

  regAllTests();
  ac_runAllTests();

  ac_setLoggingTag("BYE");
  ac_log(AC_LOG_INFO, "Have a nice day!");
  return AC_TEST_RESULT;
}
