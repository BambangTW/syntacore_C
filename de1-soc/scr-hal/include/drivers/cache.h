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
/// @brief Cache CSR's defs and inlines
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

#ifndef SCR_BSP_CACHE_H
#define SCR_BSP_CACHE_H

#include "plf.h"

// global cache's control bits
#define CACHE_GLBL_L1I_EN  (1 << 0)
#define CACHE_GLBL_L1D_EN  (1 << 1)
#define CACHE_GLBL_L1I_INV (1 << 2)
#define CACHE_GLBL_L1D_INV (1 << 3)
#define CACHE_GLBL_ENABLE  (CACHE_GLBL_L1I_EN | CACHE_GLBL_L1D_EN)
#define CACHE_GLBL_DISABLE (0)
#define CACHE_GLBL_INV (CACHE_GLBL_L1I_INV | CACHE_GLBL_L1D_INV)

#if PLF_CACHE_CFG
#ifndef PLF_CACHELINE_SIZE
#error PLF_CACHELINE_SIZE shall be defined
#endif
#endif

#ifdef INSN_NOT_SUPPORTED
#include "asm-magic.h"
#endif // INSN_NOT_SUPPORTED

// cache control CSRs
#define SCR_CSR_CACHE_GLBL (0xBD4)
// cache info CSRs
#define SCR_CSR_CACHE_DSCR_L1 (0xFC3)

#ifdef INSN_NOT_SUPPORTED
#define ENC_CLINV(regn) (0x10800073 | (((regn) & 0x1f) << 15))
// instruction: clflush <regn> (cache line flush & invalidate)
#define ENC_CLFLUSH(regn) (0x10900073 | (((regn) & 0x1f) << 15))
#endif // INSN_NOT_SUPPORTED

#define L1D_PREFETCHER_CTRL_REG_0 0xBF0
#define L1D_PREFETCHER_CTRL_REG_1 0xBF1

#define L1D_PREFETCHER_ENABLE_BIT         (1 << 0)
#define L1D_PREFETCHER_CONF_CTR_TH_SHIFT  (1)
#define L1D_PREFETCHER_CONF_CTR_MAX_SHIFT (5)
#define L1D_PREFETCHER_PREF_CTR_MAX_SHIFT (9)
#define L1D_PREFETCHER_PARAM_MASK         (0xF)

#define L1D_DEBUG_OPTIONS 0xBE1

#define L1D_DEBUG_SPEC_LD_DISABLE_BIT         (1 << 1)
#define L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT (1 << 6)
#define L1D_DEBUG_LSDP_DISABLE_BIT            (1 << 7)

#ifdef PLF_L2CTL_BASE

#define L2_CSR_VER_IDX   (0)
#define L2_CSR_DESCR_IDX (1)
#define L2_CSR_EN_IDX    (4)
#define L2_CSR_FLUSH_IDX (5)
#define L2_CSR_INV_IDX   (6)
#define L2_CSR_BUSY_IDX  (11)
#define L2_CSR_SYSCO_IDX (40)

#define L2_DESCR_SHIFT_WAYS    (0)
#define L2_DESCR_SHIFT_LINE    (4)
#define L2_DESCR_SHIFT_WIDTH   (8)
#define L2_DESCR_SHIFT_TYPE    (13)
#define L2_DESCR_SHIFT_BANKS   (16)
#define L2_DESCR_SHIFT_CPU_NUM (28)

#define L2_DESCR_MASK_WAYS    (0x07)
#define L2_DESCR_MASK_LINE    (0x0F)
#define L2_DESCR_MASK_WIDTH   (0x1F)
#define L2_DESCR_MASK_BANKS   (0x0F)
#define L2_DESCR_MASK_CPU_NUM (0x0F)

#define L2_DESCR_MASK_TYPE_WRITE_BACK (1 << 0)
#define L2_DESCR_MASK_TYPE_ALLOCATE   (1 << 1)
#define L2_DESCR_MASK_TYPE_INCLUSIVE  (1 << 2)

#define PMUC_BANK_SEL_MASK 0xF0000

// L2_PCEN_CTRL (0x400+N*0x10), N = number of counter
#define L2_PCEN_STEP        (0x10)
#define L2_PCEN_CTRL_OFFSET (0x400)
#define L2_PCEN_CTRL(i) (PLF_L2CTL_BASE + L2_PCEN_CTRL_OFFSET + i * L2_PCEN_STEP)

//L2_PCEN_LO (0x408 + N*0x10)
//L2_PCEN_HI (0x40C + N*0x10)
#define L2_PCEN_COUNTER_LO_OFFSET (0x408)
#define L2_PCEN_COUNTER_HI_OFFSET (0x40C)
#define L2_PCEN_COUNTER_LO(i) (PLF_L2CTL_BASE + L2_PCEN_COUNTER_LO_OFFSET + i * L2_PCEN_STEP)
#define L2_PCEN_COUNTER_HI(i) (PLF_L2CTL_BASE + L2_PCEN_COUNTER_HI_OFFSET + i * L2_PCEN_STEP)
#define L2_COUNTER_LO(i) (*(volatile uint32_t*)L2_PCEN_COUNTER_LO(i))
#define L2_COUNTER_HI(i) (*(volatile uint32_t*)L2_PCEN_COUNTER_HI(i))

