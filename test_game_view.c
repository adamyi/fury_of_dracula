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

TEST(basicTest, replicate_dryrun1) {
  char *trail = "";
  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_player(gv), PLAYER_LORD_GODALMING,
                 "gv_get_player(gv) == Godalming");
  ac_compare_int(gv_get_round(gv), 0, "round is 0");
  ac_compare_int(gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct starting life");
  ac_compare_int(gv_get_health(gv, PLAYER_DRACULA), GAME_START_BLOOD_POINTS,
                 "gv_get_health(gv, Dracula) == 40 (start)");
  ac_compare_int(gv_get_score(gv), GAME_START_SCORE, "gv_get_score(gv) == 366");
  ac_compare_int(gv_get_location(gv, PLAYER_LORD_GODALMING), UNKNOWN_LOCATION,
                 "Godalming is in correct start location");

  gv_drop(gv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
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
  ac_compare_int(gv_get_location(gv, PLAYER_DR_SEWARD), MEDITERRANEAN_SEA,
                 "Dr S is in Med Sea");
  gv_drop(gv);
}

TEST(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
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
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "
      "GLO.... SMR.... HCF.... MMA.... DTOT... "
      "GPL.... SMS.... HMR.... MGR.... DBAT... "
      "GEC.... SBATD.. HGO.... MAL.... DMS.... "
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
      "GED.... SGE.... HZU.... MCA.... DCF.V.. "  // 0
      "GMN.... SCFVD.. HGE.... MLS.... DBOT... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DTOT... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DBAT... "  // 3
      "GEC.... SBATD.. HGO.... MAL.... DMS.... "  // 4  -2
      "GLE.... SZA.... HTS.... MMS.... DTS.... "  // 5  -2
      "GLE.... SZA.... HTS.... MMS.... DIO.... "  // 6  -2
      "GLE.... SZA.... HTS.... MMS.... DBS..M. "  // 7  -2
      "GLE.... SZA.... HTS.... MMS.... DCNT.M. "  // 8
      "GLE.... SZA.... HTS.... MMS.... DGAT... "  // 8
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
      "GED.... SGE.... HZU.... MCA.... DCD.V.. "  // 0
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

TEST(vamp_mat_test, test_vamp_maturity_consequences) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DC?.V.. "  // 0
      "GMN.... SCF.... HGE.... MLS.... DC?T... "  // 1
      "GLO.... SMR.... HCF.... MMA.... DC?T... "  // 2
      "GPL.... SMS.... HMR.... MGR.... DC?T... "  // 3
      "GEC.... SBA.... HGO.... MAL.... DC?T... "  // 4
      "GLE.... SZA.... HTS.... MMS.... DC?T... "  // 5
      "GPA.... SVI.... HIO.... MCG.... DC?T.V.";  // 6
                                                  // 7

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  ac_compare_int((int)gv_get_player(gv), PLAYER_LORD_GODALMING,
                 "gv_get_player(gv) == Dracula");
  ac_compare_int((int)gv_get_round(gv), 7, "round is 7");

  ac_compare_int((int)gv_get_health(gv, PLAYER_DR_SEWARD),
                 GAME_START_HUNTER_LIFE_POINTS,
                 "Dr Steward has correct life (9)");
  ac_compare_int((int)gv_get_health(gv, PLAYER_DRACULA),
                 GAME_START_BLOOD_POINTS, "gv_get_health(gv, Dracula) == 40");
  ac_compare_int((int)gv_get_score(gv), GAME_START_SCORE - 7 - 13,
                 "gv_get_score(gv) == 366 - 20");
  gv_drop(gv);
}

TEST(conn_test, test_connections_from_start_locs) {
  char *trail = "GED.... SGE.... HZU.... MCA....";  // 0

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  bool move_ret[NUM_MAP_LOCATIONS];
  memset(move_ret, false, NUM_MAP_LOCATIONS);

  size_t n_locations = 0;
  location_t *destsLG = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING),
      PLAYER_LORD_GODALMING, gv_get_round(gv), true, false, false);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsLG[i]] = true;
  }
  ac_compare_int(n_locations, 2, "n_locations == 2");
  ac_compare_int(move_ret[EDINBURGH], true, "current loc Ed");
  ac_compare_int(move_ret[MANCHESTER], true, "Man avaialbe by road");
  free(destsLG);

  n_locations = 0;
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  location_t *destsLGrail = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING),
      PLAYER_LORD_GODALMING, gv_get_round(gv), false, true, false);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsLGrail[i]] = true;
  }
  ac_compare_int(n_locations, 1, "n_locations == 1");
  ac_compare_int(move_ret[EDINBURGH], true, "current loc Ed");
  free(destsLGrail);

  n_locations = 0;
  location_t *destsLGsea = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING),
      PLAYER_LORD_GODALMING, gv_get_round(gv), false, false, true);

  memset(move_ret, false, NUM_MAP_LOCATIONS);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsLGsea[i]] = true;
  }
  ac_compare_int(n_locations, 2, "n_locations == 2");

  ac_compare_int(move_ret[EDINBURGH], true, "current loc Ed");
  ac_compare_int(move_ret[NORTH_SEA], true, "North Sea should be accessable");
  free(destsLGsea);

  n_locations = 0;
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  location_t *destsLGall = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_LORD_GODALMING),
      PLAYER_LORD_GODALMING, gv_get_round(gv), true, true, true);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsLGall[i]] = true;
  }
  ac_compare_int(n_locations, 3, "n_locations == 3");
  free(destsLGall);

  ac_compare_int(move_ret[EDINBURGH], true, "current loc Ed");
  ac_compare_int(move_ret[MANCHESTER], true, "man is adjacent");
  ac_compare_int(move_ret[NORTH_SEA], true, "NS is adjacent");

  n_locations = 0;
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  location_t *destsVHroad = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_VAN_HELSING),
      PLAYER_VAN_HELSING, gv_get_round(gv), true, false, true);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsVHroad[i]] = true;
  }
  ac_compare_int(n_locations, 6, "6 == n_locations");
  ac_compare_int(move_ret[GENEVA], true, "Geneva");
  ac_compare_int(move_ret[MARSEILLES], true, "MARSEILLES");
  ac_compare_int(move_ret[MILAN], true, "MILAN");
  ac_compare_int(move_ret[MUNICH], true, "MUNICH");
  ac_compare_int(move_ret[STRASBOURG], true, "STRASBOURG");
  ac_compare_int(move_ret[ZURICH], true, "ZURICH");
  free(destsVHroad);

  n_locations = 0;
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  location_t *destsVHrail = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_VAN_HELSING),
      PLAYER_VAN_HELSING, gv_get_round(gv), false, true, true);
  for (size_t i = 0; i < n_locations; i++) {
    move_ret[destsVHrail[i]] = true;
  }

  ac_compare_int(n_locations, 7, "7 == n_locations");
  ac_compare_int(move_ret[24], true, "Florence");
  ac_compare_int(move_ret[25], true, "Frankfurt");
  ac_compare_int(move_ret[28], true, "Geneva");
  ac_compare_int(move_ret[29], true, "Genoa");
  ac_compare_int(move_ret[44], true, "Milan");
  ac_compare_int(move_ret[60], true, "Strasbourg");
  ac_compare_int(move_ret[70], true, "Zurich");
  free(destsVHrail);

  n_locations = 0;
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  location_t *destsVHall = gv_get_connections(
      gv, &n_locations, gv_get_location(gv, PLAYER_VAN_HELSING),
      PLAYER_VAN_HELSING, gv_get_round(gv), true, true, true);
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
  gv_drop(gv);
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
  GameView gv = gv_new(trail, messages);

  ac_compare_int(gv_get_location(gv, PLAYER_DRACULA), TELEPORT,
                 "location TELEPORT");

  location_t trail_drac[TRAIL_SIZE];
  gv_get_history(gv, PLAYER_DRACULA, trail_drac);
  ac_compare_int(trail_drac[0], TELEPORT, "trail[0] == TELEPORT");
  ac_compare_int(trail_drac[1], 105, "trail[1] == DOUBLE_BACK_3");
  ac_compare_int(trail_drac[2], 102, "trail[2] == HIDE");
  ac_compare_int(trail_drac[3], PRAGUE, "trail[3] == PRAUGE");
  ac_compare_int(trail_drac[4], BERLIN, "trail[4] == Berlin");
  ac_compare_int(trail_drac[5], LEIPZIG, "trail[5] == LEIPZIG");

  ac_compare_int(gv_get_location(gv, PLAYER_DRACULA), TELEPORT,
                 "current loc is"
                 " TELEPORT");

  gv_drop(gv);
}
TEST(dracConnTest, test_get_conns) {
  char *trail =
      "GED.... SGE.... HZU.... MCA.... DVA.V.. "
      "GED.... SGE.... HZU.... MCA.... DSJT... "
      "GED.... SGE.... HZU.... MCA.... DD2T... "
      "GED.... SGE.... HZU.... MCA....";

  player_message messages[] = {};
  GameView gv = gv_new(trail, messages);

  bool move_ret[NUM_MAP_LOCATIONS];
  memset(move_ret, false, NUM_MAP_LOCATIONS);
  size_t n_locations = 0;
  location_t *dests =
      gv_get_connections(gv, &n_locations, VALONA, PLAYER_DRACULA,
                         gv_get_round(gv), true, true, true);

  for (size_t i = 0; i < n_locations; i++) {
    move_ret[dests[i]] = true;
  }
  ac_compare_int(n_locations, 6, "n_locations == 6");
  ac_compare_int(move_ret[IONIAN_SEA], true, "IO included");
  ac_compare_int(move_ret[VALONA], true, "VA included");
  ac_compare_int(move_ret[SALONICA], true, "SA included");
  ac_compare_int(move_ret[SOFIA], true, "SO included");
  ac_compare_int(move_ret[ATHENS], true, "AT included");
  ac_compare_int(move_ret[SARAJEVO], true, "SJ included");

  free(dests);
  gv_drop(gv);
}

// register all tests
// tests will run in the same order they are registered.
static void regAllTests() {
  ac_regTest(basicTest, replicate_dryrun1);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp);
  ac_regTest(encounterTest, drS_encounters_drac_and_vamp_then_drac_and_trap);
  ac_regTest(hospitalTest, drS_teleports_to_hospital_drac_sea_dmg);
  ac_regTest(castleDracTest, drac_heals_in_castle);
  ac_regTest(castleDracTest, drac_heals_in_castle_to_max);
  ac_regTest(vamp_mat_test, test_vamp_maturity_consequences);
  ac_regTest(conn_test, test_connections_from_start_locs);
  ac_regTest(teleTest, drac_teleports);
  ac_regTest(dracConnTest, test_get_conns);
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
