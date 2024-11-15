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
/// @brief Perfomance counters defs and inlines
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

#ifndef SCR_BSP_PERF_H
#define SCR_BSP_PERF_H

#include "csr.h"

#include <stdint.h>

enum perf_hw_id
{
    // Common hardware events, generalized by the kernel
    PERF_COUNT_HW_CPU_CYCLES              = 0,
    PERF_COUNT_HW_INSTRUCTIONS            = 1,
    PERF_COUNT_HW_CACHE_REFERENCES        = 2,
    PERF_COUNT_HW_CACHE_MISSES            = 3,
    PERF_COUNT_HW_BRANCH_INSTRUCTIONS     = 4,
    PERF_COUNT_HW_BRANCH_MISSES           = 5,
    PERF_COUNT_HW_BUS_CYCLES              = 6,
    PERF_COUNT_HW_STALLED_CYCLES_FRONTEND = 7,
    PERF_COUNT_HW_STALLED_CYCLES_BACKEND  = 8,
    PERF_COUNT_HW_REF_CPU_CYCLES          = 9,
    PERF_COUNT_HW_MAX, /* non-ABI */

    // Platform specific events
    PERF_COUNT_HW_L1D_HITS   = 113,
    PERF_COUNT_HW_L1D_MISSES = 117,
    PERF_COUNT_HW_L1I_HITS   = 123,
    PERF_COUNT_HW_L1I_MISSES = 127
};

enum perf_sw_ids
{
    PERF_COUNT_SW_CPU_CLOCK        = 0,
    PERF_COUNT_SW_TASK_CLOCK       = 1,
    PERF_COUNT_SW_PAGE_FAULTS      = 2,
    PERF_COUNT_SW_CONTEXT_SWITCHES = 3,
    PERF_COUNT_SW_CPU_MIGRATIONS   = 4,
    PERF_COUNT_SW_PAGE_FAULTS_MIN  = 5,
    PERF_COUNT_SW_PAGE_FAULTS_MAJ  = 6,
    PERF_COUNT_SW_ALIGNMENT_FAULTS = 7,
    PERF_COUNT_SW_EMULATION_FAULTS = 8,
    PERF_COUNT_SW_DUMMY            = 9,
    PERF_COUNT_SW_BPF_OUTPUT       = 10,
    PERF_COUNT_SW_MAX /* non-ABI */
};

enum perf_type_id
{
    PERF_TYPE_HARDWARE   = 0,
    PERF_TYPE_SOFTWARE   = 1,
    PERF_TYPE_TRACEPOINT = 2,
    PERF_TYPE_HW_CACHE   = 3,
    PERF_TYPE_RAW        = 4,
    PERF_TYPE_BREAKPOINT = 5,
    PERF_TYPE_MAX /* non-ABI */
};

enum PERF_EVENT_GRP
{
    GRP_DIS      = 0,  // HPM counter is disabled
    GRP_GEN      = 1,  // General group
    GRP_PRD      = 2,  // Prediction group
    GRP_EXE      = 3,  // Execution group
    GRP_L1I      = 4,  // L1I cache group
    GRP_L1D      = 5,  // L1D cache group
    GRP_EXC      = 6,  // Exceptions group
    GRP_IRQ      = 7,  // Interrupts group
    GRP_L1D_PREF = 9,  // L1D prefetcher group
    GRP_HPW      = 10, // Hardware Pagewalker group
    GRP_STLB     = 11, // Shared TLB group
    GRP_PREF     = 12, // Page prefetcher group
    GRP_HPWX     = 13  // Hardware Pagewalker (0-3) group
};

enum PERF_EVENT_GRP_GEN
{
    GEN_CYC   = 0, // GEN_CYC Cycles
    GEN_RSV0  = 1, // Reserved 0
    GEN_INST  = 2, // Instructions retired
    GEN_ISS   = 3, // Instructions issued
    GEN_SPEC  = 4, // GEN_SPEC_EXC Instructions executed speculatively
    GEN_RSV1  = 5, // Reserved 1
    GEN_STRAP = 6, // Trap/IRQ to S-mode
    GEN_MTRAP = 7, // Trap/IRQ to M-mode
    GEN_URET  = 8, // MRET/SRET to U-mode
    GEN_SRET  = 9, // MRET/SRET to S-mode
    GEN_MRET  = 10 // MRET to M-mode
};

