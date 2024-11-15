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
/// @brief Syntacore MTimer
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

#ifndef SCR_BSP_MTIMER_H
#define SCR_BSP_MTIMER_H

#include "arch.h"

#ifndef PLF_MTIMER_BASE
#error Unknown PLF_MTIMER_BASE
#endif

// MTimer control register bits
#define SCR_MTIMER_CTRL_EN (1 << 0)
#define SCR_MTIMER_CTRL_SI (0 << 1)
#define SCR_MTIMER_CTRL_SE (1 << 1)

#if PLF_RTC_SRC_EXTERNAL
#define SCR_PLF_RTC_SRC SCR_MTIMER_CTRL_SE
#define SCR_PLF_TIMEBASE_DIV (((PLF_SYS_CLK) / 4) / (PLF_RTC_TIMEBASE) - 1)
#else
#define SCR_PLF_RTC_SRC SCR_MTIMER_CTRL_SI
#define SCR_PLF_TIMEBASE_DIV ((PLF_SYS_CLK) / (PLF_RTC_TIMEBASE) - 1)
#endif // PLF_RTC_SRC_EXTERNAL

#define SCR_RTC_CTL_OFF   0
#define SCR_RTC_DIV_OFF   4
#define SCR_RTC_TIME_OFF  8
#define SCR_RTC_TIMEH_OFF 12
#define SCR_RTC_CMP_OFF   16
#define SCR_RTC_CMPH_OFF  20

#define SCR_RTC_CTL       (PLF_MTIMER_BASE+SCR_RTC_CTL_OFF)
#define SCR_RTC_DIVIDER   (PLF_MTIMER_BASE+SCR_RTC_DIV_OFF)
#define SCR_RTC_MTIME     (PLF_MTIMER_BASE+SCR_RTC_TIME_OFF)
#define SCR_RTC_MTIMEH    (PLF_MTIMER_BASE+SCR_RTC_TIMEH_OFF)
#define SCR_RTC_MTIMECMP  (PLF_MTIMER_BASE+SCR_RTC_CMP_OFF)
#define SCR_RTC_MTIMECMPH (PLF_MTIMER_BASE+SCR_RTC_CMPH_OFF)

#ifdef PLF_MTIMER_EXT
#define SCR_RTC_MTIMECMP_OFFS         (0x1000)
#define SCR_RTC_MTIMECMP_CORE(x)      (PLF_MTIMER_BASE + SCR_RTC_MTIMECMP_OFFS + ((x) << 3))
#define SCR_RTC_MTIMECMPH_CORE(x)     (PLF_MTIMER_BASE + SCR_RTC_MTIMECMP_OFFS + ((x) << 3) + 4)
#endif //PLF_MTIMER_EXT

#define SCR_RTC_CTL_EN (1 << 0)
#define SCR_RTC_CTL_INTERNAL_SRC (0 << 1)
#define SCR_RTC_CTL_EXTERNAL_SRC (1 << 1)

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct scr_mtimer_struct {
    uint32_t ctrl;
    uint32_t div;
#if __riscv_xlen == 32
    uint32_t time;
    uint32_t timeh;
    uint32_t cmp;
    uint32_t cmph;
#else // __riscv_xlen  == 32
    uint64_t time;
    uint64_t cmp;
#endif // __riscv_xlen == 32
} volatile scr_mtimer;

#define SCR_MTIMER (*(scr_mtimer*)(PLF_MTIMER_BASE))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

static inline sys_tick_t scr_mtimer_current(void)
{
#if __riscv_xlen == 32
    return read_csr64(time);
#else //  __riscv_xlen == 32
    return read_csr(time);
#endif // __riscv_xlen == 32
}

// basic functions

static inline void scr_mtimer_setcmp(sys_tick_t t)
{
#if __riscv_xlen == 32
    SCR_MTIMER.cmph = ~0;
    SCR_MTIMER.cmp  = (uint32_t)t;
    SCR_MTIMER.cmph = (uint32_t)(t >> 32);
#else //  __riscv_xlen == 32
    SCR_MTIMER.cmp  = t;
#endif //  __riscv_xlen == 32
}

static inline void scr_mtimer_setcmp_offset(sys_tick_t to)
{
    scr_mtimer_setcmp(scr_mtimer_current() + to);
}

static inline void scr_mtimer_set(sys_tick_t t)
{
#if __riscv_xlen == 32
    SCR_MTIMER.timeh = 0;
    SCR_MTIMER.time  = (uint32_t)t;
    SCR_MTIMER.timeh = (uint32_t)(t >> 32);
#else //  __riscv_xlen == 32
    SCR_MTIMER.time  = t;
#endif //  __riscv_xlen == 32
}

static inline void scr_mtimer_reset(void)
{
    // reset counter and cmp, set divisor
    SCR_MTIMER.ctrl  = 0;
    SCR_MTIMER.div   = SCR_PLF_TIMEBASE_DIV;
    SCR_MTIMER.time  = 0;
#if __riscv_xlen == 32
    SCR_MTIMER.timeh = 0;
#endif // __riscv_xlen == 32
    SCR_MTIMER.cmp   = ~0;
#if __riscv_xlen == 32
    SCR_MTIMER.cmph  = ~0;
#endif // __riscv_xlen == 32
}

static inline void scr_mtimer_enable(void)
{
    SCR_MTIMER.ctrl = SCR_MTIMER_CTRL_EN | SCR_PLF_RTC_SRC;
}

static inline void scr_mtimer_disable(void)
{
    SCR_MTIMER.ctrl = 0;
}

static inline void scr_mtimer_init(void)
{
    scr_mtimer_reset();
    scr_mtimer_enable();
}

#pragma GCC diagnostic pop

#ifdef __cplusplus
}
#endif

#endif // SCR_BSP_MTIMER_H
