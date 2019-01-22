########################################################################
# COMP2521 19t0 // The Fury of Dracula // Makefile
#
# Adam Yi <i@adamyi.com>

CC	 = ./3c
CFLAGS	= -std=c99 -g -fprofile-arcs -ftest-coverage
LDFLAGS = -lm -ldl -fprofile-arcs -ftest-coverage
BINS	= test_game_view test_hunter_view test_dracula_view generate_mapdata tap_game_view tap_hunter_view tap_dracula_view

all: ${BINS}
mapdata: mapdata.c mapdata.h

tap_game_view:		tap_game_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
tap_game_view.o:	tap_game_view.c game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

tap_hunter_view:	tap_hunter_view.o hunter_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
tap_hunter_view.o:	tap_hunter_view.c hunter_view.h game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

tap_dracula_view:	tap_dracula_view.o dracula_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
tap_dracula_view.o:	tap_dracula_view.c dracula_view.h game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

test_game_view:		test_game_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
test_game_view.o:	test_game_view.c game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o internal_game_view.o map.o mapdata.o places.o player.o rollingarray.o ac_log.o ac_test.o ac_memory.o
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h internal_game_view.h map.h mapdata.h places.h game.h player.h rollingarray.h

generate_mapdata:	generate_mapdata.o places.o ac_log.o

ac_log.o:	ac_log.c ac_log.h ac_config.h ac_constant.h
ac_test.o:	ac_test.c ac_test.h ac_log.h ac_config.h ac_constant.h
ac_memory.o:    ac_memory.c ac_memory.h ac_log.h ac_config.h ac_constant.h

places.o:	places.c places.h game.h
player.o:	player.c places.h game.h rollingarray.h ac_memory.h
map.o:          map.c map.h mapdata.h places.h
mapdata.o:      mapdata.c map.h mapdata.h places.h
generate_mapdata.o: generate_mapdata.c places.h
game_view.o:	game_view.c game_view.h internal_game_view.c internal_game_view.h game.h player.h rollingarray.h ac_log.h ac_config.h ac_constant.h ac_memory.h
internal_game_view.o:	internal_game_view.c internal_game_view.h game.h player.h rollingarray.h ac_log.h ac_config.h ac_constant.h ac_memory.h
hunter_view.o:	hunter_view.c hunter_view.h game_view.h internal_game_view.h game.h ac_memory.h ac_log.h
dracula_view.o:	dracula_view.c dracula_view.h game_view.h internal_game_view.h game.h ac_memory.h ac_log.h
rollingarray.o: rollingarray.c rollingarray.h places.h ac_memory.h

mapdata.c: generate_mapdata
	./generate_mapdata

mapdata.h: generate_mapdata
	./generate_mapdata

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core mapdata.*

.PHONY: prove
prove: tap_game_view tap_hunter_view tap_dracula_view
	prove -f ./tap_game_view ./tap_hunter_view ./tap_dracula_view
