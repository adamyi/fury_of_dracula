#!/usr/bin/env bash

set -e
set -x

make

clang-format --version
./run-clang-format.py *.c *.h

clang-tidy-4.0 --version
clang-tidy-4.0 *.c *.h

./test_dracula_view
./test_game_view
./test_hunter_view

set +e
set +x
