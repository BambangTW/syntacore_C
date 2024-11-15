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
/// @brief Cache routines
/// Syntacore SCR infra
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

#include "drivers/cache.h"
#include <stdint.h>
#include <stdio.h>

#ifdef PLF_CACHE_CFG
static int cacheinfo2str(char* buf, size_t len, unsigned long info)
{
    const unsigned banks   = (unsigned)(1 + ((info >> 16) & 0xf));
    const unsigned ways    = 1U << (info & 0x7);
    const unsigned linesz  = 1U << ((info >> 4) & 0xf);
    const unsigned lines   = 1U << ((info >> 8) & 0x1f);
    const unsigned size_kb = (lines * linesz * ways * banks) / 1024;

    return snprintf(buf, len, "%uK, %u-ways, %u-bytes line", size_kb, ways, linesz);
}
#endif // PLF_CACHE_CFG

static bool cache_l1_available(void) { return read_csr(SCR_CSR_CACHE_DSCR_L1) != 0; }

bool plf_l1cache_is_enabled(void)
{
    return (cache_l1_available()) ? (read_csr(SCR_CSR_CACHE_GLBL) & CACHE_GLBL_ENABLE) != 0 : false;
}

void plf_l1cache_enable(void)
{
#ifdef PLF_CACHE_CFG
    if (cache_l1_available())
    {
        write_csr(SCR_CSR_CACHE_GLBL, PLF_CACHE_CFG);
        ifence();
    }
#endif // PLF_CACHE_CFG
}

void plf_l1cache_disable(void)
{
#ifdef PLF_CACHE_CFG
    if (cache_l1_available())
    {
        write_csr(SCR_CSR_CACHE_GLBL, CACHE_GLBL_DISABLE | CACHE_GLBL_INV);
        ifence();
        // wait until invalidation complete
        while (read_csr(SCR_CSR_CACHE_GLBL) & (CACHE_GLBL_INV)) fence();
    }
#endif // PLF_CACHE_CFG
}

void plf_l1cache_init(void) { plf_l1cache_enable(); }

void cache_l1_invalidate(void* vaddr, long size)
{
#ifdef PLF_CACHE_CFG
    fence();

    if (size)
    {
        register uintptr_t a0 = (uintptr_t)vaddr;
        /* take in account alignment of vaddr,
           invalidate next after last cache line too if needed */
        size += a0 & (PLF_CACHELINE_SIZE - 1);
        do {
#ifdef INSN_NOT_SUPPORTED
            asm volatile(".equ code_clinv, "STRINGIFY(ENC_CLINV(10))";"
                         "mv a0, %0; .word code_clinv;" ::"r"(a0) : "a0");
#else
            asm volatile(".insn i SYSTEM, 0, zero, %[rs1], 0b000100001000"
                         :: [rs1]"r"(a0) : "memory");
#endif // INSN_NOT_SUPPORTED
            a0 += PLF_CACHELINE_SIZE;
            size -= PLF_CACHELINE_SIZE;
        } while (size > 0);
    }
#else
    (void)vaddr;
    (void)size;
#endif // PLF_CACHE_CFG
}

void cache_l1_flush(void* vaddr, long size)
{
#if PLF_CACHE_CFG
    if (size)
    {
        register uintptr_t a0 = (uintptr_t)vaddr;
        /* take in account alignment of vaddr,
           flush next after last cache line too if needed */
        size += a0 & (PLF_CACHELINE_SIZE - 1);
        /* Flush the cache for the requested range */
        do {
#ifdef INSN_NOT_SUPPORTED
            asm volatile(".equ code_clflush, "STRINGIFY(ENC_CLFLUSH(10))";"
                         "mv a0, %0; .word code_clflush;" ::"r"(a0) : "a0");
#else
            asm volatile(".insn i SYSTEM, 0, zero, %[rs1], 0b000100001001"
                         :: [rs1]"r"(a0) : "memory");
#endif // INSN_NOT_SUPPORTED
            a0 += PLF_CACHELINE_SIZE;
            size -= PLF_CACHELINE_SIZE;
        } while (size > 0);
    }

    fence();
#else
    (void)vaddr;
    (void)size;
#endif // PLF_CACHE_CFG
}

