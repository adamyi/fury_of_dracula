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

TEST(encounterTest, drS_encounters_drac_and_vamp) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
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

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, BORDEAUX, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 1, "n_traps == 1");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");

  n_traps = 0;
  n_vamps = 0;
  dv_get_locale_info(dv, CLERMONT_FERRAND, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 0, "n_traps == 0");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");

  dv_drop(dv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
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

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, BARCELONA, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 0, "n_traps == 0");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");
  dv_drop(dv);
}

TEST(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
      "GEC.... SBATD.. HGO.... MAL.... DMS.... "
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
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "  // 0
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DTOT... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DBAT... "  // 3
      "GEC.... SBATD.. HGO.... MAL.... DMS.... "  // 4  -2
      "GLE.... SZA.... HTS.... MMS.... DTS.... "  // 5  -2
      "GLE.... SZA.... HTS.... MMS.... DIO.... "  // 6  -2
      "GLE.... SZA.... HTS.... MMS.... DBS..M. "  // 7  -2
      "GLE.... SZA.... HTS.... MMS.... DCNT.M. "  // 8
      "GLE.... SZA.... HTS.... MMS.... DGAT.M. "  // 8
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
      "GED.... SGE.... HZU.... MCA.... DCD.V.. "  // 0
      "GED.... SGE.... HZU.... MCA.... DHIT... "  // 1
      "GED.... SGE.... HZU.... MCA....";          // 2
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
                 "dv_get_score(dv) == 366 - 2");
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

TEST(teleTest, drac_teleports) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DHA.V.. "  // 0
      "GMN.... SMR.... HMI.... MLS.... DLIT... "  // 1
      "GED.... SGE.... HZU.... MCA.... DBRT... "  // 2
      "GMN.... SGE.... HMI.... MLS.... DPRT... "  // 3
      "GED.... SMR.... HZU.... MCA.... DHIT... "  // 4
      "GMN.... SGE.... HMI.... MLS.... DD3T... "  // 5
      "GED.... SMR.... HZU.... MCA.... DTPTV.. "  // 6
      "GMN.... SGE.... HMI.... MLS....";          // 7

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_location(dv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "location"
                 " CASTLE_DRACULA");

  location_t trail_drac[TRAIL_SIZE];
  dv_get_trail(dv, PLAYER_DRACULA, trail_drac);
  ac_compare_int(trail_drac[0], CASTLE_DRACULA, "trail[0] == TELEPORT");
  ac_compare_int(trail_drac[1], BERLIN, "trail[1] == DOUBLE_BACK_3");
  ac_compare_int(trail_drac[2], PRAGUE, "trail[2] == HIDE");
  ac_compare_int(trail_drac[3], PRAGUE, "trail[3] == PRAUGE");
  ac_compare_int(trail_drac[4], BERLIN, "trail[4] == Berlin");
  ac_compare_int(trail_drac[5], LEIPZIG, "trail[5] == LEIPZIG");

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, BERLIN, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 2, "n_traps == 2");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");
  n_traps = 74;
  n_vamps = 63;
  dv_get_locale_info(dv, PRAGUE, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 2, "n_traps == 2");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");

  dv_drop(dv);
}

TEST(teleTest, drac_double_back_to_hide_in_castle) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCD.V.. "  // 0
      "GMN.... SMR.... HMI.... MLS.... DHIT... "  // 1
      "GED.... SGE.... HZU.... MCA.... DKLT... "  // 2
      "GMN.... SGE.... HMI.... MLS.... DGAT... "  // 3
      "GED.... SMR.... HZU.... MCA.... DCNT... "  // 4
      "GMN.... SGE.... HMI.... MLS.... DD4T... "  // 5
      "GED.... SMR.... HZU.... MCA....";          // 6

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_location(dv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "location"
                 " CASTLE_DRACULA");

  location_t trail_drac[TRAIL_SIZE];
  dv_get_trail(dv, PLAYER_DRACULA, trail_drac);
  ac_compare_int(trail_drac[0], CASTLE_DRACULA, "trail[0] correct");
  ac_compare_int(trail_drac[1], CONSTANTA, "trail[1] correct");
  ac_compare_int(trail_drac[2], GALATZ, "trail[2] correct");
  ac_compare_int(trail_drac[3], KLAUSENBURG, "trail[3] correct");
  ac_compare_int(trail_drac[4], CASTLE_DRACULA, "trail[4] correct");
  ac_compare_int(trail_drac[5], CASTLE_DRACULA, "trail[5] correct");

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, CASTLE_DRACULA, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 2, "n_traps == 2");
  ac_compare_int(n_vamps, 1, "n_vamps == 1");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA), 70, "Drac HP == 70");

  dv_drop(dv);
}

