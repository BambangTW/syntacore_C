/*
 * Copyright (C) 2022, Syntacore Ltd.
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
/// @brief AIA constants and functions
/// @copyright Syntacore LLC, 2022.
#ifndef SCR_BSP_AIA_H
#define SCR_BSP_AIA_H

#include <stddef.h>
#include <stdint.h>
#include "arch.h"
#include "csr.h"

#if PLF_APLIC_BASE_M

// Common definitions

#define AIA_REG32_BITS_IN_REG            (32)
#define AIA_REG32_POW2_BITS_IN_REG       (5)
#define AIA_REG32_BYTE_OFFSET(src)       ((src >> AIA_REG32_POW2_BITS_IN_REG) * sizeof(uint32_t))
#define AIA_REG32_BIT_OFFSET(src)        (src & (AIA_REG32_BITS_IN_REG - 1))
#define AIA_REG32_WORD_OFFSET(src)       ((src >> AIA_REG32_POW2_BITS_IN_REG))

#define AIA_REG64_BITS_IN_REG            (64)
#define AIA_REG64_POW2_BITS_IN_REG       (6)
#define AIA_REG64_BYTE_OFFSET(src)       ((src >> AIA_REG64_POW2_BITS_IN_REG) * sizeof(uint64_t))
#define AIA_REG64_BIT_OFFSET(src)        (src & (AIA_REG64_BITS_IN_REG - 1))

#if __riscv_xlen == 64
#define AIA_REG_BITS_IN_REG              (AIA_REG64_BITS_IN_REG)
#define AIA_REG_POW2_BITS_IN_REG         (AIA_REG64_POW2_BITS_IN_REG)
#define AIA_REG_BYTE_OFFSET(src)         (AIA_REG64_BYTE_OFFSET(src))
#define AIA_REG_BIT_OFFSET(src)          (AIA_REG64_BIT_OFFSET(src))
#else /* __riscv_xlen == 64 */
#define AIA_REG_BITS_IN_REG              (AIA_REG32_BITS_IN_REG)
#define AIA_REG_POW2_BITS_IN_REG         (AIA_REG32_POW2_BITS_IN_REG)
#define AIA_REG_BYTE_OFFSET(src)         (AIA_REG32_BYTE_OFFSET(src))
#define AIA_REG_BIT_OFFSET(src)          (AIA_REG32_BIT_OFFSET(src))
#endif /* __riscv_xlen == 64 */

#define AIA_REG_POW2_BYTES_IN_REG        (AIA_REG_POW2_BITS_IN_REG - 3)
#define AIA_REG_BYTES_IN_REG             (AIA_REG_BITS_IN_REG >> 3)

// APLIC definitions

#define APLIC_M_BASE_ADDR (PLF_APLIC_BASE_M) // Machine level
#define APLIC_S_BASE_ADDR (PLF_APLIC_BASE_S) // Supervisor level

#define APLIC_SOURCE_MIN   (1)
#define APLIC_SOURCE_MAX   (PLF_APLIC_NUM_SOURCE)
#define APLIC_SOURCE_LIMIT (1023)

#define APLIC_M (0) // Machine level domain
#define APLIC_S (1) // Supervisor level domain

#define APLIC_REG_ALL_BITS_MASK     (0xFFFFFFFF)

#define APLIC_BASE_ADDR(domain) ((domain) ? APLIC_S_BASE_ADDR : APLIC_M_BASE_ADDR)

