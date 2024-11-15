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

#ifndef PLATFORM_SCR9_L2_MPU_AIA_H
#define PLATFORM_SCR9_L2_MPU_AIA_H

#include "plf_scr9.h"

#define PLF_MPU_SUPPORT        (1)

#define PLF_PMP_SUPPORT        (0)

#define PLF_SMP_SUPPORT        (1)

#define PLF_INIT_REGS          (1)

//----------------------
// memory configuration
//----------------------

#define PLF_MEM_BASE     (0)
#define PLF_MEM_SIZE     (4UL*1024*1024*1024)
#define PLF_MEM_ATTR     (SCR_MPU_CTRL_MT_WEAKLY | SCR_MPU_CTRL_ALL)
#define PLF_MEM_NAME     "DDR"

#define PLF_MMCFG_BASE   EXPAND32ADDR(0xf0040000)
#define PLF_MMCFG_SIZE   (64*1024)
#define PLF_MMCFG_ATTR   (SCR_MPU_CTRL_MT_CFG | \
                          SCR_MPU_CTRL_MR |     \
                          SCR_MPU_CTRL_MW)
#define PLF_MMCFG_NAME   "MMCFG"

#define PLF_MTIMER_BASE  PLF_MMCFG_BASE
#define PLF_L2CTL_BASE   (PLF_MMCFG_BASE + 0x1000)

#define PLF_APLIC_BASE_M  (0xffffff0c000000)
#define PLF_APLIC_SIZE_M  (16*1024)
#define PLF_APLIC_ATTR_M  (SCR_MPU_CTRL_MT_STRONG |  \
                         SCR_MPU_CTRL_MR |         \
                         SCR_MPU_CTRL_MW |         \
                         SCR_MPU_CTRL_SR |         \
                         SCR_MPU_CTRL_SW)
#define PLF_APLIC_NAME_M   "APLIC_M"

#define PLF_APLIC_BASE_S  (0xffffff0d000000)
#define PLF_APLIC_SIZE_S  (16*1024)
#define PLF_APLIC_ATTR_S  (SCR_MPU_CTRL_MT_STRONG |  \
                         SCR_MPU_CTRL_MR |         \
                         SCR_MPU_CTRL_MW |         \
                         SCR_MPU_CTRL_SR |         \
                         SCR_MPU_CTRL_SW)
#define PLF_APLIC_NAME_S   "APLIC_S"

#define PLF_IMSIC_BASE_M  (0xffffff24000000)
#define PLF_IMSIC_SIZE_M  (8*4*1024)
#define PLF_IMSIC_ATTR_M  (SCR_MPU_CTRL_MT_STRONG |  \
                           SCR_MPU_CTRL_MR |         \
                           SCR_MPU_CTRL_MW |         \
                           SCR_MPU_CTRL_SR |         \
                           SCR_MPU_CTRL_SW)
#define PLF_IMSIC_NAME_M   "IMSIC_M"

#define PLF_IMSIC_BASE_S  (0xffffff28000000)
#define PLF_IMSIC_SIZE_S  (8*4*1024)
#define PLF_IMSIC_ATTR_S  (SCR_MPU_CTRL_MT_STRONG |  \
                           SCR_MPU_CTRL_MR |         \
                           SCR_MPU_CTRL_MW |         \
                           SCR_MPU_CTRL_SR |         \
                           SCR_MPU_CTRL_SW)
#define PLF_IMSIC_NAME_S   "IMSIC_S"

#define PLF_MMIO_BASE    EXPAND32ADDR(0xff000000)
#define PLF_MMIO_SIZE    (8*1024*1024)
#define PLF_MMIO_ATTR    (SCR_MPU_CTRL_MT_STRONG |  \
                          SCR_MPU_CTRL_MR |         \
                          SCR_MPU_CTRL_MW |         \
                          SCR_MPU_CTRL_SR |         \
                          SCR_MPU_CTRL_SW)
#define PLF_MMIO_NAME    "MMIO"

#define PLF_OCRAM_BASE   EXPAND32ADDR(0xffff0000)
#define PLF_OCRAM_SIZE   (64*1024)
#define PLF_OCRAM_ATTR   (SCR_MPU_CTRL_MT_WEAKLY | \
                          SCR_MPU_CTRL_MA | \
                          SCR_MPU_CTRL_SR)
#define PLF_OCRAM_NAME   "On-Chip RAM"

#define PLF_MEM_MAP                                                   \
    {PLF_MEM_BASE, PLF_MEM_SIZE, PLF_MEM_ATTR, PLF_MEM_NAME},         \
    {PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR, PLF_MMCFG_NAME}, \
    {PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR, PLF_MMIO_NAME},     \
    {PLF_OCRAM_BASE, PLF_OCRAM_SIZE, PLF_OCRAM_ATTR, PLF_OCRAM_NAME}, \
    {PLF_APLIC_BASE_M, PLF_APLIC_SIZE_M, PLF_APLIC_ATTR_M, PLF_APLIC_NAME_M},   \
    {PLF_APLIC_BASE_S, PLF_APLIC_SIZE_S, PLF_APLIC_ATTR_S, PLF_APLIC_NAME_S},   \
    {PLF_IMSIC_BASE_M, PLF_IMSIC_SIZE_M, PLF_IMSIC_ATTR_M, PLF_IMSIC_NAME_M}, \
    {PLF_IMSIC_BASE_S, PLF_IMSIC_SIZE_S, PLF_IMSIC_ATTR_S, PLF_IMSIC_NAME_S}


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

#define PLF_APLIC_NUM_SOURCE 96

#define PLF_IMSIC_NUM_IDS 127

#define PLF_APLIC_IRQ_CFG                                \
    {PLF_INTLINE_UART0, APLIC_SOURCECFG_SM_LEVEL1}

#endif // PLATFORM_SCR9_L2_MPU_AIA_H