int plf_l1cache_prefetcher_info(char *buf, size_t len)
{
    int ssz = 0;
    if (plf_l1cache_prefetcher_is_enabled())
    {
        const unsigned long ctrl_reg_0 = read_csr(L1D_PREFETCHER_CTRL_REG_0);
        const unsigned long conf_ctr_th =
            (ctrl_reg_0 >> L1D_PREFETCHER_CONF_CTR_TH_SHIFT) & L1D_PREFETCHER_PARAM_MASK;
        const unsigned long conf_ctr_max =
            (ctrl_reg_0 >> L1D_PREFETCHER_CONF_CTR_MAX_SHIFT) & L1D_PREFETCHER_PARAM_MASK;
        const unsigned long pref_ctr_max =
            (ctrl_reg_0 >> L1D_PREFETCHER_PREF_CTR_MAX_SHIFT) & L1D_PREFETCHER_PARAM_MASK;

        ssz = snprintf(buf, len, "Confidence counter threshold: %lu max: %lu\n"
                                 "\t\tPrefetch counter max: %lu",
                                  conf_ctr_th, conf_ctr_max, pref_ctr_max);
    }
    return ssz;
}

int cache_l1i_info(char* buf, size_t len)
{
    int sz0 = 0;
#ifdef PLF_CACHE_CFG
    if (cache_l1_available())
    {
        const unsigned long cstate = read_csr(SCR_CSR_CACHE_GLBL);
        const unsigned long cinfo  = read_csr(SCR_CSR_CACHE_DSCR_L1) & 0xffff;

        if (cinfo)
        {
            len -= 1;
            sz0 = snprintf(buf, len, "[%04lx] ", cinfo);

            if (sz0 >= 0 && (size_t)sz0 < len)
            {
                int sz1 = cacheinfo2str(buf + sz0, len - (size_t)sz0, cinfo);
                if (sz1 >= 0)
                {
                    sz0 += sz1;
                    if ((size_t)sz0 < len)
                    {
                        sz1 = snprintf(buf + sz0, len - (size_t)sz0, ", %sabled",
                                       (cstate & CACHE_GLBL_L1I_EN) ? "en" : "dis");
                        if (sz1 >= 0)
                            sz0 += sz1;
                    }
                }
            }
        }
    }
#else
    (void)buf;
    (void)len;
#endif // PLF_CACHE_CFG
    return sz0;
}

int cache_l1d_info(char* buf, size_t len)
{
    int sz0 = 0;
#ifdef PLF_CACHE_CFG
    if (cache_l1_available())
    {
        const unsigned long cstate = read_csr(SCR_CSR_CACHE_GLBL);
        const unsigned long cinfo  = (read_csr(SCR_CSR_CACHE_DSCR_L1) >> 16) & 0xffff;

        if (cinfo)
        {
            len -= 1;
            sz0 = snprintf(buf, len, "[%04lx] ", cinfo);

            if (sz0 >= 0 && (size_t)sz0 < len)
            {
                int sz1 = cacheinfo2str(buf + sz0, len - (size_t)sz0, cinfo);
                if (sz1 >= 0)
                {
                    sz0 += sz1;
                    if ((size_t)sz0 < len)
                    {
                        sz1 = snprintf(buf + sz0, len - (size_t)sz0, ", %sabled",
                                       (cstate & CACHE_GLBL_L1D_EN) ? "en" : "dis");
                        if (sz1 >= 0)
                            sz0 += sz1;
                    }
                }
            }
        }
    }
#else
    (void)buf;
    (void)len;
#endif // PLF_CACHE_CFG
    return sz0;
}