static inline uint32_t readl(size_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void writel(uint32_t value, size_t addr)
{
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t aplic_get(size_t domain, size_t reg)
{
    return readl(APLIC_BASE_ADDR(domain) + reg);
}

static inline void aplic_set(size_t domain, size_t reg, uint32_t value)
{
    writel(value, APLIC_BASE_ADDR(domain) + reg);
}

#define APLIC_DOMAINCFG (0x0000)

#define APLIC_DOMAINCFG_BE_SHIFT    (0)   // domaincfg[0:0] Big-Endian (ro)
#define APLIC_DOMAINCFG_BE_MASK     (0x1) // domaincfg[0:0] Big-Endian (ro)
#define APLIC_DOMAINCFG_BE_BIT      BIT(APLIC_DOMAINCFG_BE_SHIFT)
#define APLIC_DOMAINCFG_DM_SHIFT    (2)   // domaincfg[2:2] Delivery Mode (0 - direct, 1 - MSI)
#define APLIC_DOMAINCFG_DM_MASK     (0x1) // domaincfg[2:2] Delivery Mode (0 - direct, 1 - MSI)
#define APLIC_DOMAINCFG_DM_BIT      BIT(APLIC_DOMAINCFG_DM_SHIFT)
#define APLIC_DOMAINCFG_CM_SHIFT    (6)   // domaincfg[6:6] Compatibility Mode
#define APLIC_DOMAINCFG_CM_MASK     (0x1) // domaincfg[6:6] Compatibility Mode
#define APLIC_DOMAINCFG_CM_BIT      BIT(APLIC_DOMAINCFG_CM_SHIFT)
#define APLIC_DOMAINCFG_IE_SHIFT    (8)   // domaincfg[8:8] Interrupt Enable
#define APLIC_DOMAINCFG_IE_MASK     (0x1) // domaincfg[8:8] Interrupt Enable
#define APLIC_DOMAINCFG_IE_BIT      BIT(APLIC_DOMAINCFG_IE_SHIFT)
#define APLIC_DOMAINCFG_MAGIC       (0x80)
#define APLIC_DOMAINCFG_MAGIC_SHIFT (24)
#define APLIC_DOMAINCFG_MAGIC_MASK  (0xff)

static inline uint32_t aplic_get_domaincfg(size_t domain)
{
    return aplic_get(domain, APLIC_DOMAINCFG);
}

static inline void aplic_set_domaincfg(size_t domain, uint32_t domaincfg)
{
    aplic_set(domain, APLIC_DOMAINCFG, domaincfg);
}

#define APLIC_SOURCECFG_BASE (0x0004) // sourcecfg_base[1..1023]

#define APLIC_SOURCECFG_D_SHIFT (10)  // sourcecfg[10:10] Delegate
#define APLIC_SOURCECFG_D_MASK  (0x1) // sourcecfg[10:10] Delegate
#define APLIC_SOURCECFG_D_BIT   BIT(APLIC_SOURCECFG_D_SHIFT)

#define APLIC_SOURCECFG_CHILD_INDEX_SHIFT (0)     // sourcecfg[8:0] (if D==1)
#define APLIC_SOURCECFG_CHILD_INDEX_MASK  (0x1ff) // sourcecfg[8:0] (if D==1)

#define APLIC_SOURCECFG_SM_SHIFT    (0)   // sourcecfg[2:0] Source Mode (if D==0)
#define APLIC_SOURCECFG_SM_MASK     (0x7) // sourcecfg[2:0] Source Mode (if D==0)
#define APLIC_SOURCECFG_SM_INACTIVE (0)   // Inactive in this domain (and not delegated)
#define APLIC_SOURCECFG_SM_DETACH   (1)   // Active, detached from the source wire
#define APLIC_SOURCECFG_SM_EDGE1    (4)   // Active, edge-sensitive; asserted on rising edge
#define APLIC_SOURCECFG_SM_EDGE0    (5)   // Active, edge-sensitive; asserted on falling edge
#define APLIC_SOURCECFG_SM_LEVEL1   (6)   // Active, level-sensitive; asserted when high
#define APLIC_SOURCECFG_SM_LEVEL0   (7)   // Active, level-sensitive; asserted when low

static inline uint32_t aplic_get_sourcecfg(size_t domain, size_t src)
{
    return aplic_get(domain, APLIC_SOURCECFG_BASE + (src - 1) * sizeof(uint32_t));
}

static inline void aplic_set_sourcecfg(size_t domain, size_t src, uint32_t sourcecfg)
{
    aplic_set(domain, APLIC_SOURCECFG_BASE + (src - 1) * sizeof(uint32_t), sourcecfg);
}

static inline uint32_t aplic_sourcecfg_delegate(uint32_t sourcecfg)
{
    return ((sourcecfg >> APLIC_SOURCECFG_D_SHIFT) & APLIC_SOURCECFG_D_MASK);
}

static inline uint32_t aplic_sourcecfg_sm(uint32_t sourcecfg)
{
    return ((sourcecfg >> APLIC_SOURCECFG_SM_SHIFT) & APLIC_SOURCECFG_SM_MASK);
}

static inline uint32_t aplic_sourcecfg_child_index(uint32_t sourcecfg)
{
    return ((sourcecfg >> APLIC_SOURCECFG_CHILD_INDEX_SHIFT) & APLIC_SOURCECFG_CHILD_INDEX_MASK);
}

// Machine MSI address configuration (machine-level interrupt domains only)
#define APLIC_MMSIADDRCFG  (0x1BC0) // mmsiaddrcfg[31:0] Low Base PPN (Physical Page Number)
#define APLIC_MMSIADDRCFGH (0x1BC4) // mmsiaddrcfgh

#define APLIC_MMSIADDRCFGH_LOCKED_SHIFT (31)    // mmsiaddrcfgh[31] L (Locked) affects smsiaddrcfg(h) too
#define APLIC_MMSIADDRCFGH_LOCKED_MASK  (0x1)   // mmsiaddrcfgh[31] L (Locked)
#define APLIC_MMSIADDRCFGH_L_BIT        BIT(APLIC_MMSIADDRCFGH_LOCKED_SHIFT)
#define APLIC_MMSIADDRCFGH_HHXS_SHIFT   (24)    // mmsiaddrcfgh[28:24] HHXS (High Hart Index Shift)
#define APLIC_MMSIADDRCFGH_HHXS_MASK    (0x1f)  // mmsiaddrcfgh[28:24] HHXS (High Hart Index Shift)
#define APLIC_MMSIADDRCFGH_LHXS_SHIFT   (20)    // mmsiaddrcfgh[22:20] LHXS (Low Hart Index Shift)
#define APLIC_MMSIADDRCFGH_LHXS_MASK    (0x7)   // mmsiaddrcfgh[22:20] LHXS (Low Hart Index Shift)
#define APLIC_MMSIADDRCFGH_HHXW_SHIFT   (16)    // mmsiaddrcfgh[18:16] HHXW (High Hart Index Width)
#define APLIC_MMSIADDRCFGH_HHXW_MASK    (0x7)   // mmsiaddrcfgh[18:16] HHXW (High Hart Index Width)
#define APLIC_MMSIADDRCFGH_LHXW_SHIFT   (12)    // mmsiaddrcfgh[15:12] LHXW (Low Hart Index Width)
#define APLIC_MMSIADDRCFGH_LHXW_MASK    (0xF)   // mmsiaddrcfgh[15:12] LHXW (Low Hart Index Width)
#define APLIC_MMSIADDRCFGH_HBPPN_SHIFT  (0)     // mmsiaddrcfgh[11:0] High Base PPN (Physical Page Number)
#define APLIC_MMSIADDRCFGH_HBPPN_MASK   (0xfff) // mmsiaddrcfgh[11:0] High Base PPN (Physical Page Number)


#define IMSIC_MMIO_PAGE_SHIFT           (12)
#define APLIC_LHXW_M                    (3)
#define APLIC_LHXS_M                    (0)
#define APLIC_HHXW_M                    (0)
#define APLIC_HHXS_M                    (2 * IMSIC_MMIO_PAGE_SHIFT)
#define APLIC_xMSIADDRCFG_PPN_SHIFT     (12)

#define APLIC_xMSIADDRCFG_PPN_HART(__lhxs) \
        ((1UL << (__lhxs)) - 1)
#define APLIC_xMSIADDRCFG_PPN_LHX_MASK(__lhxw) \
        ((1UL << (__lhxw)) - 1)
#define APLIC_xMSIADDRCFG_PPN_LHX_SHIFT(__lhxs) \
        ((__lhxs))
#define APLIC_xMSIADDRCFG_PPN_LHX(__lhxw, __lhxs) \
        (APLIC_xMSIADDRCFG_PPN_LHX_MASK(__lhxw) << \
         APLIC_xMSIADDRCFG_PPN_LHX_SHIFT(__lhxs))

#define APLIC_xMSIADDRCFG_PPN_HHX_MASK(__hhxw) \
        ((1UL << (__hhxw)) - 1)
#define APLIC_xMSIADDRCFG_PPN_HHX_SHIFT(__hhxs) \
        ((__hhxs) + APLIC_xMSIADDRCFG_PPN_SHIFT)
#define APLIC_xMSIADDRCFG_PPN_HHX(__hhxw, __hhxs) \
        (APLIC_xMSIADDRCFG_PPN_HHX_MASK(__hhxw) << \
         APLIC_xMSIADDRCFG_PPN_HHX_SHIFT(__hhxs))

struct aplic_msicfg_data {
    unsigned long lhxs;
    unsigned long lhxw;
    unsigned long hhxs;
    unsigned long hhxw;
    unsigned long base_addr;
};

static inline void aplic_write_msiconfig(struct aplic_msicfg_data *msicfg,
                                         size_t msicfgaddr, size_t msicfgaddrh)
{
    uint32_t val;
    unsigned long base_ppn;

    /* Check if MSI config is already locked */
    if (readl(msicfgaddrh) & (APLIC_MMSIADDRCFGH_LOCKED_MASK << APLIC_MMSIADDRCFGH_LOCKED_SHIFT))
        return;

    base_ppn = msicfg->base_addr >> APLIC_xMSIADDRCFG_PPN_SHIFT;
    base_ppn &= ~APLIC_xMSIADDRCFG_PPN_HART(msicfg->lhxs);
    base_ppn &= ~APLIC_xMSIADDRCFG_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
    base_ppn &= ~APLIC_xMSIADDRCFG_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

    writel((uint32_t)base_ppn, msicfgaddr);

    val = (((uint64_t)base_ppn) >> 32) &
        APLIC_MMSIADDRCFGH_HBPPN_MASK;
    val |= (msicfg->lhxw & APLIC_MMSIADDRCFGH_LHXW_MASK)
        << APLIC_MMSIADDRCFGH_LHXW_SHIFT;
    val |= (msicfg->hhxw & APLIC_MMSIADDRCFGH_HHXW_MASK)
        << APLIC_MMSIADDRCFGH_HHXW_SHIFT;
    val |= (msicfg->lhxs & APLIC_MMSIADDRCFGH_LHXS_MASK)
        << APLIC_MMSIADDRCFGH_LHXS_SHIFT;
    val |= (msicfg->hhxs & APLIC_MMSIADDRCFGH_HHXS_MASK)
        << APLIC_MMSIADDRCFGH_HHXS_SHIFT;

    writel(val, msicfgaddrh);
}



static inline uint32_t aplic_get_mmsiaddrcfg(size_t domain)
{
    return aplic_get(domain, APLIC_MMSIADDRCFG);
}

static inline void aplic_set_mmsiaddrcfg(size_t domain, uint32_t mmsiaddrcfg)
{
    aplic_set(domain, APLIC_MMSIADDRCFG, mmsiaddrcfg);
}

static inline uint32_t aplic_get_mmsiaddrcfgh(size_t domain)
{
    return aplic_get(domain, APLIC_MMSIADDRCFGH);
}

static inline void aplic_set_mmsiaddrcfgh(size_t domain, uint32_t mmsiaddrcfgh)
{
    aplic_set(domain, APLIC_MMSIADDRCFGH, mmsiaddrcfgh);
}

// Supervisor MSI address configuration (machine-level interrupt domains only)
#define APLIC_SMSIADDRCFG  (0x1BC8) // smsiaddrcfg[31:0] Low Base PPN (Physical Page Number)
#define APLIC_SMSIADDRCFGH (0x1BCC) // smsiaddrcfgh

#define APLIC_SMSIADDRCFGH_LHXS_SHIFT  (20)    // smsiaddrcfgh[22:20] LHXS (Low Hart Index Shift)
#define APLIC_SMSIADDRCFGH_LHXS_MASK   (0x7)   // smsiaddrcfgh[22:20] LHXS (Low Hart Index Shift)
#define APLIC_SMSIADDRCFGH_HBPPN_SHIFT (0)     // smsiaddrcfgh[11:0] High Base PPN (Physical Page Number)
#define APLIC_SMSIADDRCFGH_HBPPN_MASK  (0xfff) // smsiaddrcfgh[11:0] High Base PPN (Physical Page Number)

static inline uint32_t aplic_get_smsiaddrcfg(size_t domain)
{
    return aplic_get(domain, APLIC_SMSIADDRCFG);
}

static inline void aplic_set_smsiaddrcfg(size_t domain, uint32_t smsiaddrcfg)
{
    aplic_set(domain, APLIC_SMSIADDRCFG, smsiaddrcfg);
}

static inline uint32_t aplic_get_smsiaddrcfgh(size_t domain)
{
    return aplic_get(domain, APLIC_SMSIADDRCFGH);
}

static inline void aplic_set_smsiaddrcfgh(size_t domain, uint32_t smsiaddrcfgh)
{
    aplic_set(domain, APLIC_SMSIADDRCFGH, smsiaddrcfgh);
}

#define APLIC_MIN_IRQ_REG (0)  // relates to setip[], in_clrip[], setie[], clrie[]
#define APLIC_MAX_IRQ_REG (31) // relates to setip[], in_clrip[], setie[], clrie[]
#define APLIC_MAX_IMPL_IRQ_REG  (AIA_REG32_WORD_OFFSET(APLIC_SOURCE_MAX))

#define APLIC_SETIP_BASE (0x1c00) // setip[0..31] Set interrupt-pending bit
#define APLIC_SETIPNUM   (0x1cdc) // setipnum Set interrupt-pending bit by number (1..1023)

static inline uint32_t aplic_get_setip(size_t domain, size_t reg_num)
{
    return aplic_get(domain, APLIC_SETIP_BASE + reg_num * sizeof(uint32_t));
}

static inline void aplic_set_setip(size_t domain, size_t reg_num, uint32_t value)
{
    aplic_set(domain, APLIC_SETIP_BASE + reg_num * sizeof(uint32_t), value);
}

static inline uint32_t aplic_get_setipnum(size_t domain, size_t src)
{
    return (aplic_get(domain, APLIC_SETIP_BASE + AIA_REG32_BYTE_OFFSET(src))
                >> AIA_REG32_BIT_OFFSET(src)) & 1;
}

static inline void aplic_set_setipnum(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_SETIPNUM, src);
}

#define APLIC_IN_CLRIP_BASE (0x1d00) // in_clrip[0..31]
#define APLIC_CLRIPNUM      (0x1ddc) // clripnum

static inline uint32_t aplic_get_clrip(size_t domain, size_t reg_num)
{
    return aplic_get(domain, APLIC_IN_CLRIP_BASE + reg_num * sizeof(uint32_t));
}

static inline void aplic_set_clrip(size_t domain, size_t reg_num, uint32_t value)
{
    aplic_set(domain, APLIC_IN_CLRIP_BASE + reg_num * sizeof(uint32_t), value);
}

static inline uint32_t aplic_get_clripnum(size_t domain, size_t src)
{
    return (aplic_get(domain, APLIC_IN_CLRIP_BASE + AIA_REG32_BYTE_OFFSET(src))
                >> AIA_REG32_BIT_OFFSET(src)) & 1;
}

static inline void aplic_set_clripnum(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_CLRIPNUM, src);
}

