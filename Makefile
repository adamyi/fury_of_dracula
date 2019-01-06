########################################################################
# COMP2521 19t0 // The Fury of Dracula // the View
# view/Makefile: build tests for {Game,Hunter,Dracula}View
#
# 2018-12-31	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>

CC	 = gcc
CFLAGS	= -std=c99
BINS	 = test_game_view test_hunter_view test_dracula_view

all: ${BINS}

test_game_view:		test_game_view.o game_view.o map.o places.o
test_game_view.o:	test_game_view.c game_view.h map.h places.h game.h

test_hunter_view:	test_hunter_view.o hunter_view.o game_view.o map.o places.o
test_hunter_view.o:	test_hunter_view.c hunter_view.h game_view.h map.h places.h game.h

test_dracula_view:	test_dracula_view.o dracula_view.o game_view.o map.o places.o
test_dracula_view.o:	test_dracula_view.c dracula_view.h game_view.h map.h places.h game.h

places.o:	places.c places.h game.h
map.o:		map.c map.h places.h game.h
game_view.o:	game_view.c game_view.h game.h
hunter_view.o:	hunter_view.c hunter_view.h game.h
dracula_view.o:	dracula_view.c dracula_view.h game.h

.PHONY: clean
clean:
	-rm -f ${BINS} *.o core

.PHONY: prove
prove: test_game_view test_hunter_view test_dracula_view
	prove -f ./test_game_view ./test_hunter_view ./test_dracula_view