#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE

#define L3C_REG_SIZE (8)

#define L3C_VID_IDX          (0)
#define L3C_DESCR_CACHE_IDX  (1)
#define L3C_DESCR_BANK_IDX   (2)
#define L3C_FEAT_EN_IDX      (5)
#define L3C_INTERLEAVING_IDX (6)
#define L3C_MCP_CTRL         (7)
#define L3C_CMD_CTRL_IDX     (8)
#define L3C_CMD_ADDR_IDX     (9)
#define L3C_CMD_DATA0_IDX    (10)
#define L3C_CMD_DATA1_IDX    (11)
#define L3C_CMD_DATA2_IDX    (12)
#define L3C_CMD_DATA3_IDX    (13)
#define L3C_CMD_DATA4_IDX    (14)
#define L3C_CMD_DATA5_IDX    (15)
#define L3C_CMD_DATA6_IDX    (16)
#define L3C_CMD_DATA7_IDX    (17)

#define L3B_STEP    (32)

#define L3B_STS     (0)
#define L3B_MCP_STS (1)
#define L3B_CMD_STS (2)

#define L3B_PCE_CNTR_STEP (16)

#define L3C_DESCR_CACHE_SHIFT_CPU_NUM   (0)
#define L3C_DESCR_CACHE_SHIFT_BANK_NUM  (8)
#define L3C_DESCR_CACHE_SHIFT_MEM_NUM   (16)
#define L3C_DESCR_CACHE_SHIFT_IO_NUM    (24)

#define L3C_DESCR_BANK_SHIFT_IDX_NUM    (0)
#define L3C_DESCR_BANK_SHIFT_WAY_NUM    (8)
#define L3C_DESCR_BANK_SHIFT_LINE_WIDTH (16)
#define L3C_DESCR_BANK_SHIFT_DAT_WIDTH  (24)
#define L3C_DESCR_BANK_SHIFT_ADDR_WIDTH (32)

#define L3C_MASK (0xFF)

#define L3C_CMD_OPCODE_NOP         (0) // No operation
#define L3C_CMD_OPCODE_FLUSH       (1) // Eviction
#define L3C_CMD_OPCODE_INVALID     (2) // Invalidation
#define L3C_CMD_OPCODE_CLEAN       (3) // Sync with memory
#define L3C_CMD_OPCODE_LOAD_ADDR   (4) // Load data by full addr
#define L3C_CMD_OPCODE_LOAD_DIRECT (5) // Load data by index and way

#define L3B_CMD_STS_DONE (1)

#endif // PLF_L3CTL_BASE

#ifndef __ASSEMBLER__

#include "arch.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void plf_l1cache_enable(void);
void plf_l1cache_disable(void);
bool plf_l1cache_is_enabled(void);
void plf_l1cache_init(void);
void cache_l1_invalidate(void *vaddr, long size);
void cache_l1_flush(void *vaddr, long size);

int cache_l1i_info(char *buf, size_t len);
int cache_l1d_info(char *buf, size_t len);

static inline void plf_l1cache_invalidate(void *vaddr, long size)
{
   cache_l1_invalidate(vaddr, size);
}

static inline void plf_l1cache_flush(void *vaddr, long size)
{
   cache_l1_flush(vaddr, size);
}

static inline __attribute__((always_inline))
void plf_l1cache_debug_feature_reset(uint64_t feature_bit)
{
    const bool disabled = read_csr(L1D_DEBUG_OPTIONS) & feature_bit;

    if (disabled == false) {
        set_csr(L1D_DEBUG_OPTIONS, feature_bit);
        clear_csr(L1D_DEBUG_OPTIONS, feature_bit);
    }
}

static inline __attribute__((always_inline))
void plf_l1cache_prefetcher_feature_reset(uint64_t feature_bit)
{
    const bool enabled = read_csr(L1D_PREFETCHER_CTRL_REG_0) & feature_bit;

    clear_csr(L1D_PREFETCHER_CTRL_REG_0, feature_bit);
    set_csr(L1D_PREFETCHER_CTRL_REG_0, feature_bit);

    if (enabled == false) {
        clear_csr(L1D_PREFETCHER_CTRL_REG_0, feature_bit);
    }
}