#define APLIC_SETIE_BASE (0x1e00) // setie[0..31]
#define APLIC_SETIENUM   (0x1edc) // setienum

static inline uint32_t aplic_get_setie(size_t domain, size_t reg_num)
{
    return aplic_get(domain, APLIC_SETIE_BASE + reg_num * sizeof(uint32_t));
}

static inline void aplic_set_setie(size_t domain, size_t reg_num, uint32_t value)
{
    aplic_set(domain, APLIC_SETIE_BASE + reg_num * sizeof(uint32_t), value);
}

static inline uint32_t aplic_get_setienum(size_t domain, size_t src)
{
    return (aplic_get(domain, APLIC_SETIE_BASE + AIA_REG32_BYTE_OFFSET(src))
                >> AIA_REG32_BIT_OFFSET(src)) & 1;
}

static inline void aplic_set_setienum(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_SETIENUM, src);
}

#define APLIC_CLRIE_BASE (0x1f00) // clrie[0..31]
#define APLIC_CLRIENUM   (0x1fdc) // clrienum

static inline uint32_t aplic_get_clrie(size_t domain, size_t reg_num)
{
    return aplic_get(domain, APLIC_CLRIE_BASE + reg_num * sizeof(uint32_t));
}

static inline void aplic_set_clrie(size_t domain, size_t reg_num, uint32_t value)
{
    aplic_set(domain, APLIC_CLRIE_BASE + reg_num * sizeof(uint32_t), value);
}

