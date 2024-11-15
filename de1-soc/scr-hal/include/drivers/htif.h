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
/// @brief SiFive HTIF interface
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

#ifndef SCR_BSP_HTIF_H
#define SCR_BSP_HTIF_H

#ifndef __ASSEMBLER__

#include "lock.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile int htif_console_buf;
extern arch_lock_t htif_lock;

void htif_syscall(uintptr_t arg);
void htif_console_putchar(int ch);
int plf_htif_console_getchar(void);

void plf_htif__check_fromhost();
void plf_htif__set_tohost(uintptr_t dev, uintptr_t cmd, uintptr_t data);
void plf_htif_do_tohost_fromhost(long dev, long cmd, uintptr_t data);
void plf_htif_poweroff(int ret_code) __attribute__((noreturn));

// HTIF console i/o
#ifdef PLF_HTIF_CONSOLE
static inline void plf_con_tx_flush(void)
{
}

static inline int plf_con_put(int ch)
{
    htif_console_putchar(ch);

    return ch;
}

static inline int plf_con_getch_nowait(void)
{
    return plf_htif_console_getchar();
}

#define plf_con_init() do {} while (0)
#endif // PLF_HTIF_CONSOLE

#ifdef __cplusplus
}
#endif
#endif // !__ASSEMBLER__
#endif // SCR_BSP_HTIF_H
