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
/// @brief Cluster control defs and inline funcs

#ifndef SCR_BSP_CLUSTER_CONTROL_H
#define SCR_BSP_CLUSTER_CONTROL_H

#include "plf.h"

#if PLF_CLUSTER_RESET_CORES

#include <stdint.h>

/* CPU External Control CSRs */
#define PLF_CLUSTER_CPU_BASE            (PLF_CLUSTER_CFG_BASE + 4 * 1024)   // CPU External Control CSRs memory base
#define CL_CPU_DESCR                    (PLF_CLUSTER_CPU_BASE + 0)    	    // Description register (common for all CPUs)
#define CL_CPU_0_CTRL                   (PLF_CLUSTER_CPU_BASE + 0x40) 	    // CPU0 Control Register
#define CL_CPU_0_MHARTID                (PLF_CLUSTER_CPU_BASE + 0x48) 	    // CPU0 MHARTID Register
#define CL_CPU_0_RESETPC                (PLF_CLUSTER_CPU_BASE + 0x50)       // CPU0 PC to start after reset
#define CL_CPU_0_JTAGIDCODE             (PLF_CLUSTER_CPU_BASE + 0x58)       // CPU0 JTAG ID code
#define CL_CPU_N_CTRL(N)                (CL_CPU_0_CTRL + (N) * 0x40)        // CPUn Control Register (n=0..7)
#define CL_CPU_N_MHARTID(N)             (CL_CPU_0_MHARTID + (N) * 0x40)     // CPUn MHARTID Register (n=0..7)
#define CL_CPU_N_RESETPC(N)             (CL_CPU_0_RESETPC + (N) * 0x40)     // CPUn PC to start after reset (n=0..7)
#define CL_CPU_N_JTAGIDCODE(N)          (CL_CPU_0_JTAGIDCODE + (N) * 0x40)  // CPUn JTAG ID code (n=0..7)

/* CPUn Control Register bits */
#define CL_CPU_CTRL_RESET_N_BIT                 (1 << 0)      // This bit corresponds to rst_n pin of CPU
#define CL_CPU_CTRL_WFI_BIT                     (1 << 9)      // CPU WFI status
#define CL_CPU_CTRL_ACT_BIT                     (1 << 10)     // CPU is active
#define CL_CPU_CTRL_LINKUP_BIT                  (1ULL << 32)  // CPU CHI-Link activation enable/disable control

#define CL_CPU_CTRL_LINK_STS_MASK               (15ULL << 40)  // Mask for 4-bit link status

void plf_ext_cpu_activate_all (void);
void plf_local_cpu_deactivation_prepare (void);
void plf_ext_cpu_deactivate (uint32_t cpu_id);
void plf_ext_cpu_activate (uint32_t cpu_id);

#endif // PLF_CLUSTER_RESET_CORES
#endif // SCR_BSP_CLUSTER_CONTROL_H