enum PERF_EVENT_GRP_PRD
{
    PRD_CFI      = 0, // CFI retired
    PRD_BRANCH   = 1, // Conditional branches retired
    PRD_CALL     = 2, // Calls retired
    PRD_RET      = 3, // Returns retired
    PRD_DIR_JMP  = 4, // Direct jumps(not call) retired
    PRD_REL_JMP  = 5, // Relative jumps(not return) retired
    PRD_CFI_MIS  = 6, // CFI misspredictions
    PRD_CALL_MIS = 7, // Call misspredictions
    PRD_RET_MIS  = 8, // Return misspredictions
    PRD_BR_MIS   = 9  // Branch misspredictions
};

enum PERF_EVENT_GRP_EXE
{
    EXC_INT_RTR  = 0,  // Integer non-cfi instuctions retired
    EXC_FP_RTR   = 1,  // Float-point instructions retired
    EXC_MEM_RTR  = 2,  // Memory instructions retired
    EXC_0IPC_RTR = 3,  // Number of cycles when no instruction retired
    EXC_1IPC_RTR = 4,  // Number of cycles when 1 instruction retired
    EXC_2IPC_RTR = 5,  // Number of cycles when 2 instruction retired
    EXC_3IPC_RTR = 6,  // Number of cycles when 3 instruction retired
    EXC_4IPC_RTR = 7,  // Number of cycles when 4 instruction retired
    EXC_BE_IDLE  = 8,  // Number of cycles when there is no active operation in back-end
    EXC_W_ALUMUL = 9,  // Number of cycles when oldest instruction in results queue is waiting for ALU_MUL pipeline
    EXC_W_ALUDIV = 10, // Number of cycles when oldest instruction in results queue is waiting for ALU_DIV pipeline
    EXC_W_FPU    = 11, // Number of cycles when oldest instruction in results queue is waiting for FPU pipeline
    EXC_W_LSU    = 12, // Number of cycles when oldest instruction in results queue is waiting for LSU pipeline
    EXC_W_CSR    = 13, // Number of cycles when oldest instruction in results queue is waiting for CSR pipeline
    EXC_ST_ISS   = 14, // ISS stalls
    EXC_ST_SRCT  = 15, // Stalls because of structural hazards
    EXC_ST_INT   = 16, // Stalls because of integer pipelines SCB full
    EXC_ST_LSU   = 17, // Stalls because of LSU CRQ full
    EXC_ST_FPU   = 18  // Stalls because of FPU SCB full
};

enum PERF_EVENT_GRP_L1I
{
    L1I_HIT         = 0,  // L1I hit
    L1I_MISS        = 1,  // L1I miss
    L1I_REFILL      = 2,  // L1I refill
    L1I_PREF_HIT    = 3,  // L1I prefetcher hit (prefetcher not implemented yet)
    L1I_PREF_MISS   = 4,  // L1I prefetcher miss (prefetcher not implemented yet)
    L1I_PREF_REFILL = 5,  // L1I prefetcher refill (prefetcher not implemented yet)
    L1I_TLB_ACCESS  = 6,  // L1I TLB access
    L1I_TLB_MISS    = 7,  // L1I TLB miss
    L1I_SNP         = 8,  // L1I snoop requests
    L1I_SNP_DUMMY   = 9,  // L1I dummy snoop requests
    L1I_EVICT       = 10, // L1I eviction
    L1I_NC_REQ      = 11, // L1I Non-cacheable request
    L1I_SO_REQ      = 12  // L1I Strongly ordered request
};

enum PERF_EVENT_GRP_L1D
{
    L1D_HIT         = 0,  // L1D hit
    L1D_MISS        = 1,  // L1D miss
    L1D_REFILL      = 2,  // L1D refill
    L1D_PREF_HIT    = 3,  // L1D prefetcher hit (prefetcher not implemented yet)
    L1D_PREF_MISS   = 4,  // L1D prefetcher miss (prefetcher not implemented yet)
    L1D_PREF_REFILL = 5,  // L1D prefetcher refill (prefetcher not implemented yet)
    L1D_TLB_ACCESS  = 6,  // L1D TLB access
    L1D_TLB_MISS    = 7,  // L1D TLB miss
    L1D_SNP_L1D     = 8,  // snoop requests
    L1D_SNP_DUMMY   = 9,  // L1D dummy snoop requests
    L1D_EVICT       = 10, // L1D eviction
    L1D_NC_REQ      = 11, // L1D Non-cacheable request
    L1D_SO_REQ      = 12, // L1D Strongly ordered request
    L1D_LD_SPEC     = 13, // L1D load speculatively executed
    L1D_ST_SPEC     = 14, // L1D store speculatively executed
    L1D_FENCE_SPEC  = 15, // L1D fence speculatively executed
    L1D_AMO_SPEC    = 16, // L1D amo speculatively executed
    L1D_LR_SPEC     = 17, // L1D load-reserved speculatively executed
    L1D_SC_SPEC     = 18, // L1D store-conditional speculatively executed
    L1D_CM_SPEC     = 19  // L1D cache maintenance instructions speculatively executed
};