static inline __attribute__((always_inline))
void plf_l1cache_prefetcher_reset()
{
    plf_l1cache_prefetcher_feature_reset(L1D_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_prefetcher_enable(void)
{
    set_csr(L1D_PREFETCHER_CTRL_REG_0, L1D_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_prefetcher_disable(void)
{
#if defined(L1D_PREFETCHER)
    clear_csr(L1D_PREFETCHER_CTRL_REG_0, L1D_PREFETCHER_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, L1D_PREFETCHER_ENABLE_BIT);
#endif // L1D_PREFETCHER
}

static inline __attribute__((always_inline))
bool plf_l1cache_prefetcher_is_enabled(void)
{
    return (read_csr(L1D_PREFETCHER_CTRL_REG_0) & L1D_PREFETCHER_ENABLE_BIT);
};

static inline __attribute__((always_inline))
bool plf_l1cache_prefetcher_is_available(void)
{
    return ((arch_coreid() == 7) || (arch_coreid() == 9));
}

int plf_l1cache_prefetcher_info(char *buf, size_t len);

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_reset()
{
    plf_l1cache_debug_feature_reset(L1D_DEBUG_SPEC_LD_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_enable(void)
{
    clear_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_SPEC_LD_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_disable(void)
{
#if defined(L1D_LOAD_SPEC)
    set_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_SPEC_LD_DISABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    set_csr(mhpmevent3, L1D_DEBUG_SPEC_LD_DISABLE_BIT);
#endif // L1D_LOAD_SPEC
}

static inline __attribute__((always_inline))
bool plf_l1cache_spec_ld_is_enabled(void)
{
    return (!(read_csr(L1D_DEBUG_OPTIONS) & L1D_DEBUG_SPEC_LD_DISABLE_BIT));
}

static inline __attribute__((always_inline))
bool plf_l1cache_spec_ld_is_available(void)
{
    return (arch_coreid() == 9);
}

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_over_st_reset()
{
    plf_l1cache_debug_feature_reset(L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_over_st_enable(void)
{
    clear_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_spec_ld_over_st_disable(void)
{
#if defined(L1D_LOAD_OVER_STORE_SPEC)
    set_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    set_csr(mhpmevent3, L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT);
#endif // L1D_LOAD_OVER_STORE_SPEC
}

static inline __attribute__((always_inline))
bool plf_l1cache_spec_ld_over_st_is_enabled(void)
{
    return (!(read_csr(L1D_DEBUG_OPTIONS) & L1D_DEBUG_SPEC_LD_OVER_ST_DISABLE_BIT));
}

static inline __attribute__((always_inline))
bool plf_l1cache_spec_ld_over_st_is_available(void)
{
    return (arch_coreid() == 9);
}

static inline __attribute__((always_inline))
void plf_l1cache_lsdp_reset()
{
    plf_l1cache_debug_feature_reset(L1D_DEBUG_LSDP_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_lsdp_enable(void)
{
    clear_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_LSDP_DISABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_lsdp_disable(void)
{
#if defined(L1D_LOAD_STORE_DEP_PREDICTOR)
    set_csr(L1D_DEBUG_OPTIONS, L1D_DEBUG_LSDP_DISABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    set_csr(mhpmevent3, L1D_DEBUG_LSDP_DISABLE_BIT);
#endif // L1D_LOAD_STORE_DEP_PREDICTOR
}

static inline __attribute__((always_inline))
bool plf_l1cache_lsdp_is_enabled(void)
{
    return (!(read_csr(L1D_DEBUG_OPTIONS) & L1D_DEBUG_LSDP_DISABLE_BIT));
}

static inline __attribute__((always_inline))
bool plf_l1cache_lsdp_is_available(void)
{
    return (arch_coreid() == 9);
}

void plf_l2cache_enable(void);
void plf_l2cache_disable(void);
bool plf_l2cache_is_enabled(void);
void plf_l2cache_init(void);

int cache_l2_info(char *buf, size_t len);

void plf_l3cache_init(void);

int cache_l3_info(char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#else // !__ASSEMBLER__

#include "csr.h"

.altmacro

// assembler macros

#ifndef INSN_NOT_SUPPORTED
// cache line flush
.macro clflush reg
    .insn i SYSTEM, 0, zero, \reg, 0b000100001001
.endm

// cache line invalidate
.macro clinval reg
    .insn i SYSTEM, 0, zero, \reg, 0b000100001000
.endm
#endif // INSN_NOT_SUPPORTED

// reset L1 and disable caching
.macro cache_l1_reset_nc
#ifdef PLF_CACHE_CFG
    // setup caches: disabled, flush&invalidate
    LOCAL cache_inv_wait, cache_inv_skip
    csrr t0, SCR_CSR_CACHE_DSCR_L1
    beqz t0, cache_inv_skip
    csr_write_imm SCR_CSR_CACHE_GLBL, %(CACHE_GLBL_DISABLE | CACHE_GLBL_INV)
#ifdef __riscv_zifencei
    fence.i
#endif
    // wait until invalidation complete
cache_inv_wait:
    csrr t0, SCR_CSR_CACHE_GLBL
    andi t0, t0, CACHE_GLBL_INV
    bnez t0, cache_inv_wait
#ifdef __riscv_zifencei
    fence.i
#endif
cache_inv_skip:
#endif
.endm

.macro cache_l1_flush addr, size
    LOCAL cache_flush_loop
cache_flush_loop:
    clflush \addr
    addi \size, \size, -PLF_CACHELINE_SIZE
    addi \addr, \addr, PLF_CACHELINE_SIZE
    bgez \size, cache_flush_loop
.endm // cache_l1_flush

#endif // !__ASSEMBLER__
#endif // SCR_BSP_CACHE_H
