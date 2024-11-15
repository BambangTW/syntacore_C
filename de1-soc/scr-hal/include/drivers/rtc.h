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
/// @brief generic RTC defs and funcs
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

#ifndef SCR_BSP_RTC_H
#define SCR_BSP_RTC_H

#include "arch.h"
#include "csr.h"

#ifndef PLF_RTC_TIMEBASE
#define PLF_RTC_TIMEBASE 1000000
#endif

#include <limits.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t sys_tick_t;

#ifdef __cplusplus
}
#endif

#if defined(PLF_MTIMER_BASE)
#include "drivers/mtimer.h"
#elif defined(PLF_CLINT_BASE)
#include "drivers/clint.h"
#else // PLF_MTIMER_BASE
#error PLF_MTIMER_BASE or PLF_CLINT_BASE shall be defined
#endif // PLF_MTIMER_BASE

// RTC constants and conversions
#define PLF_HZ (PLF_RTC_TIMEBASE)

#define PLF_TICKS2S(t)  (t / PLF_HZ)
#define PLF_S2TICKS(s)  (s * PLF_HZ)
#define PLF_TICKS2MS(t) ((t * 1000) / PLF_HZ)
#define PLF_MS2TICKS(m) ((m * PLF_HZ) / 1000)
#define PLF_TICKS2US(t) ((t * 1000000) / PLF_HZ)
#define PLF_US2TICKS(u) ((u * PLF_HZ) / 1000000)
#define PLF_TICKS2NS(t) ((t * 1000000000) / PLF_HZ)
#define PLF_NS2TICKS(n) ((n * PLF_HZ) / 1000000000)

#define PLF_MAX_DELAY_NS PLF_TICKS2NS(ULONG_MAX)
#define PLF_MAX_DELAY_US PLF_TICKS2US(ULONG_MAX)
#define PLF_MAX_DELAY_MS PLF_TICKS2MS(ULONG_MAX)

#ifdef __cplusplus
extern "C" {
#endif

// accessories

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

#if defined(PLF_MTIMER_BASE)
#define rtc_init()  scr_mtimer_init()
#define rtc_now()   scr_mtimer_current()
#define rtc_setcmp(t) scr_mtimer_setcmp(t)
#define rtc_setcmp_offset(t) scr_mtimer_setcmp_offset(t)
#elif defined(PLF_CLINT_BASE)
#define rtc_init()  clint_mtimer_init()
#define rtc_now()   clint_mtimer_current()
#define rtc_setcmp(t) clint_mtimer_setcmp(t)
#define rtc_setcmp_offset(t) clint_mtimer_setcmp_offset(t)
#endif

static inline unsigned long rtc_ticks2ns(sys_tick_t t)
{
    return PLF_TICKS2NS(t);
}

static inline sys_tick_t rtc_ns2ticks(long t)
{
    return PLF_NS2TICKS(t);
}

static inline unsigned long rtc_ticks2us(sys_tick_t t)
{
    return PLF_TICKS2US(t);
}

static inline sys_tick_t rtc_us2ticks(long t)
{
    return PLF_US2TICKS(t);
}

static inline unsigned long rtc_ticks2ms(sys_tick_t t)
{
    return PLF_TICKS2MS(t);
}

static inline sys_tick_t rtc_ms2ticks(long t)
{
    return PLF_MS2TICKS(t);
}

static inline unsigned long rtc_ticks2sec(sys_tick_t t)
{
    return PLF_TICKS2S(t);
}

static inline sys_tick_t rtc_sec2ticks(long t)
{
    return PLF_S2TICKS(t);
}

static inline void rtc_delay_ticks(unsigned long ticks)
{
    sys_tick_t t = rtc_now();

    if (ticks > 100) {
        do {
            cpu_relax();
        } while ((rtc_now() - t) < ticks);
    } else {
        while ((rtc_now() - t) < ticks);
    }
}

static inline void rtc_delay_ns(unsigned long ns)
{
    while (ns >= PLF_MAX_DELAY_NS) {
        rtc_delay_ticks(PLF_NS2TICKS(PLF_MAX_DELAY_NS));
        ns -= PLF_MAX_DELAY_US;
    }

    rtc_delay_ticks(PLF_NS2TICKS(ns));
}

static inline void rtc_delay_us(unsigned long us)
{
    while (us >= PLF_MAX_DELAY_US) {
        rtc_delay_ticks(PLF_US2TICKS(PLF_MAX_DELAY_US));
        us -= PLF_MAX_DELAY_US;
    }

    rtc_delay_ticks(PLF_US2TICKS(us));
}

static inline void rtc_delay_ms(unsigned long ms)
{
    while (ms >= PLF_MAX_DELAY_MS) {
        rtc_delay_ticks(PLF_MS2TICKS(PLF_MAX_DELAY_MS));
        ms -= PLF_MAX_DELAY_MS;
    }

    rtc_delay_ticks(PLF_MS2TICKS(ms));
}

static inline void rtc_interrupt_enable(void)
{
    set_csr(mie, MIE_MTIMER);
}

static inline void rtc_interrupt_disable(void)
{
    clear_csr(mie, MIE_MTIMER);
}

int rtc_time2str(char *buf, size_t len, sys_tick_t t);
unsigned long rtc_dyn_hart_clock_rate(void);

#pragma GCC diagnostic pop

#ifdef __cplusplus
}
#endif
#endif // SCR_BSP_RTC_H
