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
/// @brief PMU driver implementation
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

#if __riscv_xlen == 64

#include "drivers/cache.h"
#include "drivers/pmu.h"

#include "arch.h"
#include "perf.h"
#include "utils.h"

#include <stdint.h>

typedef struct {
    unsigned long selector;
    const char* name;
    const size_t name_len;
} pmuc_descriptor_t;

typedef struct {
    uint64_t value;
    size_t id;
} pmuc_counter_t;

#define PMUC_NAME_LENGTH(name) name, sizeof(name) - 1
#define PMUC_EVENT_SEL(event) (event << 4)

static const pmuc_descriptor_t pmuc_descriptors[] = {
    { (PMUC_EVENT_SEL(GEN_INST) | GRP_GEN), PMUC_NAME_LENGTH("gen_inst") },
    { (PMUC_EVENT_SEL(GEN_CYC) | GRP_GEN), PMUC_NAME_LENGTH("gen_cyc") },
    { (PMUC_EVENT_SEL(GEN_ISS) | GRP_GEN), PMUC_NAME_LENGTH("gen_iss") },
    { (PMUC_EVENT_SEL(GEN_SPEC) | GRP_GEN), PMUC_NAME_LENGTH("gen_spec") },

    { (PMUC_EVENT_SEL(PRD_BRANCH) | GRP_PRD), PMUC_NAME_LENGTH("prd_branch") },
    { (PMUC_EVENT_SEL(PRD_BR_MIS) | GRP_PRD), PMUC_NAME_LENGTH("prd_br_mis") },
    { (PMUC_EVENT_SEL(PRD_CALL) | GRP_PRD), PMUC_NAME_LENGTH("prd_call") },
    { (PMUC_EVENT_SEL(PRD_CALL_MIS) | GRP_PRD), PMUC_NAME_LENGTH("prd_call_mis") },
    { (PMUC_EVENT_SEL(PRD_RET) | GRP_PRD), PMUC_NAME_LENGTH("prd_ret") },
    { (PMUC_EVENT_SEL(PRD_RET_MIS) | GRP_PRD), PMUC_NAME_LENGTH("prd_ret_mis") },
    { (PMUC_EVENT_SEL(PRD_CFI) | GRP_PRD), PMUC_NAME_LENGTH("prd_cfi") },
    { (PMUC_EVENT_SEL(PRD_CFI_MIS) | GRP_PRD), PMUC_NAME_LENGTH("prd_cfi_mis") },
    { (PMUC_EVENT_SEL(PRD_REL_JMP) | GRP_PRD), PMUC_NAME_LENGTH("prd_rel_jmp") },
    { (PMUC_EVENT_SEL(PRD_DIR_JMP) | GRP_PRD), PMUC_NAME_LENGTH("prd_dir_jmp") },

    { (PMUC_EVENT_SEL(EXC_INT_RTR) | GRP_EXE), PMUC_NAME_LENGTH("exc_int_rtr") },
    { (PMUC_EVENT_SEL(EXC_FP_RTR) | GRP_EXE), PMUC_NAME_LENGTH("exc_fp_rtr") },
    { (PMUC_EVENT_SEL(EXC_MEM_RTR) | GRP_EXE), PMUC_NAME_LENGTH("exc_mem_rtr") },
    { (PMUC_EVENT_SEL(EXC_ST_ISS) | GRP_EXE), PMUC_NAME_LENGTH("exc_st_iss") },
    { (PMUC_EVENT_SEL(EXC_ST_SRCT) | GRP_EXE), PMUC_NAME_LENGTH("exc_st_srct") },
    { (PMUC_EVENT_SEL(EXC_ST_LSU) | GRP_EXE), PMUC_NAME_LENGTH("exc_st_lsu") },
    { (PMUC_EVENT_SEL(EXC_ST_INT) | GRP_EXE), PMUC_NAME_LENGTH("exc_st_int") },
    { (PMUC_EVENT_SEL(EXC_ST_FPU) | GRP_EXE), PMUC_NAME_LENGTH("exc_st_fpu") },

    { (PMUC_EVENT_SEL(L1I_HIT) | GRP_L1I), PMUC_NAME_LENGTH("l1i_hit") },
    { (PMUC_EVENT_SEL(L1I_MISS) | GRP_L1I), PMUC_NAME_LENGTH("l1i_miss") },
    { (PMUC_EVENT_SEL(L1I_TLB_ACCESS) | GRP_L1I), PMUC_NAME_LENGTH("l1i_tlb_access") },
    { (PMUC_EVENT_SEL(L1I_TLB_MISS) | GRP_L1I), PMUC_NAME_LENGTH("l1i_tlb_miss") },
    { (PMUC_EVENT_SEL(L1I_EVICT) | GRP_L1I), PMUC_NAME_LENGTH("l1i_evict") },

    { (PMUC_EVENT_SEL(L1D_HIT) | GRP_L1D), PMUC_NAME_LENGTH("l1d_hit") },
    { (PMUC_EVENT_SEL(L1D_MISS) | GRP_L1D), PMUC_NAME_LENGTH("l1d_miss") },
    { (PMUC_EVENT_SEL(L1D_TLB_ACCESS) | GRP_L1D), PMUC_NAME_LENGTH("l1d_tlb_access") },
    { (PMUC_EVENT_SEL(L1D_TLB_MISS) | GRP_L1D), PMUC_NAME_LENGTH("l1d_tlb_miss") },
    { (PMUC_EVENT_SEL(L1D_SNP_L1D) | GRP_L1D), PMUC_NAME_LENGTH("l1d_snp_l1d") },
    { (PMUC_EVENT_SEL(L1D_EVICT) | GRP_L1D), PMUC_NAME_LENGTH("l1d_evict") },
    { (PMUC_EVENT_SEL(L1D_LD_SPEC) | GRP_L1D), PMUC_NAME_LENGTH("l1d_ld_spec") },
    { (PMUC_EVENT_SEL(L1D_ST_SPEC) | GRP_L1D), PMUC_NAME_LENGTH("l1d_st_spec") },

    { (PMUC_EVENT_SEL(L1D_PF_ISS) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_iss") },
    { (PMUC_EVENT_SEL(L1D_PF_REQ) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_req") },
    { (PMUC_EVENT_SEL(L1D_PF_ISS_HIT) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_iss_hit") },
    { (PMUC_EVENT_SEL(L1D_PF_FLT_HIT) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_flt_hit") },
    { (PMUC_EVENT_SEL(L1D_PF_HIT_SM) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_hit_sm") },
    { (PMUC_EVENT_SEL(L1D_PF_HIT_DC) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_hit_dc") },
    { (PMUC_EVENT_SEL(L1D_PF_HIT_CLB) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_hit_clb") },
    { (PMUC_EVENT_SEL(L1D_PF_CANCEL) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_cancel") },
    { (PMUC_EVENT_SEL(L1D_PF_ISS_INV) | GRP_L1D_PREF), PMUC_NAME_LENGTH("l1d_pf_iss_inv") },

    { (PMUC_EVENT_SEL(HPW_4K_HIT) | GRP_HPW), PMUC_NAME_LENGTH("hpw_4k_hit") },
    { (PMUC_EVENT_SEL(HPW_MP_HIT) | GRP_HPW), PMUC_NAME_LENGTH("hpw_mp_hit") },
    { (PMUC_EVENT_SEL(HPW_GP_HIT) | GRP_HPW), PMUC_NAME_LENGTH("hpw_gp_hit") },
    { (PMUC_EVENT_SEL(HPW_ITLB_REQ) | GRP_HPW), PMUC_NAME_LENGTH("hpw_itlb_req") },
    { (PMUC_EVENT_SEL(HPW_DTLB_REQ) | GRP_HPW), PMUC_NAME_LENGTH("hpw_dtlb_req") },
    { (PMUC_EVENT_SEL(HPW_PREF_REQ) | GRP_HPW), PMUC_NAME_LENGTH("hpw_pref_req") },
    { (PMUC_EVENT_SEL(HPW_PAGE_FAULT) | GRP_HPW), PMUC_NAME_LENGTH("hpw_page_fault") },
    { (PMUC_EVENT_SEL(HPW_ACCESS_FAULT) | GRP_HPW), PMUC_NAME_LENGTH("hpw_access_fault") },
    { (PMUC_EVENT_SEL(HPW_NO_ERR) | GRP_HPW), PMUC_NAME_LENGTH("hpw_no_err") },

    { (PMUC_EVENT_SEL(STLB_REQ) | GRP_STLB), PMUC_NAME_LENGTH("stlb_req") },
    { (PMUC_EVENT_SEL(STLB_REFILL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_refill") },
    { (PMUC_EVENT_SEL(STLB_ITLB_REQ) | GRP_STLB), PMUC_NAME_LENGTH("stlb_itlb_req") },
    { (PMUC_EVENT_SEL(STLB_DTLB_REQ) | GRP_STLB), PMUC_NAME_LENGTH("stlb_dtlb_req") },
    { (PMUC_EVENT_SEL(STLB_ITLB_FLUSH) | GRP_STLB), PMUC_NAME_LENGTH("stlb_itlb_flush") },
    { (PMUC_EVENT_SEL(STLB_DTLB_FLUSH) | GRP_STLB), PMUC_NAME_LENGTH("stlb_dtlb_flush") },
    { (PMUC_EVENT_SEL(STLB_ITLB_MISS) | GRP_STLB), PMUC_NAME_LENGTH("stlb_itlb_miss") },
    { (PMUC_EVENT_SEL(STLB_DTLB_MISS) | GRP_STLB), PMUC_NAME_LENGTH("stlb_dtlb_miss") },
    { (PMUC_EVENT_SEL(STLB_ITLB_STALL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_itlb_stall") },
    { (PMUC_EVENT_SEL(STLB_DTLB_STALL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_dtlb_stall") },
    { (PMUC_EVENT_SEL(STLB_HPW_STALL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_hpw_stall") },
    { (PMUC_EVENT_SEL(STLB_SFENCE_STALL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_sfence_stall") },
    { (PMUC_EVENT_SEL(STLB_SFENCE_FLUSH_ALL) | GRP_STLB), PMUC_NAME_LENGTH("stlb_sfence_flush_all") },
    { (PMUC_EVENT_SEL(STLB_RESP_WO_ERR) | GRP_STLB), PMUC_NAME_LENGTH("stlb_resp_wo_err") },
    { (PMUC_EVENT_SEL(STLB_RESP_ANY) | GRP_STLB), PMUC_NAME_LENGTH("stlb_resp_any") },

    { (PMUC_EVENT_SEL(PREF_REQ) | GRP_PREF), PMUC_NAME_LENGTH("pref_req") },
    { (PMUC_EVENT_SEL(PREF_HIT) | GRP_PREF), PMUC_NAME_LENGTH("pref_hit") },
    { (PMUC_EVENT_SEL(PREF_HPW_REQ) | GRP_PREF), PMUC_NAME_LENGTH("pref_hpw_req") },
    { (PMUC_EVENT_SEL(PREF_LATE_REQ) | GRP_PREF), PMUC_NAME_LENGTH("pref_late_req") },

    { (PMUC_EVENT_SEL(HPW0_4K_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_4k_req") },
    { (PMUC_EVENT_SEL(HPW0_2M_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_2m_req") },
    { (PMUC_EVENT_SEL(HPW0_1G_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_1g_req") },
    { (PMUC_EVENT_SEL(HPW0_4K_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_4k_wait") },
    { (PMUC_EVENT_SEL(HPW0_2M_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_2m_wait") },
    { (PMUC_EVENT_SEL(HPW0_1G_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw0_1g_wait") },
    { (PMUC_EVENT_SEL(HPW1_4K_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_4k_req") },
    { (PMUC_EVENT_SEL(HPW1_2M_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_2m_req") },
    { (PMUC_EVENT_SEL(HPW1_1G_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_1g_req") },
    { (PMUC_EVENT_SEL(HPW1_4K_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_4k_wait") },
    { (PMUC_EVENT_SEL(HPW1_2M_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_2m_wait") },
    { (PMUC_EVENT_SEL(HPW1_1G_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw1_1g_wait") },
    { (PMUC_EVENT_SEL(HPW2_4K_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_4k_req") },
    { (PMUC_EVENT_SEL(HPW2_2M_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_2m_req") },
    { (PMUC_EVENT_SEL(HPW2_1G_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_1g_req") },
    { (PMUC_EVENT_SEL(HPW2_4K_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_4k_wait") },
    { (PMUC_EVENT_SEL(HPW2_2M_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_2m_wait") },
    { (PMUC_EVENT_SEL(HPW2_1G_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw2_1g_wait") },
    { (PMUC_EVENT_SEL(HPW3_4K_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_4k_req") },
    { (PMUC_EVENT_SEL(HPW3_2M_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_2m_req") },
    { (PMUC_EVENT_SEL(HPW3_1G_REQ) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_1g_req") },
    { (PMUC_EVENT_SEL(HPW3_4K_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_4k_wait") },
    { (PMUC_EVENT_SEL(HPW3_2M_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_2m_wait") },
    { (PMUC_EVENT_SEL(HPW3_1G_WAIT) | GRP_HPWX), PMUC_NAME_LENGTH("hpw3_1g_wait") },

#ifdef PLF_L2CTL_BASE
    { 0, PMUC_NAME_LENGTH("l2/hit") },
    { 1, PMUC_NAME_LENGTH("l2/miss") },
    { 2, PMUC_NAME_LENGTH("l2/refill") },
    { 3, PMUC_NAME_LENGTH("l2/evict-clear") },
    { 4, PMUC_NAME_LENGTH("l2/evict-dirty") },
    { 5, PMUC_NAME_LENGTH("l2/evict-rollback") },
    { 6, PMUC_NAME_LENGTH("l2/evict-collision") },
    { 7, PMUC_NAME_LENGTH("l2/evict-request") },
    { 8, PMUC_NAME_LENGTH("l2/evict-snoop") },
#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE
    { 0, PMUC_NAME_LENGTH("l3/nop")},
    { 1, PMUC_NAME_LENGTH("l3/hit")},
    { 2, PMUC_NAME_LENGTH("l3/miss")},
    { 3, PMUC_NAME_LENGTH("l3/retry")},
    { 4, PMUC_NAME_LENGTH("l3/evict-clear")},
    { 5, PMUC_NAME_LENGTH("l3/evict-dirty")},
    { 6, PMUC_NAME_LENGTH("l3/rollback")},
    { 7, PMUC_NAME_LENGTH("l3/collision")},
    { 8, PMUC_NAME_LENGTH("l3/request")},
    { 9, PMUC_NAME_LENGTH("l3/snoop")},
    { 10, PMUC_NAME_LENGTH("l3/writes")},
    { 11, PMUC_NAME_LENGTH("l3/reads")},
    { 12, PMUC_NAME_LENGTH("l3/dat_flits")},
    { 13, PMUC_NAME_LENGTH("l3/clk")}
#endif //PLF_L3CTL_BASE
};

#define PMUC_EVENT_MAX ARRAY_SIZE(pmuc_descriptors)

#ifdef PLF_L3CTL_BASE
#define PMUC_L3_EVENT_MAX (14)
#define PMUC_L3_EVENT_START (PMUC_EVENT_MAX - PMUC_L3_EVENT_MAX)
#endif // PLF_L3CTL_BASE


#ifdef PLF_L2CTL_BASE
#define PMUC_L2_EVENT_MAX (9)

#ifdef PLF_L3CTL_BASE
#define PMUC_L2_EVENT_START (PMUC_L3_EVENT_START - PMUC_L2_EVENT_MAX)
#else
#define PMUC_L2_EVENT_START (PMUC_EVENT_MAX - PMUC_L2_EVENT_MAX)
#endif // PLF_L3CTL_BASE

#endif // PLF_L2CTL_BASE

#if defined(PLF_L2CTL_BASE) && defined(PLF_L3CTL_BASE)
#define PMUC_L2_EVENT_MAX (9)
#define PMUC_L2_EVENT_START (PMUC_L3_EVENT_START - PMUC_L2_EVENT_MAX)
#endif // PLF_L2CTL_BASE && PLF_L3CTL_BASE

#if defined(PLF_L2CTL_BASE) && !defined(PLF_L3CTL_BASE)
#define PMUC_L2_EVENT_MAX (9)
#define PMUC_L2_EVENT_START (PMUC_EVENT_MAX - PMUC_L2_EVENT_MAX)
#endif // PLF_L2CTL_BASE && !PLF_L3CTL_BASE

#define PMUC_CSR_EVENT_IDX_BASE 0x3
#define PMUC_CSR_MHPMEVENT_OF (1UL << 63)
#define PMUC_BIT(x) (1UL << (x))

__attribute__((section (".data")))
static pmuc_counter_t pmu_csr_counters[PMUC_MAX_CSR_EVENT_COUNT];

__attribute__((section (".data")))
unsigned long pmuc_csr_counters_mask = 0;

__attribute__((section (".data")))
static size_t pmuc_available_csr_counters_num = 0;

__attribute__((section (".data")))
static size_t pmuc_selected_csr_counters_num = 0;

#ifdef PLF_L2CTL_BASE
__attribute__((section (".data")))
static pmuc_counter_t pmu_l2_counters[PMUC_MAX_L2_EVENT_COUNT];

__attribute__((section (".data")))
static size_t pmuc_selected_l2_counters_num = 0;
#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE
__attribute__((section (".data")))
static pmuc_counter_t pmu_l3_counters[PMUC_MAX_L3_EVENT_COUNT];

__attribute__((section (".data")))
static size_t pmuc_selected_l3_counters_num = 0;
#endif // PLF_L3CTL_BASE

#define PMUC_CSR_HPMCOUNTER3 0xc03
#define PMUC_CSR_HPMCOUNTER4 0xc04
#define PMUC_CSR_HPMCOUNTER8 0xc08
#define PMUC_CSR_HPMCOUNTER16 0xc10

#define PMUC_CSR_MHPMCOUNTER3 0xb03
#define PMUC_CSR_MHPMCOUNTER4 0xb04
#define PMUC_CSR_MHPMCOUNTER8 0xb08
#define PMUC_CSR_MHPMCOUNTER16 0xb10

#define PMUC_CSR_MHPMEVENT3 0x323
#define PMUC_CSR_MHPMEVENT4 0x324
#define PMUC_CSR_MHPMEVENT8 0x328
#define PMUC_CSR_MHPMEVENT16 0x330

static unsigned long csr_read_num(int csr_num)
{
#define switchcase_csr_read(__csr_num, __val) \
    case __csr_num:                           \
        __val = read_csr(__csr_num);          \
        break;
#define switchcase_csr_read_2(__csr_num, __val) \
    switchcase_csr_read(__csr_num + 0, __val) switchcase_csr_read(__csr_num + 1, __val)
#define switchcase_csr_read_4(__csr_num, __val) \
    switchcase_csr_read_2(__csr_num + 0, __val) switchcase_csr_read_2(__csr_num + 2, __val)
#define switchcase_csr_read_8(__csr_num, __val) \
    switchcase_csr_read_4(__csr_num + 0, __val) switchcase_csr_read_4(__csr_num + 4, __val)
#define switchcase_csr_read_16(__csr_num, __val) \
    switchcase_csr_read_8(__csr_num + 0, __val) switchcase_csr_read_8(__csr_num + 8, __val)

    unsigned long ret = 0;
    // clang-format off
    switch (csr_num) {
        switchcase_csr_read(PMUC_CSR_HPMCOUNTER3, ret)
        switchcase_csr_read_4(PMUC_CSR_HPMCOUNTER4, ret)
        switchcase_csr_read_8(PMUC_CSR_HPMCOUNTER8, ret)
        switchcase_csr_read_16(PMUC_CSR_HPMCOUNTER16, ret)
        default : break;
    }
    // clang-format on
    return ret;
#undef switchcase_csr_read_16
#undef switchcase_csr_read_8
#undef switchcase_csr_read_4
#undef switchcase_csr_read_2
#undef switchcase_csr_read
}

static void csr_write_num(int csr_num, unsigned long val)
{
#define switchcase_csr_write(__csr_num, __val) \
    case __csr_num:                            \
        write_csr(__csr_num, __val);           \
        break;
#define switchcase_csr_write_2(__csr_num, __val) \
    switchcase_csr_write(__csr_num + 0, __val) switchcase_csr_write(__csr_num + 1, __val)
#define switchcase_csr_write_4(__csr_num, __val) \
    switchcase_csr_write_2(__csr_num + 0, __val) switchcase_csr_write_2(__csr_num + 2, __val)
#define switchcase_csr_write_8(__csr_num, __val) \
    switchcase_csr_write_4(__csr_num + 0, __val) switchcase_csr_write_4(__csr_num + 4, __val)
#define switchcase_csr_write_16(__csr_num, __val) \
    switchcase_csr_write_8(__csr_num + 0, __val) switchcase_csr_write_8(__csr_num + 8, __val)

    // clang-format off
    switch (csr_num) {
        switchcase_csr_write(PMUC_CSR_MHPMCOUNTER3, val)
        switchcase_csr_write_4(PMUC_CSR_MHPMCOUNTER4, val)
        switchcase_csr_write_8(PMUC_CSR_MHPMCOUNTER8, val)
        switchcase_csr_write_16(PMUC_CSR_MHPMCOUNTER16, val)
        switchcase_csr_write(PMUC_CSR_MHPMEVENT3, val)
        switchcase_csr_write_4(PMUC_CSR_MHPMEVENT4, val)
        switchcase_csr_write_8(PMUC_CSR_MHPMEVENT8, val)
        switchcase_csr_write_16(PMUC_CSR_MHPMEVENT16, val)
        default : break;
    }
    // clang-format on
#undef switchcase_csr_write_16
#undef switchcase_csr_write_8
#undef switchcase_csr_write_4
#undef switchcase_csr_write_2
#undef switchcase_csr_write
}

static inline uint64_t pmuc_read_csr_counter(int idx)
{
    return csr_read_num(PMUC_CSR_HPMCOUNTER3 + idx);
}

static inline void pmuc_write_csr_counter(int idx, unsigned long val)
{
    csr_write_num(PMUC_CSR_MHPMCOUNTER3 + idx, val);
}

static inline void pmuc_setup_csr_counter(int idx, unsigned long val)
{
    csr_write_num(PMUC_CSR_MHPMEVENT3 + idx, val);
    pmuc_csr_counters_mask |= PMUC_BIT(PMUC_CSR_EVENT_IDX_BASE + idx);
}

#ifdef PLF_L2CTL_BASE

static inline uint64_t pmuc_read_l2_counter(int idx)
{
    ifence();
    return (uint64_t)L2_COUNTER_LO(idx) + ((uint64_t)L2_COUNTER_HI(idx) << 32);
}

static inline void pmuc_write_l2_counter(int idx, unsigned long val)
{
    const uint32_t low = (uint32_t)(val & 0xFFFFFFFF);
    const uint32_t hi = (uint32_t)(val >> 32);

    ifence();

    L2_COUNTER_LO(idx) = low;
    while(L2_COUNTER_LO(idx) != low);
    ifence();

    L2_COUNTER_HI(idx) = hi;
    while(L2_COUNTER_HI(idx) != hi);
    ifence();
}

static inline void pmuc_start_l2_counter(int idx, unsigned long val)
{
    *(volatile uint32_t*)(L2_PCEN_CTRL(idx)) = (uint32_t)val;
    ifence();
}

static inline void pmuc_stop_l2_counter(int idx)
{
    *(volatile uint32_t*)(L2_PCEN_CTRL(idx)) = 0;
    ifence();
}

void pmuc_start_selected_l2_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_l2_counters_num; i++) {
        const size_t id = pmu_l2_counters[i].id;
        pmuc_start_l2_counter(i, pmuc_descriptors[id].selector | PMUC_BANK_SEL_MASK);
    }
}

void pmuc_stop_selected_l2_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_l2_counters_num; i++) {
        pmuc_stop_l2_counter(i);
    }
}

#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE

static inline uint64_t get_l3_banks_num()
{
    volatile uint64_t* const l3ctl = (volatile uint64_t*)PLF_L3CTL_BASE;
    const uint64_t l3c_dscr = l3ctl[L3C_DESCR_CACHE_IDX];

    // Num of banks:
    return (l3c_dscr >> L3C_DESCR_CACHE_SHIFT_BANK_NUM) & L3C_MASK;
}

static inline uint64_t pmuc_read_l3_counter(int idx)
{
    uint64_t sum = 0;
    // for all L3 banks
    const size_t l3_banks = get_l3_banks_num();
    ifence();

    for (size_t b = 0; b < l3_banks; ++b) {
        sum += L3_COUNTER(b, idx);
    }
    ifence();
    return sum;
}

static inline void pmuc_write_l3_counter(int idx, unsigned long val)
{
    const size_t l3_banks = get_l3_banks_num();
    ifence();

    for (size_t b = 0; b < l3_banks; ++b) {
        L3_COUNTER(b, idx) = val;
    }
    ifence();
}

static inline void pmuc_start_l3_counter(int idx, unsigned long val)
{
    *(volatile uint64_t*)(L3C_PCE_CTRL0 + L3C_REG_SIZE * idx) = val & L3C_MASK;
    ifence();
}

static inline void pmuc_stop_l3_counter(int idx)
{
    *(volatile uint64_t*)(L3C_PCE_CTRL0 + L3C_REG_SIZE * idx) = 0;
    ifence();
}

void pmuc_start_selected_l3_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_l3_counters_num; i++) {
        const size_t id = pmu_l3_counters[i].id;
        pmuc_start_l3_counter(i, pmuc_descriptors[id].selector | PMUC_BANK_SEL_MASK);
    }
}

void pmuc_stop_selected_l3_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_l3_counters_num; i++) {
        pmuc_stop_l3_counter(i);
    }
}

#endif // PLF_L3CTL_BASE

static size_t pmuc_get_descriptor(const char* name)
{
    size_t length = 0;
    size_t pos = 0;
    const char* begin = name;
    while (*begin) {
        length++;
        begin++;
    }

    for (size_t i = 0; i < PMUC_EVENT_MAX; i++) {
        if (pmuc_descriptors[i].name_len != length)
            continue;

        for (pos = 0; pos < length; pos++) {
            if (pmuc_descriptors[i].name[pos] != name[pos])
                break;
        }

        if (pos == length)
            return i;
    }
    return PMUC_EVENT_MAX;
}

void pmuc_enable_cycles(void)
{
    pmuc_csr_counters_mask |= PMUC_BIT(0);
}

void pmuc_setup_selected_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_csr_counters_num; i++) {
        pmuc_write_csr_counter(i, 0);

        const size_t id = pmu_csr_counters[i].id;
        pmuc_setup_csr_counter(i, pmuc_descriptors[id].selector | PMUC_CSR_MHPMEVENT_OF);
    }

#ifdef PLF_L2CTL_BASE
    for (size_t i = 0; i < pmuc_selected_l2_counters_num; i++) {
        pmuc_write_l2_counter(i, 0);
    }
#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE
    for (size_t i = 0; i < pmuc_selected_l3_counters_num; i++) {
        pmuc_write_l3_counter(i, 0);
    }
#endif // PLF_L3CTL_BASE
}

size_t pmuc_get_available_csr_counters_num(void)
{
    if (pmuc_available_csr_counters_num > 0)
        return pmuc_available_csr_counters_num;

    write_csr(mcounteren, -1UL); // enable access for all HPM counters
    unsigned long reg_val = read_csr(mcounteren);
    for (size_t i = PMUC_CSR_EVENT_IDX_BASE; i < (PMUC_CSR_EVENT_IDX_BASE + PMUC_MAX_CSR_EVENT_COUNT); i++) {
        if (reg_val & (1UL << i)) {
            pmuc_available_csr_counters_num++;
        } else {
            break;
        }
    }

    if (pmuc_available_csr_counters_num > PMUC_MAX_CSR_EVENT_COUNT)
        pmuc_available_csr_counters_num = PMUC_MAX_CSR_EVENT_COUNT;

    return pmuc_available_csr_counters_num;
}

size_t pmuc_get_selected_counters_num(void)
{
    return pmuc_selected_csr_counters_num 
#ifdef PLF_L2CTL_BASE
    + pmuc_selected_l2_counters_num
#endif // PLF_L2CTL_BASE
#ifdef PLF_L3CTL_BASE
    + pmuc_selected_l3_counters_num
#endif // PLF_L3CTL_BASE
    ;
}

uint64_t pmuc_get_counter_value(size_t index)
{
    if (index < pmuc_selected_csr_counters_num)
        return pmu_csr_counters[index].value;
#ifdef PLF_L2CTL_BASE
    if (index < (pmuc_selected_csr_counters_num + pmuc_selected_l2_counters_num))
        return pmu_l2_counters[index - pmuc_selected_csr_counters_num].value;
#endif // PLF_L2CTL_BASE
#ifdef PLF_L3CTL_BASE
    if (index < (pmuc_selected_csr_counters_num + pmuc_selected_l2_counters_num + pmuc_selected_l3_counters_num))
        return pmu_l3_counters[index - pmuc_selected_csr_counters_num - pmuc_selected_l2_counters_num].value;
#endif // PLF_L3CTL_BASE

    return pmu_csr_counters[index].value;
}

const char* pmuc_get_counter_name(size_t index)
{
    size_t id = 0;

#ifdef PLF_L3CTL_BASE
    if (index < (pmuc_selected_csr_counters_num + pmuc_selected_l2_counters_num + pmuc_selected_l3_counters_num))
        id = pmu_l3_counters[index - pmuc_selected_csr_counters_num - pmuc_selected_l2_counters_num].id;
#endif // PLF_L3CTL_BASE
#ifdef PLF_L2CTL_BASE
    if (index < (pmuc_selected_csr_counters_num + pmuc_selected_l2_counters_num))
        id = pmu_l2_counters[index - pmuc_selected_csr_counters_num].id;
#endif // PLF_L2CTL_BASE
    if (index < pmuc_selected_csr_counters_num)
        id = pmu_csr_counters[index].id;

    return pmuc_descriptors[id].name;
}

int pmuc_add_counter(const char* name)
{
    const size_t id = pmuc_get_descriptor(name);

    if (id == PMUC_EVENT_MAX)
        return PMUC_R_INVALID_ID;

#ifdef PLF_L2CTL_BASE
    if ((id >= PMUC_L2_EVENT_START) && (id <= PMUC_L2_EVENT_START + PMUC_L2_EVENT_MAX)) {
        if (pmuc_selected_l2_counters_num == PMUC_MAX_L2_EVENT_COUNT) {
            return PMUC_R_L2_COUNTERS_LIMIT;
        }
        for (size_t i = 0; i < pmuc_selected_l2_counters_num; i++) {
            if (pmu_l2_counters[i].id == id) {
                return PMUC_R_DUPLICATE_ID;
            }
        }
        pmu_l2_counters[pmuc_selected_l2_counters_num++].id = id;
    }
    else
#endif // PLF_L2CTL_BASE
#ifdef PLF_L3CTL_BASE
    if ((id >= PMUC_L3_EVENT_START) && (id <= PMUC_L3_EVENT_START + PMUC_L3_EVENT_MAX)) {
        if (pmuc_selected_l3_counters_num == PMUC_MAX_L3_EVENT_COUNT) {
            return PMUC_R_L3_COUNTERS_LIMIT;
        }
        for (size_t i = 0; i < pmuc_selected_l3_counters_num; i++) {
            if (pmu_l3_counters[i].id == id) {
                return PMUC_R_DUPLICATE_ID;
            }
        }
        pmu_l3_counters[pmuc_selected_l3_counters_num++].id = id;
    }
    else
#endif // PLF_L3CTL_BASE
    {
        for (size_t i = 0; i < pmuc_selected_csr_counters_num; i++) {
            if (pmu_csr_counters[i].id == id) {
                return PMUC_R_DUPLICATE_ID;
            }
        }
        if (pmuc_selected_csr_counters_num == pmuc_get_available_csr_counters_num()) {
            return PMUC_R_CSR_COUNTERS_LIMIT;
        }
        pmu_csr_counters[pmuc_selected_csr_counters_num++].id = id;
    }

    return PMUC_R_OK;
}

void pmuc_update_counters(void)
{
    for (size_t i = 0; i < pmuc_selected_csr_counters_num; i++) {
        pmu_csr_counters[i].value = pmuc_read_csr_counter(i);
    }

#ifdef PLF_L2CTL_BASE
    for (size_t i = 0; i < pmuc_selected_l2_counters_num; i++) {
        pmu_l2_counters[i].value = pmuc_read_l2_counter(i);
    }
#endif // PLF_L2CTL_BASE

#ifdef PLF_L3CTL_BASE
    for (size_t i = 0; i < pmuc_selected_l3_counters_num; i++) {
        pmu_l3_counters[i].value = pmuc_read_l3_counter(i);
    }
#endif // PLF_L3CTL_BASE

}

#endif // __riscv_xlen == 64
