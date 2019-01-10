// This file is part of achelper, Adam's C helper written for UNSW assignments.
// Copyright 2018 Adam Yi <i@adamyi.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <assert.h>
#include <err.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ac_config.h"
#include "ac_log.h"
#include "ac_test.h"

static ac_TestInfo firstTest = NULL;
static ac_TestInfo lastTest = NULL;
static long long totalCompares = 0;
static long long successCompares = 0;

// register a single test
void _ac_regTest(ac_testInfo *test, const char *testCaseName,
                 const char *testName) {
  if (test == NULL) {
    ac_log(AC_LOG_FATAL, "%s - %s does not exist!", testCaseName, testName);
  }
  ac_log(AC_LOG_INFO, "Loading test %s - %s", testCaseName, testName);
  if (firstTest == NULL) {
    firstTest = test;
    lastTest = test;
  } else {
    lastTest->nextTest = test;
    lastTest = test;
  }
}

// run a single test
void ac_runTest(ac_testInfo *test) {
  snprintf(ac_getLoggingTag(), AC_HELPER_MAX_BUFFER_LENGTH, "%s | %s",
           test->testCaseName, test->testName);
  ac_log(AC_LOG_INFO, "Running test %s - %s", test->testCaseName,
         test->testName);
  if (test->testBody != NULL) {
    test->testBody();
  } else {
    test->testBody_f(test->fixtureSetter());
  }
  ac_log(AC_LOG_INFO, " Ending test %s - %s", test->testCaseName,
         test->testName);
}

// run all registered tests
void ac_runAllTests() {
  char oriLogTag[AC_HELPER_MAX_BUFFER_LENGTH];
  strncpy(oriLogTag, ac_getLoggingTag(), AC_HELPER_MAX_BUFFER_LENGTH);

  ac_testInfo *test = firstTest;
  int totalTests = 0, successTests = 0;
  size_t maxNameLength = 0;
  while (test != NULL) {
    long long oriTotalCompares = totalCompares;
    long long oriSuccessCompares = successCompares;
    ac_runTest(test);
    totalTests++;
    oriTotalCompares = totalCompares - oriTotalCompares;
    oriSuccessCompares = successCompares - oriSuccessCompares;
    int len = strlen(test->testCaseName) + strlen(test->testName) + 3;
    if (len > maxNameLength) maxNameLength = len;
    if (oriTotalCompares == oriSuccessCompares) {
      test->success = true;
      successTests++;
      ac_log(AC_LOG_INFO, "Test passed (%d assertions)!", oriTotalCompares);
    } else {
      test->success = false;
      ac_log(AC_LOG_ERROR,
             "Test failed"
             " (only %lld out of %lld assertions passed)!",
             oriSuccessCompares, oriTotalCompares);
    }
    test = test->nextTest;
  }

  ac_setLoggingTag(oriLogTag);
  ac_log(AC_LOG_INFO,
         "All tests ended. %d out of %d tests passed "
         "(%lld out of %lld assertions passed)! See log for details. "
         "Below is a summary table.",
         successTests, totalTests, successCompares, totalCompares);
  ac_printTestsSummary(maxNameLength);
  if (successTests < totalTests && (!AC_LOG_PAUSE_ON_ERROR)) {
    ac_log(AC_LOG_INFO,
           "[HINT] You can set AC_LOG_PAUSE_ON_ERROR to TRUE, "
           "which will pause the program whenever an error/mismatch occurs "
           "in a test so that you can take a closer look~");
  } else {
    ac_log(AC_LOG_INFO,
           "[HINT] If you want, you can pipe the result of this"
           " tester to see it more clearly. E.g. pipe into `more` or `less`,"
           " or just redirect output to a file.");
  }
}

// print a summary table for tests
// TODO(adamyi): write a table helper
void ac_printTestsSummary(int maxL) {
  // top line
  printf(TABLE_TOP_LEFT_CORNER);
  for (int i = -2; i < maxL; i++) printf(TABLE_HORIZONTAL_LINE);
  printf(TABLE_TOP_VERTICAL_POINT);
  for (int i = 0; i < 6; i++) printf(TABLE_HORIZONTAL_LINE);
  printf(TABLE_TOP_RIGHT_CORNER "\n");

  // body part
  ac_testInfo *test = firstTest;
  while (test != NULL) {
    // test line
    int spaces = maxL - strlen(test->testCaseName) - strlen(test->testName) - 1;
    printf(TABLE_VERTICAL_LINE);
    for (int i = 0; i < spaces / 2; i++) printf(" ");
    spaces -= spaces / 2;
    printf("%s > %s", test->testCaseName, test->testName);
    for (int i = 0; i < spaces; i++) printf(" ");
    printf(TABLE_VERTICAL_LINE);
    if (test->success)
      printf(" \x1B[32mPASS\x1B[0m ");
    else
      printf(" \x1B[31mFAIL\x1B[0m ");
    printf(TABLE_VERTICAL_LINE "\n");
    test = test->nextTest;
    // split line
    if (test != NULL) {
      printf(TABLE_LEFT_HORIZONTAL_POINT);
      for (int i = 0; i < maxL + 2; i++) printf(TABLE_HORIZONTAL_LINE);
      printf(TABLE_CROSS);
      for (int i = 0; i < 6; i++) printf(TABLE_HORIZONTAL_LINE);
      printf(TABLE_RIGHT_HORIZONTAL_POINT "\n");
    }
  }

  // bottom line
  printf(TABLE_BOTTOM_LEFT_CORNER);
  for (int i = -2; i < maxL; i++) printf(TABLE_HORIZONTAL_LINE);
  printf(TABLE_BOTTOM_VERTICAL_POINT);
  for (int i = 0; i < 6; i++) printf(TABLE_HORIZONTAL_LINE);
  printf(TABLE_BOTTOM_RIGHT_CORNER "\n");
}

// my own wrapper to compare two ints
void _ac_compare_int(const char *file, int line, int a, int b,
                     const char *msg) {
  totalCompares++;
  if (a == b) {
    successCompares++;
    _ac_log(file, line, AC_LOG_DEBUG, "[DIFF MATCH] %s=%d", msg, a);
  } else
    _ac_log(file, line, AC_LOG_ERROR, "[DIFF MISMATCH] %s=%d,%d", msg, a, b);
}

// my own wrapper to compare two strings
void _ac_compare_string(const char *file, int line, char *a, char *b,
                        const char *msg) {
  totalCompares++;
  if (strcmp(a, b) == 0) {
    successCompares++;
    _ac_log(file, line, AC_LOG_DEBUG, "[DIFF MATCH] %s=%s", msg, a);
  } else
    _ac_log(file, line, AC_LOG_ERROR, "[DIFF MISMATCH] %s=%s,%s", msg, a, b);
}
