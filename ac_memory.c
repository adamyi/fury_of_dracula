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

#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ac_config.h"
#include "ac_log.h"
#include "ac_memory.h"

void *ac_malloc(size_t size, const char *name) {
  if (size == 0) {
    ac_log(AC_LOG_DEBUG,
           "Tried to allocate memory of size 0 for %s, returned NULL", name);
    return NULL;
  }
  void *ptr = malloc(size);
  if (ptr == NULL) {
    ac_log(AC_LOG_FATAL, "Failed to allocate memory of size %d at %p for %s",
           size, ptr, name);
    __builtin_unreachable();
  }
  ac_log(AC_LOG_DEBUG, "Allocated memory of size %d at %p for %s", size, ptr,
         name);
  return ptr;
}
