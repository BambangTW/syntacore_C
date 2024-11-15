/*
 * Copyright (C) 2024, Syntacore Ltd.
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

/// @file time.h
/// @brief Architecture specific time defs
///
/// @copyright Copyright (C) 2024, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef SCR_HAL_TIME_H
#define SCR_HAL_TIME_H

// syscalls.c contain limited implementation of POSIX_TIMERS functions such as
// - clock_gettime
// - clock_getres
// to take nanoseconds capabilities of clock_gettime, need change
// PLF_RTC_TIMEBASE in rtc.h to more than 1'000'000
#define _POSIX_TIMERS
#define _POSIX_MONOTONIC_CLOCK

#include_next <time.h>

#endif // SCR_HAL_TIME_H
