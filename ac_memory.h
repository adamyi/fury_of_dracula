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

#include <stddef.h>

#include "ac_config.h"
#include "ac_constant.h"

#ifndef AC_HELPER_ACMEMORY_H_
#define AC_HELPER_ACMEMORY_H_

void *ac_malloc(size_t size, const char *name);

#endif  // AC_HELPER_ACMEMORY_H_
