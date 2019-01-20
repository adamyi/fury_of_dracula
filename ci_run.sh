#!/usr/bin/env bash
# CI script for the Fury of Dracula
# written by Adam Yi <i@adamyi.com>

CLANG_FORMAT="clang-format-7"
if [ -z "$(which $CLANG_FORMAT)" ]; then
  CLANG_FORMAT="clang-format"
fi

echo "Using clang-format: $CLANG_FORMAT"

CLANG_TIDY="clang-tidy-7"
if [ -z "$(which $CLANG_TIDY)" ]; then
  CLANG_TIDY="clang-tidy"
fi

echo "Using clang-tidy: $CLANG_TIDY"

SRCS="$(find . -maxdepth 1 -name \*.c -o -name \*.h |grep -v -F -f .lint-ignore | tr '\n' ' ')"
echo "Linting files: $SRCS"

set -e
set -x

make clean
make

$CLANG_FORMAT --version
./run-clang-format.py --clang-format-executable $CLANG_FORMAT $SRCS

$CLANG_TIDY --version
$CLANG_TIDY $SRCS

echo "Run provided tap test (dryrun)"

make prove

echo "Run unit tests"

./test_dracula_view
./test_game_view
./test_hunter_view

set +e
set +x
