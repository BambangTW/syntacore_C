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
/// @brief MRT defs and funcs

#ifndef SCR_BSP_MRT_H
#define SCR_BSP_MRT_H

#include <stddef.h>

#include "arch.h"
#include "mpu.h"
#include "pmp.h"

#if PLF_MRT_SUPPORT

#define CLUSTER_CSR_MRT_BASE    (PLF_CLUSTER_CFG_BASE + 0x2000)
#define MRT_VID                 (CLUSTER_CSR_MRT_BASE)
#define MRT_CMD                 (CLUSTER_CSR_MRT_BASE + 8)
#define MRT_CMD_RUN_BIT         (1)
#define MRT_CTRL_MTYPE_OFFS     (16)
#define MRT_CTRL_MTYPE_MASK     (~(3 << MRT_CTRL_MTYPE_OFFS))
#define MRT_CTRL_MTYPE_C_WO     (0 << MRT_CTRL_MTYPE_OFFS)
#define MRT_CTRL_MTYPE_NC_SO    (1 << MRT_CTRL_MTYPE_OFFS)
#define MRT_CTRL_MTYPE_NC_WO    (2 << MRT_CTRL_MTYPE_OFFS)
#define MRT_CTRL_MTYPE_CPU_CFG  (3 << MRT_CTRL_MTYPE_OFFS)

#define MRT_S_ENTRY_0_CTRL      (CLUSTER_CSR_MRT_BASE + 0x400 + 0 * sizeof(size_t))
#define MRT_S_ENTRY_0_ADDR      (CLUSTER_CSR_MRT_BASE + 0x400 + 1 * sizeof(size_t))
#define MRT_S_ENTRY_0_MASK      (CLUSTER_CSR_MRT_BASE + 0x400 + 2 * sizeof(size_t))
#define MRT_S_ENTRY_NEXT_OFFS   (0x20)
#define MRT_S_ENTRY_N_CTRL(N)   (MRT_S_ENTRY_0_CTRL + (N) * MRT_S_ENTRY_NEXT_OFFS)
#define MRT_S_ENTRY_N_ADDR(N)   (MRT_S_ENTRY_0_ADDR + (N) * MRT_S_ENTRY_NEXT_OFFS)
#define MRT_S_ENTRY_N_MASK(N)   (MRT_S_ENTRY_0_MASK + (N) * MRT_S_ENTRY_NEXT_OFFS)


#define MRT_A_ENTRY_0_CTRL      (CLUSTER_CSR_MRT_BASE + 0x800 + 0 * sizeof(size_t))
#define MRT_A_ENTRY_0_ADDR      (CLUSTER_CSR_MRT_BASE + 0x800 + 1 * sizeof(size_t))
#define MRT_A_ENTRY_0_MASK      (CLUSTER_CSR_MRT_BASE + 0x800 + 2 * sizeof(size_t))
#define MRT_A_ENTRY_NEXT_OFFS   (0x20)
#define MRT_A_ENTRY_N_CTRL(N)   (MRT_A_ENTRY_0_CTRL + (N) * MRT_A_ENTRY_NEXT_OFFS)
#define MRT_A_ENTRY_N_ADDR(N)   (MRT_A_ENTRY_0_ADDR + (N) * MRT_A_ENTRY_NEXT_OFFS)
#define MRT_A_ENTRY_N_MASK(N)   (MRT_A_ENTRY_0_MASK + (N) * MRT_A_ENTRY_NEXT_OFFS)

#define CLUSTER_CSR_MPORT       (0xF000000)

#define MRT_ENTRY_VALID         (1)

#define MRT_ENTRY_START         (1)

static inline void create_mrt_region(unsigned entry, size_t base, size_t size, size_t attr)
{
    *(volatile size_t*)MRT_S_ENTRY_N_ADDR(entry) = base;
    *(volatile size_t*)MRT_S_ENTRY_N_MASK(entry) = ~(size - 1);
    *(volatile size_t*)MRT_S_ENTRY_N_CTRL(entry) = attr;
}

static inline void setup_mrt_regions(const scr_mem_region_info* regions, unsigned region_count)
{
    // First region is initialized and reserved after reset
    unsigned rn = MRT_ENTRY_START;

    for (const scr_mem_region_info* rgn = regions; rgn < regions + region_count; ++rgn)
    {
        if (rgn->attr)
        {
            create_mrt_region(rn++, rgn->base, rgn->size, rgn->mrt_attr | MRT_ENTRY_VALID);
            ifence();
        }
    }

    *(volatile size_t*)MRT_CMD = MRT_CMD_RUN_BIT;
    while((*(volatile size_t*)MRT_CMD & MRT_CMD_RUN_BIT) == 1);
    ifence();
}

static inline void mrt_init(void)
{
#ifdef PLF_MEM_MAP
    setup_mrt_regions(mem_regions, mem_region_count);
#endif // PLF_MEM_MAP
}

#endif // PLF_MRT_SUPPORT

#endif // SCR_BSP_MRT_H
