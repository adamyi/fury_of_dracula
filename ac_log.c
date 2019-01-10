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

// constants for logging

static const char loggingText[4][6] = {"FATAL", "ERROR", "INFO", "DEBUG"};
static const char loggingColor[4][8] = {"\033[0;31m", "\033[0;31m",
                                        "\033[0;33m", "\033[0;34m"};
static char loggingTag[AC_HELPER_MAX_BUFFER_LENGTH];

void ac_setLoggingTag(const char *tag) {
  strncpy(loggingTag, tag, AC_HELPER_MAX_BUFFER_LENGTH);
}

char *ac_getLoggingTag() { return loggingTag; }

void _ac_log(const char *file, int line, int level, const char *fmt, ...) {
  assert(level >= 0 && level <= 3);

  if (level <= AC_LOG_LOGGING_LEVEL) {
    FILE *out;
    if (level <= AC_LOG_ERROR)
      out = stderr;
    else
      out = stdout;
    char buffer[AC_HELPER_MAX_BUFFER_LENGTH];

    // parse format string
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buffer, AC_HELPER_MAX_BUFFER_LENGTH, fmt, vl);
    va_end(vl);

    time_t ntime;
    time(&ntime);
    struct tm *timeinfo = localtime(&ntime);
    fprintf(out, "%s[%s] - [%s:%d | %s] - %02d:%02d:%02d: %s\033[0m\n",
            loggingColor[level], loggingText[level], file, line, loggingTag,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, buffer);
    if (level == AC_LOG_ERROR && AC_LOG_PAUSE_ON_ERROR) {
      fprintf(stderr, "Paused due to error...\n");
      fprintf(stderr, "[C]ontinue; [e]nd program: ");
      char tmp[20];  // long enough
      fgets(tmp, 20, stdin);
      tmp[19] = '\0';  // fgets does not terminate on error
      if (tmp[0] >= 'A' && tmp[0] <= 'Z') tmp[0] += 'a' - 'A';
      switch (tmp[0]) {
        case 'e':  // end
        case 'q':  // quit
          err(EXIT_FAILURE, "End as requested by user");
      }
    }
  }
  if (level == AC_LOG_FATAL) err(EXIT_FAILURE, "Panic due to a fatal error!");
}