static inline uint32_t aplic_get_clrienum(size_t domain, size_t src)
{
    return (aplic_get(domain, APLIC_CLRIE_BASE + AIA_REG32_BYTE_OFFSET(src))
                >> AIA_REG32_BIT_OFFSET(src)) & 1;
}

static inline void aplic_set_clrienum(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_CLRIENUM, src);
}

#define APLIC_SETIPNUM_LE (0x2000) // setipnum le (always little-endian)
#define APLIC_SETIPNUM_BE (0x2004) // setipnum be (always big-endian)

static inline void aplic_set_setipnum_le(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_SETIPNUM_LE, src);
}

static inline void aplic_set_setipnum_be(size_t domain, size_t src)
{
    aplic_set(domain, APLIC_SETIPNUM_BE, src);
}

#define APLIC_GENMSI (0x3000) // Generate MSI

#define APLIC_GENMSI_HART_IDX_SHIFT (18) // genmsi[31:18] Hart index
#define APLIC_GENMSI_HART_IDX_MASK  (0x3fff)
#define APLIC_GENMSI_BUSY_SHIFT     (12) // genmsi[12:12] Busy (read only)
#define APLIC_GENMSI_BUSY_MASK      (0x1)
#define APLIC_GENMSI_EIID_SHIFT     (0)     // genmsi[10:0]  Ext Interrupt Identity
#define APLIC_GENMSI_EIID_MASK      (0x7ff) // genmsi[10:0]  Ext Interrupt Identity

static inline uint32_t aplic_get_genmsi(size_t domain)
{
    return aplic_get(domain, APLIC_GENMSI);
}

static inline void aplic_set_genmsi(size_t domain, uint32_t genmsi)
{
    aplic_set(domain, APLIC_GENMSI, genmsi);
}

#define APLIC_TARGET_BASE (0x3004) // target_base[1..1023]

#define APLIC_TARGET_HART_IDX_SHIFT  (18)    // target[31:18] Hart index
#define APLIC_TARGET_HART_IDX_MASK   (0x3fff)
#define APLIC_TARGET_IPRIO_SHIFT     (0)     // target[7:0]   Priority (if DM==0)
#define APLIC_TARGET_IPRIO_MASK      (0xff)  // target[7:0]   Priority (if DM==0)
#define APLIC_TARGET_GUEST_IDX_SHIFT (12)    // target[17:12] Guest index (if DM==1)
#define APLIC_TARGET_GUEST_IDX_MASK  (0x3f)  // target[17:12] Guest index (if DM==1)
#define APLIC_TARGET_EIID_SHIFT      (0)     // target[10:0]  Ext Interrupt Identity (if DM==1)
#define APLIC_TARGET_EIID_MASK       (0x7ff) // target[10:0]  Ext Interrupt Identity (if DM==1)

static inline uint32_t aplic_make_target(uint32_t hart_idx, uint32_t guest_idx, uint32_t eiid)
{
    return ((hart_idx  & APLIC_TARGET_HART_IDX_MASK)  << APLIC_TARGET_HART_IDX_SHIFT)  |
           ((guest_idx & APLIC_TARGET_GUEST_IDX_MASK) << APLIC_TARGET_GUEST_IDX_SHIFT) |
           ((eiid      & APLIC_TARGET_EIID_MASK)      << APLIC_TARGET_EIID_SHIFT);
}

