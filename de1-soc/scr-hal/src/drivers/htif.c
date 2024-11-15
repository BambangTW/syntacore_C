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
/// @brief implementation of HTIF interface i/o
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

#include "plf.h"

#ifdef PLF_HTIF_BASE

#include "drivers/htif.h"
#include "lock.h"

volatile int htif_console_buf = -1;
arch_lock_t htif_lock = ARCH_LOCK_INIT(0);

void htif_syscall(uintptr_t arg)
{
    plf_htif_do_tohost_fromhost(0, 0, arg);
}

void htif_console_putchar(int ch)
{
#if __riscv_xlen == 32
    volatile uint64_t syscall_task[8];
    syscall_task[0] = 64; // sys_write
    syscall_task[1] = 1;
    syscall_task[2] = (uintptr_t)&ch;
    syscall_task[3] = 1;
    plf_htif_do_tohost_fromhost(0, 0, (uintptr_t)syscall_task);
#else // __riscv_xlen != 32
    arch_lock(&htif_lock);
    plf_htif__set_tohost(1, 1, ch);
    arch_unlock(&htif_lock);
#endif // __riscv_xlen
}

int plf_htif_console_getchar(void)
{
    arch_lock(&htif_lock);
    plf_htif__check_fromhost();
    int ch = htif_console_buf;
    if (ch >= 0) {
        htif_console_buf = -1;
        plf_htif__set_tohost(1, 0, 0);
    }
    arch_unlock(&htif_lock);

    return ch - 1;
}

void htif_halt_handler(int ret_code)
{
#if PLF_SMP_SUPPORT
#if PLF_SMP_MASTER_HARTID >= 0
    if (arch_hartid() != PLF_SMP_MASTER_HARTID) {
        return;
    }
#endif  // PLF_SMP_MASTER_HARTID >= 0
#endif  // PLF_SMP_SUPPORT
    plf_htif_poweroff(ret_code);
}
#endif  // PLF_HTIF_BASE
