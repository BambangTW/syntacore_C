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
/// @brief Cluster control funcs
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

#include "drivers/ctrl_l3.h"

#if PLF_CLUSTER_RESET_CORES

#include "arch.h"
#include "drivers/cache.h"
#include "drivers/feature_enable.h"
#include "drivers/iccm_l3.h"
#include "drivers/pmp.h"
#include <stddef.h>

extern volatile void* hart_start_table[PLF_SMP_HART_NUM];

/**
 * \brief Startup activation for non-master CPUs
 *
 * \param None
 * \return None
 */
void plf_ext_cpu_activate_all (void)
{
    extern uint64_t _bsp_start;
    unsigned long slots_num = iccm_l3_get_harts_num();
    for (uint8_t cpu_num = 1; cpu_num < slots_num; ++cpu_num) {
        // Set Hart IDs
        *(volatile uint64_t*)(CL_CPU_N_MHARTID(cpu_num)) = cpu_num;
        // Set Jtag IDs
        *(volatile uint64_t*)(CL_CPU_N_JTAGIDCODE(cpu_num)) = cpu_num;
        // Set Start PC
        *(volatile uint64_t*)(CL_CPU_N_RESETPC(cpu_num)) = (uint64_t)&_bsp_start;
    }
    // Turn on CPU
    for (uint8_t cpu_num = 1; cpu_num < slots_num; ++cpu_num) {
        plf_ext_cpu_activate(cpu_num);
    }
}

/**
 * \brief CPU self side steps for deactivation
 *
 * \param None
 * \return None
 */
void plf_local_cpu_deactivation_prepare (void)
{
    // Actions according L3 Cluster External Architecture Specification
    // Mask all interrupts
    write_csr(mstatus, 0x0);
    write_csr(mie, 0x0);
    // Disable prefetchers and loop predictor
    plf_bpu_loop_predictor_disable();
    plf_page_prefetcher_disable();
    plf_l1cache_prefetcher_disable();
    // Disable L1 caches
    plf_l1cache_disable();
    // Disable L2 cache
    plf_l2cache_disable();
    // Jump to WFI
    wfi();
}

/**
 * \brief Actions from other CPU to deactivate target CPU
 *
 * \param cpu_id number of CPU to deactivate
 * \return None
 */
void plf_ext_cpu_deactivate (uint32_t cpu_id)
{
    // Check: can't deactivate self
    if (cpu_id == read_csr(mhartid)) {
        return;
    }
    // Actions according L3 Cluster External Architecture Specification
    // Read spin until CPU WFI state
    while (!(*(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) & CL_CPU_CTRL_WFI_BIT));
    // Read spin until the end of CPU's side activity
    while (*(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) & CL_CPU_CTRL_ACT_BIT);
    // Turn off CHI link
    *(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) &= ~CL_CPU_CTRL_LINKUP_BIT;
    while (*(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) & CL_CPU_CTRL_LINK_STS_MASK);
    // Assert CPU reset
    *(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) &= ~CL_CPU_CTRL_RESET_N_BIT;
    // Bypass CPU-TAP controller, turn off power is not implemented here
}

/**
 * \brief Actions from other CPU to activate target CPU
 *
 * \param cpu_id number of CPU to activate
 * \return None
 */
void plf_ext_cpu_activate (uint32_t cpu_id)
{
    // Actions according L3 Cluster External Architecture Specification
    // First, power must be turned on and TAP Controller must be reset
    // Deassert CPU reset, Turn ON CHI Link
    *(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) |= (CL_CPU_CTRL_RESET_N_BIT | CL_CPU_CTRL_LINKUP_BIT);
    while ((*(volatile uint64_t*)CL_CPU_N_CTRL(cpu_id) & CL_CPU_CTRL_LINK_STS_MASK) != CL_CPU_CTRL_LINK_STS_MASK);
}

#endif // PLF_CLUSTER_RESET_CORES
