#!/usr/bin/env bash

make

./run-clang-format.py -r *.c *.h
clang-tidy-4.0 *.c *.h

./test_dracula_view
./test_game_view
./test_hunter_view