enum PERF_EVENT_GRP_EXC
{
    TRP_IAF    = 1,  // Instruction Access Fault
    TRP_ILLEG  = 2,  // Illegal instruction
    TRP_BP     = 3,  // Breakpoint
    TRP_LAM    = 4,  // Load address misaligned
    TRP_LAF    = 5,  // Load access fault
    TRP_SAM    = 6,  // Store/AMO address misaligned
    TRP_SAF    = 7,  // Store/AMO access fault
    TRP_UECALL = 8,  // Environment call from U-mode
    TRP_SECALL = 9,  // Environment call from S-mode
    TRP_RSV0   = 10, // Reserved 0
    TRP_MECALL = 11, // Environment call from M-mode
    TRP_IPF    = 12, // Instruction page fault
    TRP_LPF    = 13, // Load Page Fault
    TRP_RSV1   = 14, // Reserved 1
    TRP_SPF    = 15  // Store/AMO Page Fault
};

enum PERF_EVENT_GRP_IRQ
{
    IRQ_RSV0 = 0,  // Reserved 0
    IRQ_SSI  = 1,  // S-mode SW Interrupt
    IRQ_RSV1 = 2,  // Reserved 1
    IRQ_MSI  = 3,  // M-mode SW Interrupt
    IRQ_RSV2 = 4,  // Reserved 2
    IRQ_STI  = 5,  // S-mode Timer Interrupt
    IRQ_RSV3 = 6,  // Reserved 3
    IRQ_MTI  = 7,  // M-mode Timer Interrupt
    IRQ_RSV4 = 8,  // Reserved 4
    IRQ_SEI  = 9,  // S-mode External Interrupt
    IRQ_RSV5 = 10, // Reserved 5
    IRQ_MEI  = 11  // M-mode External Interrupt
};

enum PERF_EVENT_GRP_L1D_PREF
{
    L1D_PF_ISS = 0,     // Issued instruction arrives to prefetcher
    L1D_PF_REQ = 1,     // Prefetch request issued
    L1D_PF_ISS_HIT = 2, // Issued instruction hit one of the prefetcher's entries
    L1D_PF_FLT_HIT = 3, // Prefetch request hit the same address as some of already waiting in queue prefetches
    L1D_PF_HIT_SM = 4,  // Issued instruction hit one of the prefetcher's entries but the address doesn't match the predicted one (stride miss)
    L1D_PF_HIT_DC = 5,  // Prefetch hit cache
    L1D_PF_HIT_CLB = 6, // Prefetch hit CLB
    L1D_PF_CANCEL = 7,  // Prefetch cancelled
    L1D_PF_ISS_INV = 8  // Prefetch request that was waiting in queue is invalidated
};

enum PERF_EVENT_GRP_HPW
{
    HPW_4K_HIT       = 0, // Counts outgoing 4k pages
    HPW_MP_HIT       = 1, // Counts outgoing 2M pages
    HPW_GP_HIT       = 2, // Counts outgoing 1G pages
    HPW_ITLB_REQ     = 3, // Counts iTLB page walk requests
    HPW_DTLB_REQ     = 4, // Counts dTLB page walk requests
    HPW_PREF_REQ     = 5, // Counts prefetcher page walk requests
    HPW_PAGE_FAULT   = 6, // Counts page fault errors
    HPW_ACCESS_FAULT = 7, // Counts access fault errors
    HPW_NO_ERR       = 8, // Counts responses without errors
    HPW_TP_HIT       = 9, // Counts outgoing 1T pages
    HPW_PP_HIT       = 10 // Counts outgoing 1P pages
};

