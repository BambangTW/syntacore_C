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

/// @file sys_utils.S
/// @brief system helpers
/// Syntacore SCR* infra
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

#include "memasm.h"

.globl read_safe_m
.section ".text.read_safe_m","ax",@progbits
.type read_safe_m, @function
read_safe_m:
    // get memory to safe read
    mv    a5, a0

    // set default return value and custom address
    li    a0, -1
    la    a2, 1f

    // save trap vectors, status and interrupts
    csrrw a2, mtvec, a2
    csrr  a3, mstatus
    csrrw a4, mie, zero

    // if memory read susscess, set return value to 0
    LOAD_XREG a5, 0(a5)
    li    a0, 0

.align 2
    // restore trap vectors, status and interrupts
1:  csrw  mtvec,a2
    csrw  mstatus,a3
    csrw  mie, a4

    // a0 contain return value and a1 contain memory ro read value
    SAVE_XREG a5, 0(a1)
    ret
    .size read_safe_m, .-read_safe_m