TEST(teleTest, drac_double_back_to_hide_in_castle_then_TP) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCD.V.. "  // 0
      "GMN.... SMR.... HMI.... MLS.... DHIT... "  // 1
      "GED.... SGE.... HZU.... MCA.... DKLT... "  // 2
      "GMN.... SGE.... HMI.... MLS.... DGAT... "  // 3
      "GED.... SMR.... HZU.... MCA.... DCNT... "  // 4
      "GMN.... SGE.... HMI.... MLS.... DD4T... "  // 5
      "GED.... SMR.... HZU.... MCA.... DTPTV..";  // 6

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_location(dv, PLAYER_DRACULA), CASTLE_DRACULA,
                 "location"
                 " CASTLE_DRACULA");

  location_t trail_drac[TRAIL_SIZE];
  dv_get_trail(dv, PLAYER_DRACULA, trail_drac);
  ac_compare_int(trail_drac[0], CASTLE_DRACULA, "trail[0] correct");
  ac_compare_int(trail_drac[2], CONSTANTA, "trail[1] correct");
  ac_compare_int(trail_drac[3], GALATZ, "trail[2] correct");
  ac_compare_int(trail_drac[4], KLAUSENBURG, "trail[3] correct");
  ac_compare_int(trail_drac[5], CASTLE_DRACULA, "trail[4] correct");
  ac_compare_int(trail_drac[1], CASTLE_DRACULA, "trail[5] correct");

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, CASTLE_DRACULA, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 3, "n_traps == 3");
  ac_compare_int(n_vamps, 1, "n_vamps == 0");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA), 80, "Drac HP == 80");

  dv_drop(dv);
}

TEST(dracMoveTest, test_drac_cannot_move_to_hosp_or_trail) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DZA.V.. "
      "GED.... SGE.... HZU.... MCA.... DSJT... "
      "GED.... SGE.... HZU.... MCA....";

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  bool move_key[NUM_MAP_LOCATIONS];
  memset(move_key, false, NUM_MAP_LOCATIONS);
  move_key[SARAJEVO] = true;
  move_key[VALONA] = true;
  move_key[SOFIA] = true;
  move_key[BELGRADE] = true;

  bool move_ret[NUM_MAP_LOCATIONS];
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  size_t n_locations = 0;
  location_t *dests = dv_get_dests(dv, &n_locations, true, true);
  for (size_t i = 0; i < n_locations; i++) {
    move_ret[dests[i]] = true;
  }
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    ac_compare_int(move_ret[i], move_key[i], "location match");
  }
  free(dests);
  dv_drop(dv);
}

TEST(vamp_mat_test, test_vamp_maturity_consequences) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DGA.V.. "  // 0
      "GMN.... SCF.... HGE.... MLS.... DKLT... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DBDT... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DVIT... "  // 3
      "GEC.... SBA.... HGO.... MAL.... DPRT... "  // 4
      "GLE.... SZA.... HTS.... MMS.... DBRT... "  // 5
      "GPA.... SVI.... HIO.... MCG.... DHAT.V. "  // 6
      "GPA.... SVI.... HIO.... MCG.... DCOT.M. "  // 7
      "GPA.... SVI.... HIO.... MCG....";          // 8

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  ac_compare_int(dv_get_round(dv), 8, "round is 8");
  ac_compare_int(dv_get_health(dv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int(dv_get_health(dv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS,
                 "dv_get_health(dv, Dracula) == 40");
  ac_compare_int(dv_get_score(dv), GAME_START_SCORE - 8 - 13,
                 "dv_get_score(dv) == 366 - 21");

  int n_traps = 0;
  int n_vamps = 0;
  dv_get_locale_info(dv, GALATZ, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 0, "n_traps == 0");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");

  n_traps = 0;
  n_vamps = 0;
  dv_get_locale_info(dv, KLAUSENBURG, &n_traps, &n_vamps);
  ac_compare_int(n_traps, 0, "n_traps == 0");
  ac_compare_int(n_vamps, 0, "n_vamps == 0");

  dv_drop(dv);
}

TEST(conn_test, test_connections_from_start_locs) {
  char *trail = "GED.... SGE.... HZU.... MCA....";  // 0

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  bool move_ret[NUM_MAP_LOCATIONS];
  memset(move_ret, false, NUM_MAP_LOCATIONS);

  size_t n_locations = 0;

  location_t *destsVHall = dv_get_dests_player(
      dv, &n_locations, PLAYER_VAN_HELSING, true, true, true);
  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsVHall[i]] = true;
  }
  ac_compare_int(n_locations, 9, "9 == n_locations");
  ac_compare_int(move_ret[24], true, "Florence");
  ac_compare_int(move_ret[25], true, "Frankfurt");
  ac_compare_int(move_ret[28], true, "Geneva");
  ac_compare_int(move_ret[29], true, "Genoa");
  ac_compare_int(move_ret[42], true, "Marseilles");
  ac_compare_int(move_ret[44], true, "Milan");
  ac_compare_int(move_ret[45], true, "Munich");
  ac_compare_int(move_ret[60], true, "Strasbourg");
  ac_compare_int(move_ret[70], true, "Zurich");
  free(destsVHall);
  dv_drop(dv);
}