static inline uint32_t aplic_get_target(size_t domain, size_t src)
{
    return aplic_get(domain, APLIC_TARGET_BASE + (src - 1) * sizeof(uint32_t));
}

static inline void aplic_set_target(size_t domain, size_t src, uint32_t target)
{
    aplic_set(domain, APLIC_TARGET_BASE + (src - 1) * sizeof(uint32_t), target);
}

// Interrupt delivery control (IDC)

#define APLIC_IDC_BASE (0x4000)

#define APLIC_IDC_IDELIVERY  (0x00)
#define APLIC_IDC_IFORCE     (0x04)
#define APLIC_IDC_ITHRESHOLD (0x08)

#define APLIC_IDC_TOPI            (0x18)
#define APLIC_IDC_TOPI_ID_SHIFT   (16) // topi[25:16]
#define APLIC_IDC_TOPI_ID_MASK    (0x3ff)
#define APLIC_IDC_TOPI_PRIO_SHIFT (0) // topi[7:0]
#define APLIC_IDC_TOPI_PRIO_MASK  (0xff)

#define APLIC_IDC_CLAIMI (0x1c) // format is the same as topi

typedef struct aplic_idc {
    uint32_t idelivery; // Interrupt delivery enable
    uint32_t iforce;    // Interrupt force
    uint32_t itreshold; // Interrupt enable threshold (minimum interrupt priority)
    uint32_t _pad[3];
    uint32_t topi;      // Top interrupt (ro)
    uint32_t claimi;    // Claim top interrupt (ro)
} aplic_idc_t;

static inline volatile aplic_idc_t *aplic_get_idc_ptr(size_t domain, size_t hart)
{
    return (volatile aplic_idc_t *)(APLIC_BASE_ADDR(domain) + APLIC_IDC_BASE
                                        + sizeof(aplic_idc_t) * hart);
}

static inline uint32_t aplic_get_idc_idelivery(size_t domain)
{
    return aplic_get(domain, APLIC_IDC_BASE + APLIC_IDC_IDELIVERY);
}

static inline void aplic_set_idc_idelivery(size_t domain, uint32_t topi)
{
    aplic_set(domain, APLIC_IDC_BASE + APLIC_IDC_IDELIVERY, topi);
}

static inline uint32_t aplic_get_idc_iforce(size_t domain)
{
    return aplic_get(domain, APLIC_IDC_BASE + APLIC_IDC_IFORCE);
}

static inline uint32_t aplic_get_idc_itreshold(size_t domain)
{
    return aplic_get(domain, APLIC_IDC_BASE + APLIC_IDC_ITHRESHOLD);
}

static inline uint32_t aplic_get_idc_topi(size_t domain)
{
    return aplic_get(domain, APLIC_IDC_BASE + APLIC_IDC_TOPI);
}

static inline uint32_t aplic_idc_topi_identity(uint32_t topi)
{
    return ((topi >> APLIC_IDC_TOPI_ID_SHIFT) & APLIC_IDC_TOPI_ID_MASK);
}

static inline uint32_t aplic_idc_topi_priority(uint32_t topi)
{
    return (topi & APLIC_IDC_TOPI_PRIO_MASK);
}

static inline uint32_t aplic_get_idc_claimi(size_t domain)
{
    return aplic_get(domain, APLIC_IDC_BASE + APLIC_IDC_CLAIMI);
}

// IMSIC definitions

#define IMSIC_M_BASE_ADDR (PLF_IMSIC_BASE_M) // Machine level domain
#define IMSIC_S_BASE_ADDR (PLF_IMSIC_BASE_S) // Supervisor level domain
#define IMSIC_INT_FILE_OFFSET   (0x1000)

#define IMSIC_M (0) // Machine level domain
#define IMSIC_S (1) // Supervisor level domain
#define IMSIC_VS (2) // Virtual supervisor level domain

#define IMSIC_BASE_ADDR(domain, core)                                  \
    ((((domain) == IMSIC_S) ? IMSIC_S_BASE_ADDR : IMSIC_M_BASE_ADDR) + \
        (core) * IMSIC_INT_FILE_OFFSET)

#define IMSIC_MAX_NUM_MSI       (2047) // Max capable value from the spec
#define IMSIC_MIN_MSI           (1)
#define IMSIC_NUM_MSI           (PLF_IMSIC_NUM_IDS)
#define IMSIC_NUM_MSI_MASK      (IMSIC_NUM_MSI)

#define IMSIC_REG32_STEP                   (1)
#define IMSIC_REG64_STEP                   (2)
#define IMSIC_REG32_WORD_OFFSET(src)       ((src >> AIA_REG32_POW2_BITS_IN_REG) * IMSIC_REG32_STEP)
#define IMSIC_REG64_WORD_OFFSET(src)       ((src >> AIA_REG64_POW2_BITS_IN_REG) * IMSIC_REG64_STEP)

#if __riscv_xlen == 64
#define IMSIC_REG_STEP              (IMSIC_REG64_STEP)
#define IMSIC_REG_MAX               (IMSIC_REG64_WORD_OFFSET(IMSIC_NUM_MSI))
#define IMSIC_REG_ALL_BITS_MASK     (0xFFFFFFFFFFFFFFFFULL)
#define IMSIC_REG_WORD_OFFSET(src)  (IMSIC_REG64_WORD_OFFSET(src))
#else /* __riscv_xlen == 64 */
#define IMSIC_REG_STEP              (IMSIC_REG32_STEP)
#define IMSIC_REG_MAX               (IMSIC_REG32_WORD_OFFSET(IMSIC_NUM_MSI))
#define IMSIC_REG_ALL_BITS_MASK     (0xFFFFFFFF)
#define IMSIC_REG_WORD_OFFSET(src)  (IMSIC_REG32_WORD_OFFSET(src))
#endif /* __riscv_xlen == 64 */

