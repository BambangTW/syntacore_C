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
/// Syntacore SCR* framework
///
 /// @copyright Copyright (C) 2015-2020, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// @brief baremetal C/C++ startup

#include "arch.h"
#include "memasm.h"

#include <stdio.h>
#include <stdlib.h>

// implement lite libc startup/exit code
// if C++ exceptions aren't used
// and default behavior is not forced by macro

#if __cpp_exceptions
#define IMPL_BSP_DEFAULT_LIBC_INIT_EXIT 1
#elif BSP_DEFAULT_LIBC_INIT_EXIT
#define IMPL_BSP_DEFAULT_LIBC_INIT_EXIT 1
#endif // __cpp_exceptions

typedef void (*ptr_func)();

extern "C" {
extern ptr_func __preinit_array_start[];
extern ptr_func __preinit_array_end[];

extern ptr_func __init_array_start[];
extern ptr_func __init_array_end[];

extern ptr_func __fini_array_start[];
extern ptr_func __fini_array_end[];

void __libc_init_array(void);
void __libc_fini_array(void);

void _cstart(void);
void _exit(int code);
void exit(int code);

void plf_smp_wait_finit(void);
void plf_smp_hart_finit(void);
}

#ifdef HAL_QEMU_AUTOEXIT
static void qemu_exit(int ret_code);
static bool at_qemu;
#endif // HAL_QEMU_AUTOEXIT

void __init _cstart(void)
{
#ifdef HAL_QEMU_AUTOEXIT
    // QEMU sign: marchid == mimpid.
    // Otherwise explicitly set to zero to support skip bss clear option.
    at_qemu = hal_qemu_sign();
#endif
#if IMPL_BSP_DEFAULT_LIBC_INIT_EXIT
    atexit(__libc_fini_array);
    __libc_init_array();
#else  // IMPL_BSP_DEFAULT_LIBC_INIT_EXIT
    // Call constructors for static objects
    for (ptr_func* fp = __preinit_array_start; fp < __preinit_array_end; ++fp) (*fp)();

    for (ptr_func* fp = __init_array_start; fp < __init_array_end; ++fp) (*fp)();
#endif // IMPL_BSP_DEFAULT_LIBC_INIT_EXIT
}

#if IMPL_BSP_DEFAULT_LIBC_INIT_EXIT
void _exit(int code)
{
    static __attribute__((section (".data"))) int ret_code = 0;

    if (!ret_code)
        ret_code = code;
#ifdef HAL_QEMU_AUTOEXIT
    qemu_exit(ret_code);
#endif

#if PLF_SMP_SUPPORT
    plf_smp_hart_finit();

    if (arch_hartid() == 0) {
        plf_smp_wait_finit();
        _hart_halt(ret_code); // no return
    }
    _hart_halt1(); // no return
#else
    _hart_halt(ret_code); // no return
#endif
}
#else // IMPL_BSP_DEFAULT_LIBC_INIT_EXIT
void exit(int code)
{
    static __attribute__((section (".data"))) int ret_code = 0;

    if (!ret_code)
        ret_code = code;

#if PLF_SMP_SUPPORT
    if (arch_hartid() == PLF_SMP_MASTER_HARTID) {
#endif
        // Call destructors for static objects
        ptr_func* fp = __fini_array_end;

        while (fp > __fini_array_start) { (*(--fp))(); }
#if PLF_SMP_SUPPORT
    }
#endif

#ifdef HAL_QEMU_AUTOEXIT
    qemu_exit(ret_code);
#endif

#if PLF_SMP_SUPPORT
    plf_smp_hart_finit();

    // rtlsim testbenches end the simulation process when
    // the hart number 0 crosses the _hart_halt or sc_exit symbols.
    if (arch_hartid() == 0) {
        // wait for other harts finish their work
        plf_smp_wait_finit();
        _hart_halt(ret_code); // no return
    }
    _hart_halt1(); // no return
#else
    _hart_halt(ret_code); // no return
#endif
}
#endif // IMPL_BSP_DEFAULT_LIBC_INIT_EXIT

#ifdef HAL_QEMU_AUTOEXIT
void qemu_exit(int ret_code)
{
    if (!at_qemu)
        return;

#if PLF_SMP_SUPPORT
    // qemu quits only at exit from main()
    if (arch_hartid() != PLF_SMP_MASTER_HARTID)
        return;
#endif

    printf("RET_CODE = %d\n", ret_code);
    printf("TEST_ADDR_BASE = %p\n", TEST_ADDR_BASE);
    *TEST_ADDR_BASE = (ret_code) ? (0x3333 | ((unsigned long)ret_code << 16)) : 0x5555;
    printf("VALUE_TEST_ADDR_BASE = 0x%lx\n", *TEST_ADDR_BASE);
}
#endif // HAL_QEMU_AUTOEXIT
