/*
 * Copyright (C) 2024, Syntacore Ltd.
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
/// @brief Synopsys DesignWire 16550 registers
/// Syntacore SCR* infra

#ifndef SCR_HAL_DW_UART_REGS_H
#define SCR_HAL_DW_UART_REGS_H

#include <stdint.h>

#define DW_UART_APB_DATA_WIDTH          32
#define DW_UART_MAX_APB_DATA_WIDTH      32
#define DW_UART_FIFO_MODE               32      // UART FIFO depth
#define DW_UART_MEM_SELECT              1
#define DW_UART_MEM_MODE                0
#define DW_UART_CLOCK_MODE              1
#define DW_UART_AFCE_MODE               0x0
#define DW_UART_THRE_MODE               0x1
#define DW_UART_SIR_MODE                0x0
#define DW_UART_CLK_GATE_EN             1
#define DW_UART_FIFO_ACCESS             0x0
#define DW_UART_DMA_EXTRA               0x0
#define DW_UART_DMA_POL                 0
#define DW_UART_SIR_LP_MODE             0x0
#define DW_UART_DEBUG                   0
#define DW_UART_BAUD_CLK                0
#define DW_UART_ADDITIONAL_FEATURES     0x1
#define DW_UART_FIFO_STAT               0x1
#define DW_UART_SHADOW                  0x0
#define DW_UART_ADD_ENCODED_PARAMS      0x0
#define DW_UART_LATCH_MODE              0
#define DW_UART_ADDR_SLICE_LHS          8
#define DW_UART_COMP_VERSION            0x3430332a
#define DW_UART_DLF_SIZE                6       // Fractional Divisor Width
#define DW_UART_SHADOW_BUF_SIZE         16

/* Synopsys extension to 16550 on their DesignWare IP block */
typedef struct {
    union {                                     ///< 0x00
        uint32_t data;                          ///< Receive Buffer Register
        uint32_t thr;                           ///< Transmit Holding Register
        uint32_t divlo;                         ///< Divisor Latch (Low)
    };
    union {                                     ///< 0x04
        uint32_t ier;                           ///< Interrupt Enable Register
        uint32_t divhi;                         ///< Divisor Latch High
    };
    union {                                     ///< 0x08
        const uint32_t iir;                     ///< Interrupt Identification Register
        uint32_t fcr;                           ///< FIFO Control Register
    };
    uint32_t lcr;                               ///< 0x0C Line Control Register
    uint32_t mcr;                               ///< 0x10 Modem Control Register
    uint32_t lsr;                               ///< 0x14 Line Status Register
    uint32_t msr;                               ///< 0x18 Modem Status Register
    uint32_t scr;                               ///< 0x1C Scratchpad Register
    uint32_t lpdll;                             ///< 0x20 Low Power Divisor Latch Low
    uint32_t lpdlh;                             ///< 0x24 Low Power Divisor Latch High
    uint32_t _ph0[2];                           ///< 0x28, 0x2C
    union {                                     ///< 0x30
        const uint32_t srbr[DW_UART_SHADOW_BUF_SIZE];  ///< Shadow Receive Buffer Register
        uint32_t sthr[DW_UART_SHADOW_BUF_SIZE];        ///< Shadow Transmit Holding Register
    };
    uint32_t far;                               ///< 0x70 FIFO Access Register
    uint32_t tfr;                               ///< 0x74 Transmit FIFO Read
    uint32_t rfw;                               ///< 0x78 Receive FIFO Write
    uint32_t usr;                               ///< 0x7C UART Status register
    uint32_t tfl;                               ///< 0x80 Transmit FIFO Level
    uint32_t rfl;                               ///< 0x84 Receive FIFO Level
    uint32_t srr;                               ///< 0x88 Software Reset Register
    uint32_t srts;                              ///< 0x8C Shadow Request to Send
    uint32_t sbcr;                              ///< 0x90 Shadow Break Control Register
    uint32_t sdmam;                             ///< 0x94 Shadow DMA Mode Register
    uint32_t sfe;                               ///< 0x98 Shadow FIFO Enable Register
    uint32_t srt;                               ///< 0x9C Shadow RCVR Trigger Register
    uint32_t stet;                              ///< 0xA0 Shadow TX Empty Trigger Register
    uint32_t htx;                               ///< 0xA4 Halt TX
    uint32_t dmasa;                             ///< 0xA8 DMA Software Acknowledge Register
    uint32_t tcr;                               ///< 0xAC Transceiver Control Register
    uint32_t de_en;                             ///< 0xB0 Driver Output Enable Register
    uint32_t re_en;                             ///< 0xB4 Receiver Output Enable Register
    uint32_t det;                               ///< 0xB8 Driver Output Enable Timing Register
    uint32_t tat;                               ///< 0xBC TurnAround Timing Register
    uint32_t dlf;                               ///< 0xC0 Divisor Latch Fraction Register
    uint32_t rar;                               ///< 0xC4 Receive Address Register
    uint32_t tar;                               ///< 0xC8 Transmit Address Register
    uint32_t lcr_ext;                           ///< 0xCC Line Extended Control Register
    uint32_t uart_prot_level;                   ///< 0xD0 UART Protection level
    uint32_t reg_timeout_rst;                   ///< 0xD4 Register timeout counter reset value
    uint32_t _ph1[7];                           ///< 0xD8, 0xDC, 0xE0, 0xE4, 0xE8, 0xEC, 0xF0
    uint32_t cpr;                               ///< 0xF4 Component Parameter Register
    uint32_t ucv;                               ///< 0xF8 UART Component Version
    uint32_t ctr;                               ///< 0xFC Component Type Register
    uint32_t _ph2;                              ///< 0x100
    uint32_t new_modes;                         ///< 0x104
} volatile dw_uart_regs_t;

#define DW_UART_DIV(CLK, BAUDRATE)              (CLK / (16 * BAUDRATE))
#define DW_UART_DIV_FRACT(CLK, BAUDRATE)        ((((CLK) * (1ull << (DW_UART_DLF_SIZE))) / \
                                                (16 * BAUDRATE )) % (1ull << (DW_UART_DLF_SIZE)))

#endif /* SCR_HAL_DW_UART_REGS_H */
