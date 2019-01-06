#!/usr/bin/env bash

make

clang-tidy *.c

./test_dracula_view
./test_game_view
./test_hunter_view

