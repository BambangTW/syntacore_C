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

#ifndef PLATFORM_SCR3_RV32_L1_NEXYS_H
#define PLATFORM_SCR3_RV32_L1_NEXYS_H

#if __riscv_xlen != 32
#error 32bit platform, check target settings
#endif

#include <plf_scr3.h>

#define PLF_LEDS_SUPPORT (1)

#define PLF_INIT_REGS    (1)

//----------------------
// memory configuration
//----------------------
#define PLF_MEM_BASE     (0)
#define PLF_MEM_SIZE     (128*1024*1024)
#define PLF_MEM_ATTR     (SCR_MPU_CTRL_MT_WEAKLY | SCR_MPU_CTRL_ALL)
#define PLF_MEM_NAME     "DDR"

#define PLF_MMCFG_BASE   EXPAND32ADDR(0xf0040000)
#define PLF_MMCFG_SIZE   (0x1000)
#define PLF_MMCFG_ATTR   (SCR_MPU_CTRL_MT_CFG | \
                          SCR_MPU_CTRL_MR |     \
                          SCR_MPU_CTRL_MW)
#define PLF_MMCFG_NAME   "MMCFG"

#define PLF_MTIMER_BASE  PLF_MMCFG_BASE

#define PLF_MMIO_BASE    EXPAND32ADDR(0xff000000)
#define PLF_MMIO_SIZE    (0x100000)
#define PLF_MMIO_ATTR    (SCR_MPU_CTRL_MT_STRONG | \
                          SCR_MPU_CTRL_MR |        \
                          SCR_MPU_CTRL_MW |        \
                          SCR_MPU_CTRL_SR |        \
                          SCR_MPU_CTRL_SW)
#define PLF_MMIO_NAME    "MMIO"

#define PLF_OCRAM_BASE   EXPAND32ADDR(0xffff0000)
#define PLF_OCRAM_SIZE   (64*1024)
#define PLF_OCRAM_ATTR   (SCR_MPU_CTRL_MT_WEAKLY | SCR_MPU_CTRL_MA | SCR_MPU_CTRL_SR)
#define PLF_OCRAM_NAME   "On-Chip RAM"

#define PLF_MEM_MAP                                                   \
    {PLF_MEM_BASE, PLF_MEM_SIZE, PLF_MEM_ATTR, PLF_MEM_NAME},         \
    {PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR, PLF_MMCFG_NAME}, \
    {PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR, PLF_MMIO_NAME},     \
    {PLF_OCRAM_BASE, PLF_OCRAM_SIZE, PLF_OCRAM_ATTR, PLF_OCRAM_NAME}

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
// FPGA system ID
#define PLF_SYS_ID_ADDR  (PLF_MMIO_BASE + 0)
// FPGA build ID
#define PLF_BLD_ID_ADDR  (PLF_MMIO_BASE + 0x1000)
// FPGA sysclk, MHz
#define PLF_SYSCLK_MHZ_ADDR (PLF_MMIO_BASE + 0x2000)
#define VarClkMHz (*(const uint32_t*)(PLF_SYSCLK_MHZ_ADDR) * 1000000)
#ifndef PLF_SYS_CLK
#define PLF_SYS_CLK VarClkMHz
#endif

#define PLF_RGBLED_ADDR  (PLF_MMIO_BASE + 0x20000)
#define PLF_RGBLED_PORT_WIDTH 4
#define PLF_PINLED_ADDR  (PLF_MMIO_BASE + 0x20008)
#define PLF_PINLED_PORT_WIDTH 4
#define PLF_HEXLED_ADDR  (PLF_MMIO_BASE + 0x22000)
#define PLF_HEXLED_PORT_WIDTH 4
#define PLF_PINLED_INV   0
#define PLF_RGBLED_INV   0
#define PLF_PINLED_NUM   14
#define PLF_RGBLED_NUM   6

#define PLF_HEXLED_ADDR_MAP                     \
    {(PLF_HEXLED_ADDR + 0x8), 24},              \
    {(PLF_HEXLED_ADDR + 0x8), 16},              \
    {(PLF_HEXLED_ADDR + 0x8), 8},               \
    {(PLF_HEXLED_ADDR + 0x8), 0},               \
    {(PLF_HEXLED_ADDR + 0x0), 24},              \
    {(PLF_HEXLED_ADDR + 0x0), 16},              \
    {(PLF_HEXLED_ADDR + 0x0), 8},               \
    {(PLF_HEXLED_ADDR + 0x0), 0}

#define PLF_HEXLED_INV 0xff

#define HEXLED_SEG_A 0x01
#define HEXLED_SEG_B 0x02
#define HEXLED_SEG_C 0x04
#define HEXLED_SEG_D 0x08
#define HEXLED_SEG_E 0x10
#define HEXLED_SEG_F 0x20
#define HEXLED_SEG_G 0x40
#define HEXLED_SEG_P 0x80

// Xilinx GPIO IP Core
#define PLF_XLX_GPIO0_BASE   (PLF_MMIO_BASE + 0x28000)
#define PLF_XLX_GPIO0_DATA   (PLF_XLX_GPIO0_BASE + 0)
#define PLF_XLX_GPIO0_GIER   (PLF_XLX_GPIO0_BASE + 0x11c)
#define PLF_XLX_GPIO0_ISR    (PLF_XLX_GPIO0_BASE + 0x120)
#define PLF_XLX_GPIO0_IER    (PLF_XLX_GPIO0_BASE + 0x128)

// buttons
#define PLF_BTN_ADDR     (PLF_XLX_GPIO0_DATA)
#define PLF_BTN_NUM      5
// DIP switches
#define PLF_DIP_ADDR     (PLF_MMIO_BASE + 0x29000)
#define PLF_DIP_NUM      16

// external interrupt lines
#define PLF_INTLINE_UART0 0
#define PLF_INTLINE_GPIO0 1

#endif // PLATFORM_SCR3_RV32_L1_NEXYS_H