TEST(hunterTrail, get_hunters_trail) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DGA.V.. "  // 0
      "GMN.... SCF.... HGE.... MLS.... DKLT... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DBDT... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DVIT... "  // 3
      "GEC.... SBA.... HGO.... MAL.... DPRT... "  // 4
      "GLE.... SZA.... HTS.... MMS.... DBRT... "  // 5
      "GPA.... SVI.... HIO.... MCG.... DHAT.V. "  // 6
      "GPA.... SVI.... HIO.... MCG.... DCOT.M. "  // 7
      "GPA.... SVI.... HIO.... MCG....";          // 8

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  location_t trailLG[TRAIL_SIZE];

  dv_get_trail(dv, PLAYER_LORD_GODALMING, trailLG);
  ac_compare_int(trailLG[0], PARIS, "PA");
  ac_compare_int(trailLG[1], PARIS, "PA");
  ac_compare_int(trailLG[2], PARIS, "PA");
  ac_compare_int(trailLG[3], LE_HAVRE, "LE");
  ac_compare_int(trailLG[4], ENGLISH_CHANNEL, "EC");
  ac_compare_int(trailLG[5], PLYMOUTH, "PL");

  dv_drop(dv);
}

TEST(dracMoveTest, test_drac_sea_move_after_D2) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCN.V.. "
      "GED.... SGE.... HZU.... MCA.... DGAT... "
      "GED.... SGE.... HZU.... MCA.... DD2T... "
      "GED.... SGE.... HZU.... MCA....";

  player_message messages[] = {};
  DraculaView dv = dv_new(trail, messages);

  bool move_ret[NUM_MAP_LOCATIONS];
  memset(move_ret, false, NUM_MAP_LOCATIONS);

  size_t n_locations = 0;

  location_t *dests = dv_get_dests(dv, &n_locations, true, true);
  for (size_t i = 0; i < n_locations; i++) {
    move_ret[dests[i]] = true;
  }
  ac_compare_int(n_locations, 4, "4 == n_locations");
  ac_compare_int(move_ret[CONSTANTA], true, "CONSTANTA");
  ac_compare_int(move_ret[BUCHAREST], true, "BUCHAREST");
  ac_compare_int(move_ret[BLACK_SEA], true, "BLACK_SEA");
  ac_compare_int(move_ret[VARNA], true, "VARNA");
  free(dests);

  dv_drop(dv);
}
// register all tests
// tests will run in the same order they are registered.
static void regAllTests() {
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap);
  ac_regTest(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg);
  ac_regTest(castleDracTest, drac_heals_in_castle);
  ac_regTest(castleDracTest, drac_heals_check_traps_check_locs);
  ac_regTest(teleTest, drac_teleports);
  ac_regTest(teleTest, drac_double_back_to_hide_in_castle);
  ac_regTest(dracMoveTest, test_drac_cannot_move_to_hosp_or_trail);
  ac_regTest(teleTest, drac_double_back_to_hide_in_castle_then_TP);
  ac_regTest(vamp_mat_test, test_vamp_maturity_consequences);
  ac_regTest(conn_test, test_connections_from_start_locs);
  ac_regTest(hunterTrail, get_hunters_trail);
  ac_regTest(dracMoveTest, test_drac_sea_move_after_D2);
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
