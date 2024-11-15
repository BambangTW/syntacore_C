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
/// @brief PMU API definitions
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

#ifndef SCR_BSP_PMU_H
#define SCR_BSP_PMU_H

#if __riscv_xlen == 64

#include "pmu_csr.h"

#include <stddef.h>

#define PMUC_MAX_CSR_EVENT_COUNT 29
#define PMUC_MAX_L2_EVENT_COUNT 4
#define PMUC_MAX_L3_EVENT_COUNT 4
#define PMUC_MAX_EVENT_COUNT (PMUC_MAX_CSR_EVENT_COUNT + PMUC_MAX_L2_EVENT_COUNT + PMUC_MAX_L3_EVENT_COUNT)

enum
{
    PMUC_R_OK = 0,
    PMUC_R_INVALID_ID,
    PMUC_R_DUPLICATE_ID,
    PMUC_R_CSR_COUNTERS_LIMIT,
    PMUC_R_L2_COUNTERS_LIMIT,
    PMUC_R_L3_COUNTERS_LIMIT
};

#ifdef PLF_L3CTL_BASE

// L3C_PCEN_CTRL
#define L3C_PCE_CTRL0   (PLF_L3CTL_BASE + 0x0098)

#define L3_PCE_STEP     (0x100)

#define L3B_PCE_CNTR0   (PLF_L3CTL_BASE + 0x01b0)

#define L3_COUNTER(bank, idx) \
    ( *(volatile uint64_t*)(L3B_PCE_CNTR0 + idx * L3B_PCE_CNTR_STEP + bank * L3_PCE_STEP) )

#endif // PLF_L3CTL_BASE

void pmuc_setup_selected_counters(void);

void pmuc_start_selected_l2_counters(void);
void pmuc_start_selected_l3_counters(void);
void pmuc_stop_selected_l2_counters(void);
void pmuc_stop_selected_l3_counters(void);

static inline void pmuc_start_selected_counters(void)
{
#ifdef PLF_L3CTL_BASE
    pmuc_start_selected_l3_counters();
#endif // PLF_L3CTL_BASE
#ifdef PLF_L2CTL_BASE
    pmuc_start_selected_l2_counters();
#endif // PLF_L2CTL_BASE
    pmuc_start_selected_csr_counters();
}

static inline void pmuc_stop_all_counters(void)
{
    pmuc_stop_selected_csr_counters();
#ifdef PLF_L2CTL_BASE
    pmuc_stop_selected_l2_counters();
#endif // PLF_L2CTL_BASE
#ifdef PLF_L3CTL_BASE
    pmuc_stop_selected_l3_counters();
#endif // PLF_L3CTL_BASE
}

void pmuc_enable_cycles(void);
int pmuc_add_counter(const char* name);
void pmuc_update_counters(void);
size_t pmuc_get_available_csr_counters_num(void);
size_t pmuc_get_selected_counters_num(void);

// Make sure that index is less than pmuc_get_selected_counters_num() value
uint64_t pmuc_get_counter_value(size_t index);
const char* pmuc_get_counter_name(size_t index);

#endif // __riscv_xlen == 64

#endif // SCR_BSP_PMU_H

