gcc -DI_AM_DRACULA -o dracula_ai player_json.c dracula_view.c game_view.c ac_*.c internal_game_view.c dracula.c places.c map.c mapdata.c rollingarray.c player.c -ljansson
gcc -o hunter_ai player_json.c hunter_view.c game_view.c ac_*.c internal_game_view.c hunter.c places.c map.c mapdata.c rollingarray.c player.c -ljansson
node game_runner/runGame.js ./hunter_ai ./hunter_ai ./hunter_ai ./hunter_ai ./dracula_ai > test_ai.log
