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
/// @brief Architecture specific CSR's defs and inlines
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

#ifndef SCR_BSP_CSR_H
#define SCR_BSP_CSR_H

#ifndef __ASSEMBLER__

#include "stringify.h"

#define swap_csr(reg, val)                         \
({                                                 \
    unsigned long __tmp = (unsigned long)(val);    \
    asm volatile ("csrrw %0," STRINGIFY(reg) ",%1" \
                  : "=r" (__tmp) : "rK" (__tmp)    \
                  : "memory");                     \
    __tmp;                                         \
})

#define read_csr(reg)                              \
({                                                 \
    unsigned long __tmp;                           \
    asm volatile ("csrr %0," STRINGIFY(reg)        \
                  : "=r" (__tmp) :                 \
                  : "memory");                     \
    __tmp;                                         \
})

#define write_csr(reg, val)                        \
({                                                 \
    unsigned long __tmp = (unsigned long)(val);    \
    asm volatile ("csrw " STRINGIFY(reg) ",%0"     \
                  : : "rK" (__tmp)                 \
                  : "memory");                     \
})

#define set_csr(reg, val)                          \
({                                                 \
    unsigned long __tmp = (unsigned long)(val);    \
    asm volatile ("csrrs %0," STRINGIFY(reg) ",%1" \
                  : "=r" (__tmp) : "rK" (__tmp)    \
                  : "memory");                     \
    __tmp;                                         \
})

#define clear_csr(reg, val)                     \
({                                              \
    unsigned long __tmp = (unsigned long)(val); \
    asm volatile ("csrrc %0," STRINGIFY(reg) ",%1" \
                  : "=r" (__tmp) : "rK" (__tmp) \
                  : "memory");                  \
    __tmp;                                      \
})

#if __riscv_xlen == 32
#define read_csr64(reg)                                                               \
({                                                                                    \
    uint32_t lo, hi, tmp;                                                             \
                                                                                      \
    asm volatile("1:"                                                                 \
                 "csrr %[hi], " STRINGIFY(CONCAT(reg,h)) ";"                          \
                 "csrr %[lo], " STRINGIFY(reg) ";"                                    \
                 "csrr %[tmp]," STRINGIFY(CONCAT(reg,h)) ";"                          \
                 "bne  %[hi], %[tmp], 1b"                                             \
                 : [ hi ] "=&r"(hi), [ lo ] "=&r"(lo), [ tmp ] "=&r"(tmp)::"memory"); \
                                                                                      \
    ((uint64_t)hi << 32) | lo;                                                        \
})
#endif // __riscv_xlen == 32

#else // !__ASSEMBLER__

// assembler macros

.altmacro

.macro csr_write_imm csr_reg, val
    .if \val == 0
    csrw \csr_reg, zero
    .elseif (\val / 32) == 0
    csrwi \csr_reg, \val
    .else
    li   t0, \val
    csrw \csr_reg, t0
    .endif
.endm

#endif // !__ASSEMBLER__
#endif // SCR_BSP_CSR_H
