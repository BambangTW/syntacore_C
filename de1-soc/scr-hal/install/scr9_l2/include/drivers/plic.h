/*
 * Copyright (C) 2019, Syntacore Ltd.
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
/// @brief PLIC defs and inline funcs
/// Syntacore SCR* infra
///
/// @copyright Copyright (C) 2019, Syntacore Ltd.
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef SCR_BSP_PLIC_H
#define SCR_BSP_PLIC_H

#include <stdarg.h>
#include <stdint.h>

#include "arch.h"

#if PLF_PLIC_BASE

#ifndef PLF_PLIC_OFFSET_SRC_PRIO
#define PLF_PLIC_OFFSET_SRC_PRIO (0x0)
#endif

#ifndef PLF_PLIC_OFFSET_SRC_IP
#define PLF_PLIC_OFFSET_SRC_IP (0x1000)
#endif

#ifndef PLF_PLIC_OFFSET_TGT_IE
#define PLF_PLIC_OFFSET_TGT_IE (0x2000)
#endif

#ifndef PLF_PLIC_OFFSET_SRC_MODE
#define PLF_PLIC_OFFSET_SRC_MODE (0x1f0000)
#endif

#ifndef PLF_PLIC_OFFSET_TGT_PRIO_TH
#define PLF_PLIC_OFFSET_TGT_PRIO_TH (0x200000)
#endif

#ifndef PLF_PLIC_LINE_NUM
#define PLF_PLIC_LINE_NUM (1023)
#endif

#define PLIC_SRC_MODE_OFF          (0)
#define PLIC_SRC_MODE_LEVEL_HIGH   (1)
#define PLIC_SRC_MODE_LEVEL_LOW    (2)
#define PLIC_SRC_MODE_EDGE_RISING  (3)
#define PLIC_SRC_MODE_EDGE_FALLING (4)
#define PLIC_SRC_MODE_EDGE_BOTH    (5)
#define PLIC_SRC_MODE_MAX          (PLIC_SRC_MODE_EDGE_BOTH)

// source line priority (0 - line disabled, 1 - min, 7 - max)
#define PLIC_SRC_PRIO_OFF (0)
#define PLIC_SRC_PRIO_MIN (1)
#define PLIC_SRC_PRIO_MAX (7)
#define PLIC_SRC_PRIO_DFL (3)

// target priority threshold (0 - permits all, 7 - disable all)
#define PLIC_TGT_PRIO_TH_MIN (0)
#define PLIC_TGT_PRIO_TH_MAX (7)
#define PLIC_TGT_PRIO_TH_DFL PLIC_TGT_PRIO_TH_MIN

#ifndef PLF_PLIC_TARGET_NUM
// typical targets mapping:
// M-mode only (MCU) cores: 1 target line per HART
// S-mode (with MMU) cores: 2 target lines per HART
#if PLF_SMP_SUPPORT
#if PLF_MMU_SUPPORT
#define PLF_PLIC_TARGET_NUM (PLF_SMP_HART_NUM * 2) // HARTs with 2 target lines (M-mode + S-mode)
#else
#define PLF_PLIC_TARGET_NUM (PLF_SMP_HART_NUM) // HARTs with M-mode targets lines
#endif // PLF_MMU_SUPPORT
#else
#define PLF_PLIC_TARGET_NUM (1) // single HART with 1 target line (M-mode)
#endif // PLF_SMP_SUPPORT
#endif // PLF_PLIC_TARGET_NUM

#define PLIC_BANK_NUM ((PLF_PLIC_LINE_NUM + 1) / 32)

#define PLIC_TGT_PRIO_TH_OFFSET (1024)

#define PLIC_REGS_OFFSET(offset) ((volatile uint32_t*)((PLF_PLIC_BASE) + (offset)))

#define PLIC_SRC_PRIO    (PLIC_REGS_OFFSET(PLF_PLIC_OFFSET_SRC_PRIO))
#define PLIC_SRC_IP      (PLIC_REGS_OFFSET(PLF_PLIC_OFFSET_SRC_IP))
#define PLIC_SRC_MODE    (PLIC_REGS_OFFSET(PLF_PLIC_OFFSET_SRC_MODE))
#define PLIC_TGT_IE      (PLIC_REGS_OFFSET(PLF_PLIC_OFFSET_TGT_IE))
#define PLIC_TGT_PRIO_TH (PLIC_REGS_OFFSET(PLF_PLIC_OFFSET_TGT_PRIO_TH))

// current target mapping
#ifndef PLF_PLIC_CONTEXT
// typical HART to target mapping:
// M-mode only (MCU) cores: 1 target line per HART
// S-mode (with MMU) cores: 2 target lines per HART
#if PLF_SMP_SUPPORT
#if PLF_MMU_SUPPORT
#define PLF_PLIC_CONTEXT (arch_hartid() * 2) // HARTs with 2 target lines (M-mode + S-mode)
#else
#define PLF_PLIC_CONTEXT (arch_hartid()) // HARTs with M-mode targets lines
#endif // PLF_MMU_SUPPORT
#else
#define PLF_PLIC_CONTEXT (0) // single HART with 1 target line (M-mode)
#endif // PLF_SMP_SUPPORT
#endif // !PLF_PLIC_CONTEXT

#ifdef __cplusplus
extern "C" {
#endif

static inline void plic_line_config(int line, int prio, int mode)
{
    PLIC_SRC_MODE[line] = mode;
    PLIC_SRC_PRIO[line] = prio;
}

static inline void plic_target_setup(int tgt, int prio_th)
{
    PLIC_TGT_PRIO_TH[tgt * PLIC_TGT_PRIO_TH_OFFSET] = prio_th;
}

static inline void plic_target_line_enable(int tgt, int line)
{
    volatile uint32_t* const bits = &PLIC_TGT_IE[tgt * PLIC_BANK_NUM + (line / 32)];
    *bits |= (1 << (line % 32));
}

static inline void plic_target_line_disable(int tgt, int line)
{
    volatile uint32_t* const bits = &PLIC_TGT_IE[tgt * PLIC_BANK_NUM + (line / 32)];
    *bits &= ~(1 << (line % 32));
}

static inline int plic_target_claim(int tgt) { return (int)PLIC_TGT_PRIO_TH[tgt * PLIC_TGT_PRIO_TH_OFFSET + 1]; }

static inline void plic_target_complete(int tgt, int id)
{
    PLIC_TGT_PRIO_TH[tgt * PLIC_TGT_PRIO_TH_OFFSET + 1] = (uint32_t)id;
}

static inline void plic_init(void)
{
    // init targets
    for (int tgt = 0; tgt < PLF_PLIC_TARGET_NUM; ++tgt)
    {
        // disable target
        plic_target_setup(tgt, PLIC_TGT_PRIO_TH_MAX);
        // clear IE bits
        for (int i = 0; i < PLIC_BANK_NUM; ++i) { PLIC_TGT_IE[tgt * PLIC_BANK_NUM + i] = 0; }
    }
    // init lines
    for (int i = 0; i < PLF_PLIC_LINE_NUM; i++)
    {
        // disable line
        plic_line_config(i, PLIC_SRC_PRIO_OFF, PLIC_SRC_MODE_OFF);
    }
}

static inline int plic_irq_setup(int line, int mode)
{
    plic_target_line_disable(PLF_PLIC_CONTEXT, line);
    plic_line_config(line, PLIC_SRC_PRIO_DFL, mode);
    plic_target_setup(PLF_PLIC_CONTEXT, PLIC_TGT_PRIO_TH_DFL);

    return line;
}

static inline void plic_irq_enable(int irq) { plic_target_line_enable(PLF_PLIC_CONTEXT, irq); }

static inline void plic_irq_disable(int irq) { plic_target_line_disable(PLF_PLIC_CONTEXT, irq); }

static inline int plic_soi(void) { return plic_target_claim(PLF_PLIC_CONTEXT); }

static inline void plic_eoi(int irq) { plic_target_complete(PLF_PLIC_CONTEXT, irq); }

#ifdef __cplusplus
}
#endif

#endif // PLF_PLIC_BASE

#endif // SCR_BSP_PLIC_H
