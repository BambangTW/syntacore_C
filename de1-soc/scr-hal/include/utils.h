/*
 * Copyright (C) 2015, Syntacore Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file
/// @brief Architecture specific defs and inlines
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2015, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef SCR_BSP_UTILS_H
#define SCR_BSP_UTILS_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define ARRAY_SIZE(X) sizeof((X))/sizeof((X)[0])

#ifdef __llvm__
#define OPT_NONE __attribute__((optnone))
#else
#define OPT_NONE __attribute__((optimize("-O0")))
#endif

#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

extern uint64_t bss_complete_cycles;

static inline __attribute__((always_inline)) uint64_t get_bss_complete_cycles() { return bss_complete_cycles; }

void* OPT_NONE memset_optnone(void *dest, int ch, size_t count);

int hal_get_sysinfo(char* buf, size_t len, bool extended_output);

#endif // SCR_BSP_UTILS_H
