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
/// @brief `Feature enable` register options
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

#ifndef SCR_BSP_FEATURE_ENABLE_H
#define SCR_BSP_FEATURE_ENABLE_H

#include "arch.h"

#include <stdbool.h>

#define SCR_CSR_FEATURE_ENABLE 0xBE0

#define MISALIGNED_ACCESS_ENABLE_BIT    (1 << 9)
#define L1D_STORE_MERGE_ENABLE_BIT      (1 << 11)
#define BPU_EARLY_BRANCH_RESOLUTION_BIT (1 << 13)
#define BPU_LOOP_PREDICTOR_ENABLE_BIT   (1 << 15)
#define L1I_PREFETCHER_ENABLE_BIT       (1 << 17)
#define PAGE_PREFETCHER_ENABLE_BIT      (1 << 20)

static inline __attribute__((always_inline))
void plf_feature_reset(uint64_t feature_bit)
{
    const bool enabled = read_csr(SCR_CSR_FEATURE_ENABLE) & feature_bit;

    clear_csr(SCR_CSR_FEATURE_ENABLE, feature_bit);
    set_csr(SCR_CSR_FEATURE_ENABLE, feature_bit);

    if (enabled == false) {
        clear_csr(SCR_CSR_FEATURE_ENABLE, feature_bit);
    }
}

static inline __attribute__((always_inline))
void plf_page_prefetcher_reset()
{
    plf_feature_reset(PAGE_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_page_prefetcher_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, PAGE_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_page_prefetcher_disable(void)
{
#if defined(PAGE_PREFETCHER)
    clear_csr(SCR_CSR_FEATURE_ENABLE, PAGE_PREFETCHER_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, PAGE_PREFETCHER_ENABLE_BIT);
#endif // PAGE_PREFETCHER
}

static inline __attribute__((always_inline))
bool plf_page_prefetcher_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & PAGE_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
bool plf_page_prefetcher_is_available(void)
{
    return ((arch_coreid() == 7) || (arch_coreid() == 9));
}

static inline __attribute__((always_inline))
void plf_bpu_early_branch_resolution_reset()
{
    plf_feature_reset(BPU_EARLY_BRANCH_RESOLUTION_BIT);
}

static inline __attribute__((always_inline))
void plf_bpu_early_branch_resolution_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, BPU_EARLY_BRANCH_RESOLUTION_BIT);
}

static inline __attribute__((always_inline))
void plf_bpu_early_branch_resolution_disable(void)
{
#if defined(BPU_EARLY_BRANCH_RESOLUTION)
    clear_csr(SCR_CSR_FEATURE_ENABLE, BPU_EARLY_BRANCH_RESOLUTION_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, BPU_EARLY_BRANCH_RESOLUTION_BIT);
#endif // BPU_EARLY_BRANCH_RESOLUTION
}

static inline __attribute__((always_inline))
bool plf_bpu_early_branch_resolution_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & BPU_EARLY_BRANCH_RESOLUTION_BIT);
}

static inline __attribute__((always_inline))
bool plf_bpu_early_branch_resolution_is_available(void)
{
    return (arch_coreid() == 9);
}

static inline __attribute__((always_inline))
void plf_bpu_loop_predictor_reset()
{
    plf_feature_reset(BPU_LOOP_PREDICTOR_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_bpu_loop_predictor_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, BPU_LOOP_PREDICTOR_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_bpu_loop_predictor_disable(void)
{
#if defined(BPU_LOOP_PREDICTOR)
    clear_csr(SCR_CSR_FEATURE_ENABLE, BPU_LOOP_PREDICTOR_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, BPU_LOOP_PREDICTOR_ENABLE_BIT);
#endif // BPU_LOOP_PREDICTOR
}

static inline __attribute__((always_inline))
bool plf_bpu_loop_predictor_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & BPU_LOOP_PREDICTOR_ENABLE_BIT);
}

static inline __attribute__((always_inline))
bool plf_bpu_loop_predictor_is_available(void)
{
    return (arch_coreid() == 9);
}

static inline __attribute__((always_inline))
void plf_l1icache_prefetcher_reset()
{
    plf_feature_reset(L1I_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1icache_prefetcher_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, L1I_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1icache_prefetcher_disable(void)
{
#if defined(L1I_PREFETCHER)
    clear_csr(SCR_CSR_FEATURE_ENABLE, L1I_PREFETCHER_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, L1I_PREFETCHER_ENABLE_BIT);
#endif // L1I_PREFETCHER
}

static inline __attribute__((always_inline))
bool plf_l1icache_prefetcher_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & L1I_PREFETCHER_ENABLE_BIT);
}

static inline __attribute__((always_inline))
bool plf_l1icache_prefetcher_is_available(void)
{
    return (arch_coreid() == 9);
}

static inline __attribute__((always_inline))
void plf_misaligned_access_reset()
{
    plf_feature_reset(MISALIGNED_ACCESS_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_misaligned_access_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, MISALIGNED_ACCESS_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_misaligned_access_disable(void)
{
#if defined(MISALIGNED_ACCESS)
    clear_csr(SCR_CSR_FEATURE_ENABLE, MISALIGNED_ACCESS_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, MISALIGNED_ACCESS_ENABLE_BIT);
#endif // MISALIGNED_ACCESS
}

static inline __attribute__((always_inline))
bool plf_misaligned_access_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & MISALIGNED_ACCESS_ENABLE_BIT);
}

static inline __attribute__((always_inline))
bool plf_misaligned_access_is_available(void)
{
    return ((arch_coreid() == 7) || (arch_coreid() == 9));
}

static inline __attribute__((always_inline))
void plf_l1cache_store_merge_reset()
{
    plf_feature_reset(L1D_STORE_MERGE_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_store_merge_enable(void)
{
    set_csr(SCR_CSR_FEATURE_ENABLE, L1D_STORE_MERGE_ENABLE_BIT);
}

static inline __attribute__((always_inline))
void plf_l1cache_store_merge_disable(void)
{
#if defined(L1D_STORE_MERGE)
    clear_csr(SCR_CSR_FEATURE_ENABLE, L1D_STORE_MERGE_ENABLE_BIT);
#else
    // The dummy operation using `mhpmevent3` register to keep a fixed memory layout
    // regardless of a feature set. The register is later setup in a client code
    clear_csr(mhpmevent3, L1D_STORE_MERGE_ENABLE_BIT);
#endif // L1D_STORE_MERGE
}

static inline __attribute__((always_inline))
bool plf_l1cache_store_merge_is_enabled(void)
{
    return (read_csr(SCR_CSR_FEATURE_ENABLE) & L1D_STORE_MERGE_ENABLE_BIT);
}

static inline __attribute__((always_inline))
bool plf_l1cache_store_merge_is_available(void)
{
    return (arch_coreid() == 9);
}

#endif // SCR_BSP_FEATURE_ENABLE_H
