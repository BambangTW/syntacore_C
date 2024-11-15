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
///
/// @brief PMP defs and funcs

#ifndef SCR_BSP_PMP_H
#define SCR_BSP_PMP_H

#include "arch.h"
#include "csr.h"
#include "mpu.h"

#if PLF_PMP_SUPPORT

#define PMP_ENTRIES (16)  // a number of PMP entries

#define PMP_R     (0x01)  // read
#define PMP_W     (0x02)  // write
#define PMP_X     (0x04)  // execute
#define PMP_RWX   (0x07)  // rwx
#define PMP_A     (0x18)  // address matching mode
#define PMP_L     (0x80)  // lock

#define PMP_SHIFT (2)

#define PMP_TOR   (0x08)
#define PMP_NA4   (0x10)
#define PMP_NAPOT (0x18)

// PMP CSRs
#define CSR_PMPCFG0   (0x3A0)
#define CSR_PMPCFG1   (0x3A1)
#define CSR_PMPCFG2   (0x3A2)
#define CSR_PMPCFG3   (0x3A3)
#define CSR_PMPADDR0  (0x3B0)
#define CSR_PMPADDR1  (0x3B1)
#define CSR_PMPADDR2  (0x3B2)
#define CSR_PMPADDR3  (0x3B3)
#define CSR_PMPADDR4  (0x3B4)
#define CSR_PMPADDR5  (0x3B5)
#define CSR_PMPADDR6  (0x3B6)
#define CSR_PMPADDR7  (0x3B7)
#define CSR_PMPADDR8  (0x3B8)
#define CSR_PMPADDR9  (0x3B9)
#define CSR_PMPADDR10 (0x3BA)
#define CSR_PMPADDR11 (0x3BB)
#define CSR_PMPADDR12 (0x3BC)
#define CSR_PMPADDR13 (0x3BD)
#define CSR_PMPADDR14 (0x3BE)
#define CSR_PMPADDR15 (0x3BF)

/// PMP CTRL OFFSETS
#define SCR_PMP_CTRL_MATTR_OFF    (5)

#define SCR_PMP_CTRL_MT_MASK      (3 << 5)
#define SCR_PMP_CTRL_MT_WEAKLY    (0 << 5)
#define SCR_PMP_CTRL_MT_STRONG    (1 << 5)
#define SCR_PMP_CTRL_MT_WEAKLY_NC (2 << 5)
#define SCR_PMP_CTRL_MT_CONFIG    (3 << 5)

#endif // PLF_PMP_SUPPORT

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if PLF_PMP_SUPPORT

/**
 * \brief read PMP region attributes
 * \param [in] sel PMP entry number
 */
static inline size_t read_pmp_ctrl(size_t sel)
{
    size_t ctrl = 0;
    size_t cfg_num, offset;

    if (sel >= PMP_ENTRIES) {
        return 0;
    }

#if __riscv_xlen == 32
    cfg_num = sel / 4;
    offset = (sel % 4) * 8;
#else
    cfg_num = (sel / 8) * 2;
    offset = (sel % 8) * 8;
#endif

    switch (cfg_num) {
        case 0: ctrl = read_csr(CSR_PMPCFG0); break;
        case 1: ctrl = read_csr(CSR_PMPCFG1); break;
        case 2: ctrl = read_csr(CSR_PMPCFG2); break;
        case 3: ctrl = read_csr(CSR_PMPCFG3); break;
    }
    return 0xFF & (ctrl >> offset);
}

/**
 * \brief write PMP region attributes
 * \param [in] sel PMP entry number
 * \param [in] ctrl PMP config attributes
 */