void plf_l2cache_enable(void)
{
#if PLF_L2CTL_BASE
    // init L2$: disable, confirm  state, invalidate, confirm, enable, confirm
    volatile uint32_t* const l2ctl = (volatile uint32_t*)PLF_L2CTL_BASE;

    if (!l2ctl[L2_CSR_VER_IDX])
        return; // cache not exists or not supported

    uint32_t cbmask = l2ctl[L2_CSR_DESCR_IDX];

    cbmask = (1U << (((cbmask >> 16) & 0xf) + 1)) - 1;

    // disable L2$
    l2ctl[L2_CSR_EN_IDX] = 0;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
    // invalidate
    l2ctl[L2_CSR_INV_IDX] = cbmask;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
#if PLF_L3CTL_BASE
    // enable external coherency only if L3 is present
    l2ctl[L2_CSR_SYSCO_IDX] = 1;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
#endif // PLF_L3CTL_BASE
    // enable
    l2ctl[L2_CSR_EN_IDX] = cbmask;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
#endif // PLF_L2CTL_BASE
}

void plf_l2cache_disable(void)
{
#if PLF_L2CTL_BASE
    volatile uint32_t* const l2ctl = (volatile uint32_t*)PLF_L2CTL_BASE;

    if (!l2ctl[L2_CSR_VER_IDX])
        return; // cache not exists or not supported

    // sync (flush) current code
    // FIXME: cache flush before disabing

    // disable
    l2ctl[L2_CSR_EN_IDX] = 0;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
    // flush overall
    l2ctl[L2_CSR_FLUSH_IDX] = ~0U;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
    // invalidate
    l2ctl[L2_CSR_INV_IDX] = ~0U;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
#if PLF_L3CTL_BASE
    // disable external coherency only if L3 is present
    l2ctl[L2_CSR_SYSCO_IDX] = 0;
    // confirm state
    while (l2ctl[L2_CSR_BUSY_IDX])
        ;
#endif // PLF_L3CTL_BASE
#endif // PLF_L2CTL_BASE
}

bool plf_l2cache_is_enabled(void)
{
#if PLF_L2CTL_BASE
    volatile uint32_t* const l2ctl = (volatile uint32_t*)PLF_L2CTL_BASE;

    return l2ctl[L2_CSR_VER_IDX] && l2ctl[L2_CSR_EN_IDX];
#else
    return false;
#endif // PLF_L2CTL_BASE
}

void plf_l2cache_init(void) { plf_l2cache_enable(); }

int cache_l2_info(char* buf, size_t len)
{
#if PLF_L2CTL_BASE
    volatile uint32_t* const l2ctl = (volatile uint32_t*)PLF_L2CTL_BASE;

    const unsigned l2dscr = l2ctl[L2_CSR_DESCR_IDX];

    if (!l2dscr)
        return 0;

    const unsigned banks  = 1 + ((l2dscr >> L2_DESCR_SHIFT_BANKS) & L2_DESCR_MASK_BANKS);
    const unsigned ways   = 1 << ((l2dscr >> L2_DESCR_SHIFT_WAYS) & L2_DESCR_MASK_WAYS);
    const unsigned linesz = 1 << ((l2dscr >> L2_DESCR_SHIFT_LINE) & L2_DESCR_MASK_LINE);
    const unsigned lines  = 1 << ((l2dscr >> L2_DESCR_SHIFT_WIDTH) & L2_DESCR_MASK_WIDTH);
    const unsigned cores  = 1 + ((l2dscr >> L2_DESCR_SHIFT_CPU_NUM) & L2_DESCR_MASK_CPU_NUM);
    const unsigned type   = (l2dscr >> L2_DESCR_SHIFT_TYPE);
    const unsigned size_kb = (lines * linesz * ways * banks) / 1024;

    int ssz = snprintf(buf, len, "[%08x %08x] %uK, %u-ways, %u-bytes line, %s", (unsigned)(l2ctl[L2_CSR_VER_IDX]), l2dscr,
                       size_kb, ways, linesz, (cores > 1 ? "shared" : "dedicated"));

    if (cores > 1)
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, " (%u cores)", cores);
    }

    if (type & L2_DESCR_MASK_TYPE_WRITE_BACK)
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, "\n\t\tWrite-back");
    }
    else
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, "\n\t\tWrite-through");
    }

    if (type & L2_DESCR_MASK_TYPE_ALLOCATE)
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, " allocate");
    }
    else
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, " no allocate");
    }

    if (type & L2_DESCR_MASK_TYPE_INCLUSIVE)
    {
        ssz += snprintf(buf + ssz, len - (size_t)ssz, " inclusive");
    }

    ssz += snprintf(buf + ssz, len - (size_t)ssz, "\n\t\tstatus: %x", (unsigned)l2ctl[L2_CSR_EN_IDX]);

    return ssz;
