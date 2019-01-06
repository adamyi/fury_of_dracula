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

set -e
set -x

make

$CLANG_FORMAT --version
./run-clang-format.py --clang-format-executable $CLANG_FORMAT *.c *.h

$CLANG_TIDY --version
$CLANG_TIDY *.c *.h

./test_dracula_view
./test_game_view
./test_hunter_view

set +e
set +x
