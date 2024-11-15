/*
 * Copyright (C) 2020, Syntacore Ltd.
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
/// @brief lock defs and inlines
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2020, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef SCR_BSP_LOCK_H
#define SCR_BSP_LOCK_H

#include "arch.h"

#ifdef PLF_SMP_SUPPORT

#include "drivers/cache.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#if PLF_ATOMIC_SUPPORTED

typedef struct arch_lock {
#ifdef PLF_ARCH_TICKET_SPINLOCKS
    /*
    * Ticket spinlocks are conceptually two atomic variables,
    * one indicating the current FIFO head (spinlock owner),
    * and the other indicating the current FIFO tail.
    * Spinlock is acquired in the following manner:
    * - current FIFO tail value is atomically incremented while it's
    *   original value is saved as a "ticket"
    * - we spin until the FIFO head becomes equal to the ticket value
    *
    * Spinlock is released by atomic increment of the FIFO head
    */
    arch_atomic_t owner;
    arch_atomic_t tail;
#else
    volatile int lock;
#endif
} arch_lock_t;

static inline int arch_is_locked(arch_lock_t *lock)
{
#ifdef PLF_ARCH_TICKET_SPINLOCKS
    int val = atomic_read(&lock->owner);
    return !atomic_cas(&lock->tail, val, val);
#else
#if PLF_SMP_NON_COHERENT
    cache_l1_invalidate((void*)&(lock->lock), sizeof(lock->lock));
#endif // PLF_SMP_NON_COHERENT
    return lock->lock != 0;
#endif
}

static inline void arch_unlock_wait(arch_lock_t *lock)
{
    do {
        cpu_relax();
    } while (arch_is_locked(lock));
}

#ifdef PLF_ARCH_TICKET_SPINLOCKS
# define ARCH_LOCK_INIT(i) { .owner = ARCH_ATOMIC_INIT(0), .tail = ARCH_ATOMIC_INIT(0) }
#else
# define ARCH_LOCK_INIT(i) { .lock=(i) }
#endif

static inline void arch_unlock(arch_lock_t *lock)
{
#ifdef PLF_ARCH_TICKET_SPINLOCKS

    // release barrier
    __asm__ __volatile__ ("fence rw , w\n" ::: "memory");

    /* Give the spinlock to the next CPU in a FIFO */
    atomic_add(1, &lock->owner);
#else
    __asm__ __volatile__ (
        "amoswap.w.rl x0, x0, %0"
        : "=A" (lock->lock)
        :: "memory");
#endif
}

static inline int arch_trylock(arch_lock_t *lock)
{
#ifdef PLF_ARCH_TICKET_SPINLOCKS
    int owner = atomic_read(&lock->owner);
    int ticket = owner + 1;
    int ret;

    ret = atomic_cas(&lock->tail, owner, ticket);

    // acquire barrier
    __asm__ __volatile__ ("fence r , rw\n" ::: "memory");

    return ret;
#else
    int tmp = 1, busy;

    __asm__ __volatile__ (
        "amoswap.w.aq %0, %2, %1"
        : "=r" (busy), "+A" (lock->lock)
        : "r" (tmp)
        : "memory");

    return !busy;
#endif
}

static inline void arch_lock(arch_lock_t *lock)
{
#ifdef PLF_ARCH_TICKET_SPINLOCKS
    /*
    * Enqueue ourselves to the end of a spinlock waiters queue
    * receiving a ticket
    */
    int ticket = atomic_add(1, &lock->tail);

    while (ticket != atomic_read(&lock->owner))
        cpu_relax();

    // acquire barrier
    __asm__ __volatile__ ("fence r , rw\n" ::: "memory");
#else
    while (1) {
        if (arch_is_locked(lock))
            continue;

        if (arch_trylock(lock))
            break;
    }
#endif
}

#else // PLF_ATOMIC_SUPPORTED

/* 8-bit-per-hart mask size in xlen words */
#define PLF_SMP_HART8_MASK_SIZE ((((PLF_SMP_HART_NUM) * 8) + (__riscv_xlen - 1)) / __riscv_xlen)
#define PLF_SMP_HART8_XLEN (__riscv_xlen / 8)

typedef struct arch_lock {
    volatile unsigned long flags[PLF_SMP_HART8_MASK_SIZE];
} arch_lock_t;

#define ARCH_LOCK_INIT(i) {{(0)}}

void arch_unlock(arch_lock_t *lock);
void arch_lock(arch_lock_t *lock);
#endif // PLF_ATOMIC_SUPPORTED

#else // PLF_SMP_SUPPORT

typedef struct arch_lock {
} arch_lock_t;

#define arch_is_locked(x) (0)
#define arch_unlock_wait(x) do { } while (0)

#define ARCH_LOCK_INIT(i) {}

static inline void arch_unlock(arch_lock_t *lock)
{
    (void)lock;
}

static inline int arch_trylock(arch_lock_t *lock)
{
    (void)lock;

    return 1;
}

static inline void arch_lock(arch_lock_t *lock)
{
    (void)lock;
}

#ifdef __cplusplus
}
#endif

#endif // PLF_SMP_SUPPORT

#endif // SCR_BSP_LOCK_H
