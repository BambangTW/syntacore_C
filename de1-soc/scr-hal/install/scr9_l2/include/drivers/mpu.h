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
/// @brief MPU defs/macros
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

#ifndef SCR_BSP_MPU_H
#define SCR_BSP_MPU_H

#include "drivers/mpu_regs.h"
#include "csr.h"

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLF_MEM_MAP
// platform memory configuration
typedef struct scr_mem_region_info {
    uintptr_t     base;
    uintptr_t     size;
    unsigned long attr;
    const char   *name;
#ifdef PLF_MRT_SUPPORT
    unsigned long  mrt_attr;
#endif
} scr_mem_region_info;

extern const scr_mem_region_info mem_regions[];
extern const unsigned mem_region_count;
#endif // PLF_MEM_MAP

#if PLF_MPU_SUPPORT
static inline void scr_mpu_region_update_attr(long regn, unsigned long attr)
{
    write_csr(SCR_CSR_MPU_SEL, regn);
    write_csr(SCR_CSR_MPU_CTRL, attr);
}

static inline unsigned long scr_mpu_region_get_attr(long regn)
{
    write_csr(SCR_CSR_MPU_SEL, regn);
    return read_csr(SCR_CSR_MPU_CTRL);
}

static inline void scr_mpu_region_disable(long regn)
{
    scr_mpu_region_update_attr(regn, 0);
}

static inline void scr_mpu_region_update(long regn, uintptr_t base, unsigned long size, unsigned long attr)
{
    write_csr(SCR_CSR_MPU_SEL, regn);
    write_csr(SCR_CSR_MPU_CTRL, attr);
    write_csr(SCR_CSR_MPU_ADDR, SCR_MPU_MK_ADDR(base));
    write_csr(SCR_CSR_MPU_MASK, SCR_MPU_MK_MASK(size));
}

static inline void scr_mpu_region_setup(long regn, uintptr_t base, unsigned long size, unsigned long attr)
{
    write_csr(SCR_CSR_MPU_SEL, regn);
    write_csr(SCR_CSR_MPU_CTRL, 0);
    write_csr(SCR_CSR_MPU_ADDR, SCR_MPU_MK_ADDR(base));
    write_csr(SCR_CSR_MPU_MASK, SCR_MPU_MK_MASK(size));
    write_csr(SCR_CSR_MPU_CTRL, attr);
}

static inline void scr_mpu_reset_init(void)
{
    // region0= NC,SO, global
    scr_mpu_region_update(0, 0, 0, SCR_MPU_CTRL_MA | SCR_MPU_CTRL_MT_STRONG | SCR_MPU_CTRL_VALID);
    ifence();
    // region1= MMCFG
    scr_mpu_region_setup(1, PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR | SCR_MPU_CTRL_VALID);
    // region2= MMIO
    scr_mpu_region_setup(2, PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR | SCR_MPU_CTRL_VALID);
    // all other regions= disabled
    for (unsigned rgn = 3; ; ++rgn) {
        write_csr(SCR_CSR_MPU_SEL, rgn);
        if (read_csr(SCR_CSR_MPU_SEL) == 0)
            break;
        write_csr(SCR_CSR_MPU_CTRL, 0);
    }
}

static inline void scr_mpu_setup(const scr_mem_region_info* regions, unsigned region_count)
{
    // configure all regions
    unsigned rn = 1;
    for (const scr_mem_region_info* rgn = regions; rgn < regions + region_count; ++rgn)
    {
        if (rgn->attr) {
            scr_mpu_region_setup(rn, rgn->base, rgn->size, rgn->attr | SCR_MPU_CTRL_VALID);
            ifence();
            ++rn;
        }
    }
    if (rn > 1) {
        // disable all unused regions, disable default region #0
        do {
            write_csr(SCR_CSR_MPU_SEL, rn++);
            write_csr(SCR_CSR_MPU_CTRL, 0);
            ifence();
        } while (read_csr(SCR_CSR_MPU_SEL) != 0);
    }
}

void mpu_init(void);
#else
#define mpu_init() do {} while (0)
#endif // PLF_MPU_SUPPORT

#ifdef __cplusplus
}
#endif

#else //!__ASSEMBLER__

   .altmacro

// MPU control macros

.macro mpu_region_update_attr idx,attr
    csr_write_imm SCR_CSR_MPU_SEL, \idx
    csr_write_imm SCR_CSR_MPU_CTRL, \attr
.endm

.macro mpu_region_disable idx
    mpu_region_update_attr \idx, 0
.endm

.macro mpu_region_get_attr idx,dst
    csr_write_imm SCR_CSR_MPU_SEL, \idx
    csrr \dst, SCR_CSR_MPU_CTRL
.endm

// update region
.macro mpu_region_update idx,addr,size,attr
    csr_write_imm SCR_CSR_MPU_SEL, \idx
    csr_write_imm SCR_CSR_MPU_CTRL, \attr
    csr_write_imm SCR_CSR_MPU_ADDR, %(SCR_MPU_MK_ADDR(\addr))
    csr_write_imm SCR_CSR_MPU_MASK, %(SCR_MPU_MK_MASK(\size))
.endm

// setup region
.macro mpu_region_setup idx,addr,size,attr
    csr_write_imm SCR_CSR_MPU_SEL, \idx
    csr_write_imm SCR_CSR_MPU_CTRL, 0
    csr_write_imm SCR_CSR_MPU_ADDR, %(SCR_MPU_MK_ADDR(\addr))
    csr_write_imm SCR_CSR_MPU_MASK, %(SCR_MPU_MK_MASK(\size))
    csr_write_imm SCR_CSR_MPU_CTRL, \attr
.endm

.macro mpu_reset_init
#if PLF_MPU_SUPPORT
    // reset MPU
    LOCAL mpu_reset_rep
    // region0= NC,SO, global
    mpu_region_update 0, 0, 0, (SCR_MPU_CTRL_MA | SCR_MPU_CTRL_MT_STRONG | SCR_MPU_CTRL_VALID)
#ifdef __riscv_zifencei
    fence.i
#endif
    // region1= MMCFG
    mpu_region_setup 1, PLF_MMCFG_BASE, PLF_MMCFG_SIZE, (PLF_MMCFG_ATTR | SCR_MPU_CTRL_VALID)
    // region2= MMIO
    mpu_region_setup 2, PLF_MMIO_BASE, PLF_MMIO_SIZE, (PLF_MMIO_ATTR | SCR_MPU_CTRL_VALID)
    // all other regions= disabled
    li t0, 3
    li t1, -1
    csrw SCR_CSR_MPU_SEL, t1
    csrr t1, SCR_CSR_MPU_SEL
mpu_reset_rep:
    csrw SCR_CSR_MPU_SEL, t0
    csrw SCR_CSR_MPU_CTRL, zero
    addi t0, t0, 1
    ble  t0, t1, mpu_reset_rep
#endif // PLF_MPU_SUPPORT
.endm // mpu_reset_init

#endif // __ASSEMBLER__
#endif // SCR_BSP_MPU_H