#else
    (void)buf;
    (void)len;
    return 0;
#endif // PLF_L2CTL_BASE
}

void plf_l3cache_init(void)
{
#if PLF_L3CTL_BASE
    volatile uint64_t* const l3ctl = (volatile uint64_t*)PLF_L3CTL_BASE;

    const uint64_t l3_ver = l3ctl[L3C_VID_IDX];

    if (!l3_ver)
        return;

    // Invalidate all banks
    l3ctl[L3C_CMD_CTRL_IDX] = (L3C_MASK << 8) | L3C_CMD_OPCODE_INVALID;

    const unsigned banks = (l3ctl[L3C_DESCR_CACHE_IDX] >> 8) & L3C_MASK;

    for (unsigned b = 0; b < banks; b++)
    {
        while (!(l3ctl[L3B_STEP * (b + 1) + L3B_CMD_STS] & L3B_CMD_STS_DONE));
    }
#endif // PLF_L3CTL_BASE
}

int cache_l3_info(char* buf, size_t len)
{
#if PLF_L3CTL_BASE
    volatile uint64_t* const l3ctl = (volatile uint64_t*)PLF_L3CTL_BASE;

    const uint64_t l3_ver = l3ctl[L3C_VID_IDX];

    if (!l3_ver)
        return 0;

    const uint64_t l3c_dscr = l3ctl[L3C_DESCR_CACHE_IDX];
    const uint64_t l3b_dscr = l3ctl[L3C_DESCR_BANK_IDX];

    const unsigned cores = (l3c_dscr >> L3C_DESCR_CACHE_SHIFT_CPU_NUM) & L3C_MASK;
    const unsigned ios   = (l3c_dscr >> L3C_DESCR_CACHE_SHIFT_IO_NUM) & L3C_MASK;
    const unsigned banks = (l3c_dscr >> L3C_DESCR_CACHE_SHIFT_BANK_NUM) & L3C_MASK;

    const unsigned lines = 1 << ((l3b_dscr >> L3C_DESCR_BANK_SHIFT_IDX_NUM) & L3C_MASK);
    const unsigned ways  = 1 << ((l3b_dscr >> L3C_DESCR_BANK_SHIFT_WAY_NUM) & L3C_MASK);

    const unsigned linesz = (1 << ((l3b_dscr >> L3C_DESCR_BANK_SHIFT_LINE_WIDTH) & L3C_MASK)) / 8;

    const unsigned size_kb = ((uint64_t)lines * linesz * ways * banks) / 1024;
    const unsigned size_mb = size_kb / 1024;

    int ssz = snprintf(buf, len, "[%016lx %08x %016lx] %u%c, %u-ways, %u-bytes line", l3_ver, (uint32_t)l3c_dscr,
                       l3b_dscr, size_mb ? size_mb : size_kb, size_mb ? 'M' : 'K', ways, linesz);

    ssz += snprintf(buf + ssz, len - ssz, " (%u core%s)", cores, (cores > 1) ? "s" : "");

    ssz += snprintf(buf + ssz, len - ssz, " (%u IOs)", ios);

    return ssz;
#else
    (void)buf;
    (void)len;
    return 0;
#endif // PLF_L3CTL_BASE
}
