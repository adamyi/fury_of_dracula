########################################################################
# COMP2521 19t0 // The Fury of Dracula // Makefile
#
# Adam Yi <i@adamyi.com>

# CC	 = gcc
CFLAGS	= -std=c99
BINS	 = test_game_view test_hunter_view test_dracula_view generate_map

all: ${BINS}
mapdata: map.c map.h

test_game_view:		test_game_view.o game_view.o map.o places.o
test_game_view.o:	test_game_view.c game_view.h map.h places.h game.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o map.o places.o
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h map.h places.h game.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o map.o places.o
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h map.h places.h game.h

generate_map: generate_map.o places.o

places.o:	places.c places.h game.h
map.o:		map.c map.h places.h game.h
generate_map.o: places.c places.h generate_map.c
game_view.o:	game_view.c game_view.h game.h
hunter_view.o:	hunter_view.c hunter_view.h game.h
dracula_view.o:	dracula_view.c dracula_view.h game.h

map.c: generate_map
	./generate_map

map.h: generate_map
	./generate_map

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core map.*

.PHONY: prove
prove: test_game_view test_hunter_view test_dracula_view
	prove -f ./test_game_view ./test_hunter_view ./test_dracula_view