static inline uint32_t imsic_get(size_t domain, size_t core, size_t reg)
{
    return readl(IMSIC_BASE_ADDR(domain, core) + reg);
}

static inline void imsic_set(size_t domain, size_t core, size_t reg, uint32_t value)
{
    writel(value, IMSIC_BASE_ADDR(domain, core) + reg);
}

// IMSIC registers (with direct access) for using with imsic_get() and imsic_set()

#define IMSIC_SETEIPNUM_LE (0x00) // Set External Interrupt-Pending bit by Number (LE)
#define IMSIC_SETEIPNUM_BE (0x04) // Set External Interrupt-Pending bit by Number (BE)

static inline uint32_t imsic_get_seteipnum_le(size_t domain, size_t core)
{
    return imsic_get(domain, core, IMSIC_SETEIPNUM_LE);
}

static inline void imsic_set_seteipnum_le(size_t domain, size_t core, uint32_t seteipnum_le)
{
    imsic_set(domain, core, IMSIC_SETEIPNUM_LE, seteipnum_le);
}

static inline uint32_t imsic_get_seteipnum_be(size_t domain, size_t core)
{
    return imsic_get(domain, core, IMSIC_SETEIPNUM_BE);
}

static inline void imsic_set_seteipnum_be(size_t domain, size_t core, uint32_t seteipnum_be)
{
    imsic_set(domain, core, IMSIC_SETEIPNUM_BE, seteipnum_be);
}

// IMSIC CSRs - for using with READ_CSR() and WRITE_CSR()

// Machine-Level Window to Indirectly Accessed Registers
#define IMSIC_CSR_MISELECT (0x350) // Machine indirect register select
#define IMSIC_CSR_MIREG    (0x351) // Machine indirect register alias
// Machine-Level Interrupts
#define IMSIC_CSR_MTOPEI   (0x35C) // Machine top external interrupt (only with an IMSIC)
#define IMSIC_CSR_MTOPI    (0xFB0) // Machine top interrupt (RO)
// Virtual Interrupts for Supervisor Level
#define IMSIC_CSR_MVIEN    (0x308) // Machine virtual interrupt enables
#define IMSIC_CSR_MVIP     (0x309) // Machine virtual interrupt-pending bits
// Machine-Level High-Half CSRs for Interrupts 32–63 (RV32 only)
#define IMSIC_CSR_MIDELEGH (0x313) // Extension of mideleg (only with S-mode)
#define IMSIC_CSR_MIEH     (0x314) // Extension of mie
#define IMSIC_CSR_MVIENH   (0x318) // Extension of mvien (only with S-mode)
#define IMSIC_CSR_MVIPH    (0x319) // Extension of mvip (only with S-mode)
#define IMSIC_CSR_MIPH     (0x354) // Extension of mip

// Supervisor-Level Window to Indirectly Accessed Registers
#define IMSIC_CSR_SISELECT (0x150) // Supervisor indirect register select
#define IMSIC_CSR_SIREG    (0x151) // Supervisor indirect register alias
// Supervisor-Level Interrupts
#define IMSIC_CSR_STOPEI   (0x15C) // Supervisor top external interrupt (only with an IMSIC)
#define IMSIC_CSR_STOPI    (0xDB0) // Supervisor top interrupt (RO)
// Supervisor-Level High-Half CSRs for Interrupts 32–63 (RV32 only)
#define IMSIC_CSR_SIEH     (0x114) // Extension of sie
#define IMSIC_CSR_SIPH     (0x154) // Extension of sip

// Virual supervisor-Level Window to Indirectly Accessed Registers
#define IMSIC_CSR_VSISELECT (0x250) // Virual supervisor indirect register select
#define IMSIC_CSR_VSIREG    (0x251) // Virual supervisor indirect register alias

#if __riscv_xlen == 64
static inline uint64_t aia_get_topi(size_t priv_level)
#else
static inline uint32_t aia_get_topi(size_t priv_level)
#endif
{
    if (priv_level == IMSIC_M) {
        return read_csr(IMSIC_CSR_MTOPI);
    } else {
        return read_csr(IMSIC_CSR_STOPI);
    }
}

#if __riscv_xlen == 64
static inline uint64_t aia_get_topei(size_t priv_level)
#else
static inline uint32_t aia_get_topei(size_t priv_level)
#endif
{
    if (priv_level == IMSIC_M) {
        return read_csr(IMSIC_CSR_MTOPEI);
    } else {
        return read_csr(IMSIC_CSR_STOPEI);
    }
}

static inline unsigned long imsic_read(size_t domain, uint32_t reg)
{
    if (domain == IMSIC_VS) {
        write_csr(IMSIC_CSR_VSISELECT, reg);
        return read_csr(IMSIC_CSR_VSIREG);
    } else if (domain == IMSIC_S) {
        write_csr(IMSIC_CSR_SISELECT, reg);
        return read_csr(IMSIC_CSR_SIREG);
    } else { // IMSIC_M
        write_csr(IMSIC_CSR_MISELECT, reg);
        return read_csr(IMSIC_CSR_MIREG);
    }
}

static inline void imsic_write(size_t domain, uint32_t reg, unsigned long val)
{
    if (domain == IMSIC_VS) {
        write_csr(IMSIC_CSR_VSISELECT, reg);
        write_csr(IMSIC_CSR_VSIREG, val);
    } else if (domain == IMSIC_S) {
        write_csr(IMSIC_CSR_SISELECT, reg);
        write_csr(IMSIC_CSR_SIREG, val);
    } else { // IMSIC_M
        write_csr(IMSIC_CSR_MISELECT, reg);
        write_csr(IMSIC_CSR_MIREG, val);
    }
}

static inline void imsic_set_bits(size_t domain, uint32_t reg, unsigned long val)
{
    if (domain == IMSIC_VS) {
        write_csr(IMSIC_CSR_VSISELECT, reg);
        set_csr(IMSIC_CSR_VSIREG, val);
    } else if (domain == IMSIC_S) {
        write_csr(IMSIC_CSR_SISELECT, reg);
        set_csr(IMSIC_CSR_SIREG, val);
    } else { // IMSIC_M
        write_csr(IMSIC_CSR_MISELECT, reg);
        set_csr(IMSIC_CSR_MIREG, val);
    }
}