static inline void write_pmp_ctrl(size_t sel, size_t ctrl)
{
    size_t cfg_num, offset;

    if (sel >= PMP_ENTRIES) {
        return;
    }

#if __riscv_xlen == 32
    cfg_num = sel / 4;
    offset = (sel % 4) * 8;
#else
    cfg_num = (sel / 8) * 2;
    offset = (sel % 8) * 8;
#endif

    size_t current_ctrl = 0;
    switch (cfg_num) {
        case 0: current_ctrl = read_csr(CSR_PMPCFG0); break;
        case 1: current_ctrl = read_csr(CSR_PMPCFG1); break;
        case 2: current_ctrl = read_csr(CSR_PMPCFG2); break;
        case 3: current_ctrl = read_csr(CSR_PMPCFG3); break;
    }
    current_ctrl &= ~((size_t)0xFF << offset);
    ctrl = (ctrl & 0xFF) << offset;
    ctrl = current_ctrl | ctrl;
    switch (cfg_num) {
        case 0: write_csr(CSR_PMPCFG0, ctrl); break;
        case 1: write_csr(CSR_PMPCFG1, ctrl); break;
        case 2: write_csr(CSR_PMPCFG2, ctrl); break;
        case 3: write_csr(CSR_PMPCFG3, ctrl); break;
    }
}

#define PMPADDR(n, region) case n: write_csr(CSR_PMPADDR##n, region); break;

/**
 * \brief set up a PMP region
 * \param [in] sel PMP entry number
 * \param [in] addr PMP region address
 * \param [in] mask PMP region mask
 * \param [in] ctrl PMP region attributes
 */
static inline void create_pmp_region(size_t sel, size_t addr, size_t mask, size_t ctrl)
{
    const size_t region = (addr >> PMP_SHIFT) | ((~mask) >> (PMP_SHIFT + 1));
    write_pmp_ctrl(sel, 0);
    switch (sel){
        PMPADDR( 0, region);
        PMPADDR( 1, region);
        PMPADDR( 2, region);
        PMPADDR( 3, region);
        PMPADDR( 4, region);
        PMPADDR( 5, region);
        PMPADDR( 6, region);
        PMPADDR( 7, region);
        PMPADDR( 8, region);
        PMPADDR( 9, region);
        PMPADDR(10, region);
        PMPADDR(11, region);
        PMPADDR(12, region);
        PMPADDR(13, region);
        PMPADDR(14, region);
        PMPADDR(15, region);
    }
    write_pmp_ctrl(sel, ctrl | PMP_NAPOT);
}

/**
 * \brief disable PMP region
 * \param [in] sel PMP entry number
 */
static inline void disable_pmp_region(size_t sel)
{
    write_pmp_ctrl(sel, read_pmp_ctrl(sel) & (~PMP_NAPOT));
}

/**
 * \brief enable PMP region
 * \param [in] sel PMP entry number
 */
static inline void enable_pmp_region(size_t sel)
{
    write_pmp_ctrl(sel, read_pmp_ctrl(sel) | PMP_NAPOT);
}

/**
 * \brief save and return old control, write new attibutes
 * \param [in] sel PMP entry number
 * \param [in] attrs PMP region attributes
 */
static inline size_t write_pmp_ctrl_region(size_t sel, size_t attrs)
{
    const size_t ctrl = read_pmp_ctrl(sel);
    write_pmp_ctrl(sel, attrs);
    return ctrl;
}

static inline void setup_pmp_regions(const scr_mem_region_info* const regions, unsigned region_count)
{
    unsigned rn = 0;
    unsigned i  = region_count;

    while (i--)
    {
        const scr_mem_region_info* rgn = &regions[i];
        if (rgn->attr)
        {
            create_pmp_region(rn++, rgn->base, ~(rgn->size - 1), rgn->attr);
            ifence();
        }
    }
}

void pmp_init(void);
void pmp_early_init(void);

#else // PLF_PMP_SUPPORT

#define pmp_init(void) do {} while (0)
#define pmp_early_init(void) do {} while (0)

#endif // PLF_PMP_SUPPORT

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__

#endif // SCR_BSP_PMP_H
