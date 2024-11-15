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

#ifndef PLATFORM_SCR7_L3_MPU_LEGACY_H
#define PLATFORM_SCR7_L3_MPU_LEGACY_H

#include <plf_scr7.h>

#define PLF_MPU_SUPPORT        (1)

#define PLF_PMP_SUPPORT        (0)

#define PLF_MRT_SUPPORT        (1)

#define PLF_SMP_SUPPORT        (1)

#define PLF_CACHE_L2_DEDICATED (1)

#define PLF_ICCM_L3_SUPPORT    (1)

#define PLF_RTC_SRC_EXTERNAL   (1)

#define PLF_INIT_REGS          (1)

//----------------------
// memory configuration
//----------------------

#define PLF_DEFAULT_MPORT      (0)

#define PLF_MEM_BASE           (0UL)
#define PLF_MEM_SIZE           (4UL*1024UL*1024UL*1024UL)
#define PLF_MEM_ATTR           (SCR_MPU_CTRL_MT_WEAKLY | SCR_MPU_CTRL_ALL)
#define PLF_MEM_NAME           "DDR"

#define PLF_CLUSTER_CFG_BASE   (0xFFFFFF00000000ULL)
#define PLF_CLUSTER_CFG_SIZE   (0x80000000ULL)
#define PLF_CLUSTER_CFG_ATTR   (SCR_MPU_CTRL_MT_STRONG | \
                                SCR_MPU_CTRL_MR |        \
                                SCR_MPU_CTRL_MW)
#define PLF_CLUSTER_CFG_NAME   "CLUSTER CFG"

#define PLF_L3CTL_BASE         (PLF_CLUSTER_CFG_BASE + 0x4000)

#define PLF_MMCFG_BASE         EXPAND32ADDR(0x80000000)
#define PLF_MMCFG_SIZE         (8UL*1024UL)
#define PLF_MMCFG_ATTR         (SCR_MPU_CTRL_MT_CFG | \
                                SCR_MPU_CTRL_MR |     \
                                SCR_MPU_CTRL_MW)
#define PLF_MMCFG_NAME         "MMCFG"

#define PLF_L2CTL_BASE         (PLF_MMCFG_BASE)

#define PLF_MTIMER_BASE        EXPAND32ADDR(0xE0000000)
#define PLF_MTIMER_SIZE        (8UL*1024UL)
#define PLF_MTIMER_ATTR        (SCR_MPU_CTRL_MT_STRONG | \
                                SCR_MPU_CTRL_MR |        \
                                SCR_MPU_CTRL_MW)
#define PLF_MTIMER_NAME        "MTIMER"
#define PLF_MTIMER_EXT

#define PLF_PLIC_BASE          EXPAND32ADDR(0xFC000000)
#define PLF_PLIC_SIZE          (32UL*1024UL*1024UL)
#define PLF_PLIC_MSI_OFFSET    (16UL*1024UL*1024UL)
#define PLF_PLIC_ATTR          (SCR_MPU_CTRL_MT_STRONG | \
                                SCR_MPU_CTRL_MR |        \
                                SCR_MPU_CTRL_MW |        \
                                SCR_MPU_CTRL_SR |        \
                                SCR_MPU_CTRL_SW)
#define PLF_PLIC_NAME          "PLIC"

#define PLF_MMIO_BASE          EXPAND32ADDR(0xFF000000)
#define PLF_MMIO_SIZE          (8UL*1024UL*1024UL)
#define PLF_MMIO_ATTR          (SCR_MPU_CTRL_MT_STRONG | \
                                SCR_MPU_CTRL_MR |        \
                                SCR_MPU_CTRL_MW |        \
                                SCR_MPU_CTRL_SR |        \
                                SCR_MPU_CTRL_SW)
#define PLF_MMIO_NAME          "MMIO"