static inline void imsic_clr_bits(size_t domain, uint32_t reg, unsigned long val)
{
    if (domain == IMSIC_VS) {
        write_csr(IMSIC_CSR_VSISELECT, reg);
        clear_csr(IMSIC_CSR_VSIREG, val);
    } else if (domain == IMSIC_S) {
        write_csr(IMSIC_CSR_SISELECT, reg);
        clear_csr(IMSIC_CSR_SIREG, val);
    } else { // IMSIC_M
        write_csr(IMSIC_CSR_MISELECT, reg);
        clear_csr(IMSIC_CSR_MIREG, val);
    }
}

// mtopei/stopei/vstopei
#define IMSIC_XTOPEI_ID_SHIFT   (16) // xtopei[26:16]
#define IMSIC_XTOPEI_ID_MASK    (0x7ff)
#define IMSIC_XTOPEI_PRIO_SHIFT (0) // xtopei[10:0]
#define IMSIC_XTOPEI_PRIO_MASK  (0x7ff)

static inline uint64_t imsic_topei_identity(uint64_t topei)
{
    return ((topei >> IMSIC_XTOPEI_ID_SHIFT) & IMSIC_XTOPEI_ID_MASK);
}

static inline uint64_t imsic_topei_priority(uint64_t topei)
{
    return (topei & IMSIC_XTOPEI_PRIO_MASK);
}

// mtopi/stopi/vstopi
#define IMSIC_XTOPI_ID_SHIFT   (16) // xtopi[27:16]
#define IMSIC_XTOPI_ID_MASK    (0xfff)
#define IMSIC_XTOPI_PRIO_SHIFT (0) // xtopi[7:0])
#define IMSIC_XTOPI_PRIO_MASK  (0xff)

static inline uint64_t imsic_topi_identity(uint64_t topi)
{
    return ((topi >> IMSIC_XTOPI_ID_SHIFT) & IMSIC_XTOPI_ID_MASK);
}

static inline uint64_t imsic_topi_priority(uint64_t topi)
{
    return (topi & IMSIC_XTOPI_PRIO_MASK);
}

// IMSIC indirect registers (with miselect access) for using with imsic_read() and imsic_write()

#define IMSIC_IPRIO_BASE  (0x30) // Priorities for major interrupts
#define IMSIC_EIDELIVERY  (0x70) // External interrupt delivery enable
#define IMSIC_EITHRESHOLD (0x72) // External interrupt enable threshold
#define IMSIC_EIP_BASE    (0x80) // eip[0..63] External interrupt-pending
#define IMSIC_EIE_BASE    (0xC0) // eie[0..63] External interrupt-enable

#define IMSIC_IPRIO_REG_OFFSET(interrupt) \
                                (((interrupt) >> (AIA_REG_POW2_BYTES_IN_REG)) * (IMSIC_REG_STEP))

#define IMSIC_IPRIO_REG(interrupt)      ((IMSIC_IPRIO_BASE) + IMSIC_IPRIO_REG_OFFSET(interrupt))
#define IMSIC_IPRIO_SHIFT(interrupt)    (((interrupt) & (AIA_REG_BYTES_IN_REG - 1)) * 8)
#define IMSIC_IPRIO_MASK                (0xFFULL)

#define AIA_PRIO_MAX                    MIN(IMSIC_NUM_MSI, 255)

#define AIA_IPRIO_REG_NUM       (2)
#if __riscv_xlen == 64
#define AIA_IPRIO_REG_MAX       (14)
#define AIA_IPRIO_REG_STEP      (2)
#else
#define AIA_IPRIO_REG_MAX       (15)
#define AIA_IPRIO_REG_STEP      (1)
#endif

static inline size_t aia_iprio_value_set(size_t value, size_t interrupt, size_t prio)
{
    value &= ~(IMSIC_IPRIO_MASK << IMSIC_IPRIO_SHIFT(interrupt));
    value |= ((prio & IMSIC_IPRIO_MASK) << IMSIC_IPRIO_SHIFT(interrupt));

    return value;
}

static inline size_t aia_iprio_value_clear(size_t value, size_t interrupt)
{
    value &= ~(IMSIC_IPRIO_MASK << IMSIC_IPRIO_SHIFT(interrupt));

    return value;
}

static inline void imsic_write_iprio(size_t domain, size_t interrupt, size_t prio)
{
    size_t value = aia_iprio_value_set(
                            imsic_read(domain, IMSIC_IPRIO_REG(interrupt)),
                            interrupt, prio);

    imsic_write(domain, IMSIC_IPRIO_REG(interrupt), value);
}

static inline size_t imsic_read_iprio_reg(size_t domain, size_t reg)
{
    return imsic_read(domain, IMSIC_IPRIO_BASE + reg);
}

static inline void imsic_write_iprio_reg(size_t domain, size_t reg, size_t value)
{
    imsic_write(domain, IMSIC_IPRIO_BASE + reg, value);
}

static inline size_t imsic_get_iprio(size_t domain, size_t interrupt)
{
    size_t value = imsic_read(domain, IMSIC_IPRIO_REG(interrupt));

    return (value >> IMSIC_IPRIO_SHIFT(interrupt)) & IMSIC_IPRIO_MASK;
}

static inline void imsic_set_iprio(size_t domain, size_t interrupt, size_t prio)
{
    size_t value = (prio & IMSIC_IPRIO_MASK) << IMSIC_IPRIO_SHIFT(interrupt);

    imsic_set_bits(domain, IMSIC_IPRIO_REG(interrupt), value);
}

static inline void imsic_clr_iprio(size_t domain, size_t interrupt)
{
    size_t value = IMSIC_IPRIO_MASK << IMSIC_IPRIO_SHIFT(interrupt);

    imsic_clr_bits(domain, IMSIC_IPRIO_REG(interrupt), value);
}

