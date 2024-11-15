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
/// @brief platform init funcs
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

#include "arch.h"

#define SCR1_CPU_MARCHID 0x8

static __thread unsigned long _arch_coreid = 0;

/**
 * @brief Core ID value initialization
 */
static void plf_init_arch_coreid()
{
    unsigned long rev;
    unsigned long archid = arch_archid();

    // try to use marchid for core detection
    rev = (archid >> 12) & 0xf;
    if (rev >= 3) {
        _arch_coreid = rev;
        return;
    }

    // SCR1 always has marchid set to SCR1_CPU_MARCHID
    if (archid == SCR1_CPU_MARCHID) {
        _arch_coreid = 1;
        return;
    }

    // fallback to legacy core detection
    rev = (arch_impid() >> 4) & 0xf;

    // apply known fixups
    switch (rev) {
    case 3:
        // Fixup #1: SCR6 reported as SCR3
        if (supports_extension('F') && supports_extension('U') && !supports_extension('S')) {
            rev = 6;
            break;
        }

        // Fixup #2: SCR7 reported as SCR3
        if (supports_extension('F') && supports_extension('U') && supports_extension('S')) {
            rev = 7;
            break;
        }

        break;
    case 4:
        // Fixup #3: SCR3 RV64 reported as SCR4
        if (!supports_extension('F')) {
            rev = 3;
            break;
        }

        break;
    default:
        break;
    }

    _arch_coreid = rev;
}


/**
 * @brief Get Core ID value
 *        A thread variable is used, so TLS/HLS
 *        initialization is needed before using
 */
unsigned long arch_coreid()
{
    if (!(_arch_coreid)) {
        plf_init_arch_coreid();
    }

    return _arch_coreid;
}


/**
 * @brief Get core config for scr9 heavy/lite
 */
unsigned long hal_arch_coreconfig()
{
// scr9 is only 64 bit platform
#if __riscv_xlen == 64
    if (arch_coreid() == 9) {
        return (((arch_impid()) >> 32) & 0xff);
    }
#endif // __riscv_xlen == 64

    return 0;
}
