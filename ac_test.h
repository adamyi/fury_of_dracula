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

#include <stdbool.h>

#include "ac_config.h"
#include "ac_constant.h"

#ifndef AC_HELPER_ACTEST_H_
#define AC_HELPER_ACTEST_H_

#define AC_TEST_RESULT _ac_test_retval()

#define ac_compare_int(values...) _ac_compare_int(__FILE__, __LINE__, values)
#define ac_compare_string(values...) \
  _ac_compare_string(__FILE__, __LINE__, values)

#define TEST_CASE_NAME_(test_case_name, test_name) \
  test_case_name##_##test_name##_test

#define TEST_BODY_NAME_(test_case_name, test_name) \
  test_case_name##_##test_name##_body

#define TEST_FIXSET_NAME_(test_case_name) test_case_name##_fixture

#define ac_regTest(test_case_name, test_name)                               \
  _ac_regTest(&TEST_CASE_NAME_(test_case_name, test_name), #test_case_name, \
              #test_name)

#define TEST(test_case_name, test_name)                             \
  static void TEST_BODY_NAME_(test_case_name, test_name)(void);     \
  static ac_testInfo TEST_CASE_NAME_(test_case_name, test_name) = { \
      .testCaseName = #test_case_name,                              \
      .testName = #test_name,                                       \
      .testBody = &TEST_BODY_NAME_(test_case_name, test_name)};     \
  static void TEST_BODY_NAME_(test_case_name, test_name)(void)

#define TEST_F(test_case_name, test_name)                                \
  static void TEST_BODY_NAME_(test_case_name, test_name)(void *fixture); \
  static void *TEST_FIXSET_NAME_(test_case_name)(void);                  \
  static ac_testInfo TEST_CASE_NAME_(test_case_name, test_name) = {      \
      .testCaseName = #test_case_name,                                   \
      .testName = #test_name,                                            \
      .fixtureSetter = &TEST_FIXSET_NAME_(test_case_name),               \
      .testBody_f = &TEST_BODY_NAME_(test_case_name, test_name)};        \
  static void TEST_BODY_NAME_(test_case_name, test_name)(void *fixture)

#define TEST_SET_FIXTURE(test_case_name) \
  static void *TEST_FIXSET_NAME_(test_case_name)(void)

typedef struct ac_testInfo *ac_TestInfo;

typedef struct ac_testInfo {
  const char *testCaseName;
  const char *testName;
  void *(*fixtureSetter)(void);
  void (*testBody_f)(void *fixture);
  void (*testBody)(void);
  ac_TestInfo nextTest;
  bool success;
} ac_testInfo;

void _ac_regTest(ac_testInfo *test, const char *testCaseName,
                 const char *testName);
void ac_runTest(ac_testInfo *test);
void ac_runAllTests(void);
void ac_printTestsSummary(int maxL);
void _ac_compare_int(const char *file, int line, int a, int b, const char *msg);
void _ac_compare_string(const char *file, int line, char *a, char *b,
                        const char *msg);

int _ac_test_retval();

#endif  // AC_HELPER_ACTEST_H_