static inline unsigned long imsic_read_eip(size_t domain, size_t reg)
{
    return imsic_read(domain, IMSIC_EIP_BASE + reg);
}

static inline void imsic_write_eip(size_t domain, size_t reg, unsigned long val)
{
    return imsic_write(domain, IMSIC_EIP_BASE + reg, val);
}

static inline uint32_t imsic_get_eipnum(size_t domain, size_t irq_id)
{
#if __riscv_xlen == 64
    return (imsic_read(domain, IMSIC_EIP_BASE + IMSIC_REG64_WORD_OFFSET(irq_id))
                >> AIA_REG64_BIT_OFFSET(irq_id)) & 1;
#else  /* __riscv_xlen == 64 */
    return (imsic_read(domain, IMSIC_EIP_BASE + IMSIC_REG32_WORD_OFFSET(irq_id))
                >> AIA_REG32_BIT_OFFSET(irq_id)) & 1;
#endif /* __riscv_xlen == 64 */
}

static inline void imsic_set_eipnum(size_t domain, size_t irq_id)
{
    size_t reg = IMSIC_REG_WORD_OFFSET(irq_id);
    size_t bit = AIA_REG_BIT_OFFSET(irq_id);
    imsic_set_bits(domain, IMSIC_EIP_BASE + reg, 1ul << bit);
}

static inline void imsic_clr_eipnum(size_t domain, size_t irq_id)
{
    size_t reg = IMSIC_REG_WORD_OFFSET(irq_id);
    size_t bit = AIA_REG_BIT_OFFSET(irq_id);
    imsic_clr_bits(domain, IMSIC_EIP_BASE + reg, 1ul << bit);
}

static inline unsigned long imsic_read_eie(size_t domain, size_t reg)
{
    return imsic_read(domain, IMSIC_EIE_BASE + reg);
}

static inline void imsic_write_eie(size_t domain, size_t reg, unsigned long val)
{
    return imsic_write(domain, IMSIC_EIE_BASE + reg, val);
}

static inline uint32_t imsic_get_eienum(size_t domain, size_t irq_id)
{
#if __riscv_xlen == 64
    return (imsic_read(domain, IMSIC_EIE_BASE + IMSIC_REG64_WORD_OFFSET(irq_id))
                >> AIA_REG64_BIT_OFFSET(irq_id)) & 1;
#else  /* __riscv_xlen == 64 */
    return (imsic_read(domain, IMSIC_EIE_BASE + IMSIC_REG32_WORD_OFFSET(irq_id))
                >> AIA_REG32_BIT_OFFSET(irq_id)) & 1;
#endif /* __riscv_xlen == 64 */
}

static inline void imsic_set_eienum(size_t domain, size_t irq_id)
{
    size_t reg = IMSIC_REG_WORD_OFFSET(irq_id);
    size_t bit = AIA_REG_BIT_OFFSET(irq_id);
    imsic_set_bits(domain, IMSIC_EIE_BASE + reg, 1ul << bit);
}

static inline void imsic_clr_eienum(size_t domain, size_t irq_id)
{
    size_t reg = IMSIC_REG_WORD_OFFSET(irq_id);
    size_t bit = AIA_REG_BIT_OFFSET(irq_id);
    imsic_clr_bits(domain, IMSIC_EIE_BASE + reg, 1ul << bit);
}

/* misc helpers */
#define BIT(nr) (1ULL << (nr))

#define SIE_UEIE (1ULL << 8)
#define SIE_SEIE (1ULL << 9)

#define SIP_USIP SIE_USIE
#define SIP_UTIP SIE_UTIE
#define SIP_UEIP SIE_UEIE

#define GLOBAL_INTERRUPT_EN         (1 << 3)  ///< MIE
#define SUPERVISOR_INTERRUPT_EN BIT(1) // SIE

#define IRQ_S_SOFT   (1)
#define IRQ_H_SOFT   (2)
#define IRQ_M_SOFT   (3)
#define IRQ_S_TIMER  (5)
#define IRQ_H_TIMER  (6)
#define IRQ_M_TIMER  (7)
#define IRQ_S_EXT    (9)
#define IRQ_H_EXT    (10)
#define IRQ_M_EXT    (11)
#define IRQ_COP      (12)
#define IRQ_HOST     (13)

#define MIP_SSIP            (1 << IRQ_S_SOFT)
#define MIP_HSIP            (1 << IRQ_H_SOFT)
#define MIP_MSIP            (1 << IRQ_M_SOFT)
#define MIP_STIP            (1 << IRQ_S_TIMER)
#define MIP_HTIP            (1 << IRQ_H_TIMER)
#define MIP_MTIP            (1 << IRQ_M_TIMER)
#define MIP_SEIP            (1 << IRQ_S_EXT)
#define MIP_HEIP            (1 << IRQ_H_EXT)
#define MIP_MEIP            (1 << IRQ_M_EXT)

#ifndef MIE_SSIE
#define MIE_SSIE (MIP_SSIP)
#endif

#ifndef SIE_SSIE
#define SIE_SSIE (MIP_SSIP)
#endif

#ifndef MIE_HSIE
#define MIE_HSIE (MIP_HSIP)
#endif

#ifndef MIE_MSIE
#define MIE_MSIE (MIP_MSIP)
#endif

#ifndef MIE_STIE
#define MIE_STIE (MIP_STIP)
#endif

#ifndef SIE_STIE
#define SIE_STIE (MIP_STIP)
#endif

#ifndef MIE_HTIE
#define MIE_HTIE (MIP_HTIP)
#endif

#ifndef MIE_SEIE
#define MIE_SEIE (MIP_SEIP)
#endif

#ifndef MIE_HEIE
#define MIE_HEIE (MIP_HEIP)
#endif

#ifndef MIE_MEIE
#define MIE_MEIE (MIP_MEIP)
#endif

void aia_init(void);
int aia_irq_setup(int line, int mode);
void aia_irq_enable(int irq);
void aia_irq_disable(int irq);
void aia_eoi(int line);
int aia_soi(void);

#endif // PLF_APLIC_BASE_M

#endif // SCR_BSP_AIA_H

