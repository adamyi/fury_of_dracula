########################################################################
# COMP2521 19t0 // The Fury of Dracula // Makefile
#
# Adam Yi <i@adamyi.com>

# CC	 = gcc
CFLAGS	= -std=c99
BINS	 = test_game_view test_hunter_view test_dracula_view generate_mapdata

all: ${BINS}
mapdata: mapdata.c mapdata.h

test_game_view:		test_game_view.o game_view.o map.o mapdata.o places.o
test_game_view.o:	test_game_view.c game_view.h map.h mapdata.h places.h game.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o map.o mapdata.o places.o
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h map.h mapdata.h places.h game.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o map.o mapdata.o places.o
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h map.h mapdata.h places.h game.h

generate_mapdata: generate_mapdata.o places.o

places.o:	places.c places.h game.h
map.o:          map.c map.h mapdata.c mapdata.h places.h
mapdata.o:	mapdata.c mapdata.h places.h
generate_mapdata.o: places.c places.h generate_mapdata.c
game_view.o:	game_view.c game_view.h game.h
hunter_view.o:	hunter_view.c hunter_view.h game.h
dracula_view.o:	dracula_view.c dracula_view.h game.h

mapdata.c: generate_mapdata
	./generate_mapdata

mapdata.h: generate_mapdata
	./generate_mapdata

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core mapdata.*

.PHONY: prove
prove: test_game_view test_hunter_view test_dracula_view
	prove -f ./test_game_view ./test_hunter_view ./test_dracula_view