enum PERF_EVENT_GRP_STLB
{
    STLB_REQ                 = 0,  // Counts sTLB acesses from uTLB
    STLB_REFILL              = 1,  // Counts sTLB refills
    STLB_ITLB_REQ            = 2,  // Counts sTLB acesses from iTLB
    STLB_DTLB_REQ            = 3,  // Counts sTLB acesses from dTLB
    STLB_ITLB_FLUSH          = 4,  // Counts flush requests for iTLB
    STLB_DTLB_FLUSH          = 5,  // Counts flush requests for dTLB
    STLB_ITLB_MISS           = 6,  // Counts sTLB misses on iTLB request
    STLB_DTLB_MISS           = 7,  // Counts sTLB misses on dTLB request
    STLB_ITLB_STALL          = 8,  // Counts how many times iTLB requests were stalled for processing SFENCE requests
    STLB_DTLB_STALL          = 9,  // Counts how many times dTLB requests were stalled for processing SFENCE requests
    STLB_HPW_STALL           = 10, // Counts how many times HPW requests were stalled for processing SFENCE requests
    STLB_SFENCE_STALL        = 11, // Counts how many times the processing of SFENCE requests has been stalled
    STLB_SFENCE_FLUSH_ALL    = 12, // Counts SFENCE flush all requests
    STLB_SFENCE_FLUSH_VPN    = 13, // Counts SFENCE flush with matching VPN requests
    STLB_SFENCE_FLUSH_ASID   = 14, // Counts SFENCE flush with matching ASID requests
    STLB_SFENCE_FLUSH_TARGET = 15, // Counts SFENCE flush with matching ASID and VPN requests
    STLB_RESP_WO_ERR         = 16, // Counts sTLB responses without errors
    STLB_RESP_ANY            = 17  // Counts all sTLB responses
};

enum PERF_EVENT_GRP_PREF
{
    PREF_REQ      = 0, // Counts prefetcher accesses
    PREF_HIT      = 1, // Counts prefetcher hits
    PREF_HPW_REQ  = 2, // Counts prefetcher requests to HPW
    PREF_LATE_REQ = 3  // Counts prefetcher late requests
};

enum PERF_EVENT_GRP_HPWX
{
    HPW0_4K_REQ  = 0,  // Counts 4K PTE mem requests from HPW0
    HPW0_2M_REQ  = 1,  // Counts 2M PTE mem requests from HPW0
    HPW0_1G_REQ  = 2,  // Counts 1G PTE mem requests from HPW0
    HPW0_4K_WAIT = 3,  // Counts wait cycles for 4K PTE mem requests from HPW0
    HPW0_2M_WAIT = 4,  // Counts wait cycles for 2M PTE mem requests from HPW0
    HPW0_1G_WAIT = 5,  // Counts wait cycles for 1G PTE mem requests from HPW0
    HPW1_4K_REQ  = 6,  // Counts 4K PTE mem requests from HPW1
    HPW1_2M_REQ  = 7,  // Counts 2M PTE mem requests from HPW1
    HPW1_1G_REQ  = 8,  // Counts 1G PTE mem requests from HPW1
    HPW1_4K_WAIT = 9,  // Counts wait cycles for 4K PTE mem requests from HPW1
    HPW1_2M_WAIT = 10, // Counts wait cycles for 2M PTE mem requests from HPW1
    HPW1_1G_WAIT = 11, // Counts wait cycles for 1G PTE mem requests from HPW1
    HPW2_4K_REQ  = 12, // Counts 4K PTE mem requests from HPW2
    HPW2_2M_REQ  = 13, // Counts 2M PTE mem requests from HPW2
    HPW2_1G_REQ  = 14, // Counts 1G PTE mem requests from HPW2
    HPW2_4K_WAIT = 15, // Counts wait cycles for 4K PTE mem requests from HPW2
    HPW2_2M_WAIT = 16, // Counts wait cycles for 2M PTE mem requests from HPW2
    HPW2_1G_WAIT = 17, // Counts wait cycles for 1G PTE mem requests from HPW2
    HPW3_4K_REQ  = 18, // Counts 4K PTE mem requests from HPW3
    HPW3_2M_REQ  = 19, // Counts 2M PTE mem requests from HPW3
    HPW3_1G_REQ  = 20, // Counts 1G PTE mem requests from HPW3
    HPW3_4K_WAIT = 21, // Counts wait cycles for 4K PTE mem requests from HPW3
    HPW3_2M_WAIT = 22, // Counts wait cycles for 2M PTE mem requests from HPW3
    HPW3_1G_WAIT = 23  // Counts wait cycles for 1G PTE mem requests from HPW3
};

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

static inline void perf_init_hpm()
{
    // Activate dedicated counters (CYCLE, TIME, INSTRET)
    write_csr(mcountinhibit, -1ULL << 3);

    // Enable access to all counters
    write_csr(mcounteren, -1ULL);
}

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__

#endif // SCR_BSP_PERF_H