#define PLF_OCRAM_BASE         EXPAND32ADDR(0xFFFF0000)
#define PLF_OCRAM_SIZE         (64UL*1024UL)
#define PLF_OCRAM_ATTR         (SCR_MPU_CTRL_MT_WEAKLY | \
                                SCR_MPU_CTRL_MA |        \
                                SCR_MPU_CTRL_SR)
#define PLF_OCRAM_NAME         "On-Chip RAM"

#if PLF_ICCM_L3_SUPPORT
// Support for new ICCM
#define PLF_ICCM_BASE          (0xFFFFFF10000000ULL)
#define PLF_ICCM_SIZE          (8UL*1024UL)
#define PLF_ICCM_ATTR          (SCR_MPU_CTRL_MT_STRONG | \
                                SCR_MPU_CTRL_MR |        \
                                SCR_MPU_CTRL_MW |        \
                                SCR_MPU_CTRL_SR |        \
                                SCR_MPU_CTRL_SW)
#define PLF_ICCM_NAME          "ICCM"
#define PLF_ICCM_REGION        {PLF_ICCM_BASE, PLF_ICCM_SIZE, PLF_ICCM_ATTR, PLF_ICCM_NAME, CLUSTER_CSR_MPORT}
#else
#define PLF_ICCM_REGION
#endif

#define PLF_MEM_MAP                                                                           \
    {PLF_MEM_BASE, PLF_MEM_SIZE, PLF_MEM_ATTR, PLF_MEM_NAME, PLF_DEFAULT_MPORT},              \
    {PLF_CLUSTER_CFG_BASE, PLF_CLUSTER_CFG_SIZE, PLF_CLUSTER_CFG_ATTR, PLF_CLUSTER_CFG_NAME, CLUSTER_CSR_MPORT}, \
    PLF_ICCM_REGION,                                                                          \
    {PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR, PLF_MMCFG_NAME, PLF_DEFAULT_MPORT},      \
    {PLF_MTIMER_BASE, PLF_MTIMER_SIZE, PLF_MTIMER_ATTR, PLF_MTIMER_NAME, CLUSTER_CSR_MPORT},  \
    {PLF_PLIC_BASE, PLF_PLIC_SIZE, PLF_PLIC_ATTR, PLF_PLIC_NAME, CLUSTER_CSR_MPORT},          \
    {PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR, PLF_MMIO_NAME, PLF_DEFAULT_MPORT},          \
    {PLF_OCRAM_BASE, PLF_OCRAM_SIZE, PLF_OCRAM_ATTR, PLF_OCRAM_NAME, PLF_DEFAULT_MPORT}

//----------------------
// cache configuration
//----------------------

// min cache line
#define PLF_CACHELINE_SIZE (16)

// global configuration: cachable
#define PLF_CACHE_CFG CACHE_GLBL_ENABLE

//----------------------
// PLIC configuration
//----------------------

// interrupt pending banks offset
#define PLF_PLIC_OFFSET_SRC_IP  (0x2000)

// interrupt enable banks offset
#define PLF_PLIC_OFFSET_TGT_IE  (0x4000)

#define PLF_PLIC_LINE_NUM       (2047)

//----------------------
// MMIO configuration
//----------------------

// FPGA build ID
#define PLF_BLD_ID_ADDR (PLF_MMIO_BASE)
// FPGA sysclk, MHz
#define PLF_SYSCLK_MHZ_ADDR (PLF_MMIO_BASE + 0x1000)
#define VarClkMHz (*(const uint32_t*)(PLF_SYSCLK_MHZ_ADDR) * 1000000)
#ifndef PLF_SYS_CLK
#define PLF_SYS_CLK VarClkMHz
#endif
// FPGA UART port
#define PLF_UART0_BASE  (PLF_MMIO_BASE + 0x10000)
#define PLF_UART0_16550

// external interrupt lines
#define PLF_INTLINE_UART0   (1)

#endif // PLATFORM_SCR7_L3_MPU_LEGACY_H
