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
/// @brief platform specific configurations
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

#ifndef PLATFORM_SCR7_L2_H
#define PLATFORM_SCR7_L2_H

#include "plf_scr7.h"

#define PLF_MPU_SUPPORT        (0)

#define PLF_PMP_SUPPORT        (1)

#define PLF_SMP_SUPPORT        (1)

#define PLF_INIT_REGS          (1)

//----------------------
// memory configuration
//----------------------

#define PLF_MEM_BASE     (0)
#define PLF_MEM_SIZE     (4UL*1024*1024*1024)
#define PLF_MEM_ATTR     (SCR_PMP_CTRL_MT_WEAKLY | PMP_RWX)
#define PLF_MEM_NAME     "DDR"

#define PLF_MMCFG_BASE   EXPAND32ADDR(0xf0040000)
#define PLF_MMCFG_SIZE   (8*1024)
#define PLF_MMCFG_ATTR   (SCR_PMP_CTRL_MT_CONFIG)
#define PLF_MMCFG_NAME   "MMCFG"

#define PLF_MTIMER_BASE  PLF_MMCFG_BASE
#define PLF_L2CTL_BASE   (PLF_MMCFG_BASE + 0x1000)

#define PLF_PLIC_BASE    EXPAND32ADDR(0xfe000000)
#define PLF_PLIC_SIZE    (16*1024*1024)
#define PLF_PLIC_ATTR    (SCR_PMP_CTRL_MT_STRONG |  \
                          PMP_R |                   \
                          PMP_W)
#define PLF_PLIC_NAME    "PLIC"

#define PLF_MMIO_BASE    EXPAND32ADDR(0xff000000)
#define PLF_MMIO_SIZE    (8*1024*1024)
#define PLF_MMIO_ATTR    (SCR_PMP_CTRL_MT_STRONG |  \
                          PMP_R |                   \
                          PMP_W)
#define PLF_MMIO_NAME    "MMIO"

#define PLF_OCRAM_BASE   EXPAND32ADDR(0xffff0000)
#define PLF_OCRAM_SIZE   (64*1024)
#define PLF_OCRAM_ATTR   (SCR_PMP_CTRL_MT_WEAKLY |  \
                          PMP_R |                   \
                          PMP_W)
#define PLF_OCRAM_NAME   "On-Chip RAM"

#define PLF_MEM_MAP                                                   \
    {PLF_MEM_BASE, PLF_MEM_SIZE, PLF_MEM_ATTR, PLF_MEM_NAME},         \
    {PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR, PLF_MMCFG_NAME}, \
    {PLF_PLIC_BASE, PLF_PLIC_SIZE, PLF_PLIC_ATTR, PLF_PLIC_NAME},     \
    {PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR, PLF_MMIO_NAME},     \
    {PLF_OCRAM_BASE, PLF_OCRAM_SIZE, PLF_OCRAM_ATTR, PLF_OCRAM_NAME}

#define PLF_EARLY_MEM_MAP                                             \
    {PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR, PLF_MMCFG_NAME}


//----------------------
// cache configuration
//----------------------

// min cache line
#define PLF_CACHELINE_SIZE 16
// global configuration: cachable
#define PLF_CACHE_CFG CACHE_GLBL_ENABLE

//----------------------
// MMIO configuration
//----------------------

// FPGA UART port
#define PLF_UART0_BASE   (PLF_MMIO_BASE + 0x10000)
#define PLF_UART0_16550
// FPGA build ID
#define PLF_BLD_ID_ADDR  (PLF_MMIO_BASE)
// FPGA sysclk, MHz
#define PLF_SYSCLK_MHZ_ADDR (PLF_MMIO_BASE + 0x1000)
#define VarClkMHz (*(const uint32_t*)(PLF_SYSCLK_MHZ_ADDR) * 1000000)
#ifndef PLF_SYS_CLK
#define PLF_SYS_CLK VarClkMHz
#endif

// external interrupt lines
#define PLF_INTLINE_UART0   1

#define PLF_PLIC_IRQ_CFG                                \
    {PLF_INTLINE_UART0, PLIC_SRC_MODE_LEVEL_HIGH}

#endif // PLATFORM_SCR7_L2_H
