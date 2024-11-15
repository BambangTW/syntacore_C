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
/// @brief Atomic primitives defs and inlines
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

#ifndef SCR_BSP_ATOMIC_H
#define SCR_BSP_ATOMIC_H

#include "arch.h"
#include "drivers/cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct arch_atomic {
    int counter;
} arch_atomic_t;

#define ARCH_ATOMIC_INIT(i) {(i)}

static inline int atomic_read(const arch_atomic_t *v)
{
#if PLF_SMP_NON_COHERENT
    cache_l1_invalidate((void*)&(v->counter), sizeof(v->counter));
#endif // PLF_SMP_NON_COHERENT
    return *((volatile int *)(&(v->counter)));
}

static inline void atomic_set(arch_atomic_t *v, int i)
{
    *((volatile int *)(&(v->counter))) = i;
#if PLF_SMP_NON_COHERENT
    cache_l1_flush((void*)&(v->counter), sizeof(v->counter));
#endif // PLF_SMP_NON_COHERENT
}

#if PLF_ATOMIC_SUPPORTED

static inline int atomic_add(int i, arch_atomic_t *v)
{
    int out;

    __asm__ __volatile__ (
        "amoadd.w %1, %2, %0"
        : "+A" (v->counter), "=r" (out)
        : "r" (i) : "memory");
    return out;
}

static inline int atomic_cas(arch_atomic_t *v, int cmp_val, int new_val)
{
    unsigned int rc;
    int ret;

    __asm__ __volatile__ (
        "0: lr.w %0, %2\n"
        "bne  %0, %3, 1f\n"
        "sc.w %1, %4, %2\n"
        "bnez %1, 0b\n"
        "1:\n"
        : "=&r" (ret), "=&r" (rc), "+A" (v->counter)
        : "r" (cmp_val), "r" (new_val)
        : "memory");

    return ret == cmp_val;
}

#else

static inline int atomic_add(int i, arch_atomic_t *v)
{
    int out;

    out = atomic_read(v);
    atomic_set(v, out + i);

    return out;
}

static int atomic_cas(arch_atomic_t *v, int cmp_val, int new_val)
{
    int out;
    int res;

    out = atomic_read(v);
    res = out == cmp_val;

    if (res)
        atomic_set(v, new_val);

    return res;
}

#endif // PLF_ATOMIC_SUPPORTED

#ifdef __cplusplus
}
#endif
#endif // SCR_BSP_ATOMIC_H
