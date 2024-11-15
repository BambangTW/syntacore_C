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
/// @brief Synopsys extension to 16550 on their DesignWare IP block defs and inline funcs
/// Syntacore SCR* infra

#ifndef SCR_HAL_UART_DW_H
#define SCR_HAL_UART_DW_H

#include "arch.h"

#if defined(PLF_UART0_BASE) || defined(UART0_16550_DW)

#ifndef PLF_UART_CLK
#define PLF_UART_CLK PLF_SYS_CLK
#endif

#ifndef PLF_UART_BAUDRATE
#define PLF_UART_BAUDRATE 115200
#endif // PLF_UART_BAUDRATE

#define PLF_UART0_MMIO 32

#include "drivers/16550_regs.h"

#include "drivers/dw_uart_regs.h"

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UART0_16550_DW
#define UART0_16550_DW (*(dw_uart_regs_t*)PLF_UART0_BASE)
#endif

static inline int uart0_16550_dw_rx_ready(void)
{
    return UART0_16550_DW.lsr & UART_16550_LSR_DR;
}

static inline int uart0_16550_dw_tx_ready(void)
{
    return UART0_16550_DW.lsr & UART_16550_LSR_THRE;
}

static inline void uart0_16550_dw_tx_flush(void)
{
    while ((UART0_16550_DW.lsr & UART_16550_LSR_TEMT) == 0);
}

static inline void uart0_16550_dw_rx_flush(void)
{
    while (UART0_16550_DW.lsr & UART_16550_LSR_DR)
        (void)UART0_16550_DW.data;
}

static inline int uart0_16550_dw_put(int data)
{
    while (!uart0_16550_dw_tx_ready());

    return (int)(UART0_16550_DW.data = (uint32_t)data);
}

static inline int uart0_16550_dw_getch_nowait(void)
{
    if (uart0_16550_dw_rx_ready())
        return (int)UART0_16550_DW.data;

    return -1; // no input
}

static inline void uart0_16550_dw_init(void)
{
    const unsigned int div = DW_UART_DIV(PLF_UART_CLK, PLF_UART_BAUDRATE);
    const unsigned int div_fract = DW_UART_DIV_FRACT(PLF_UART_CLK, PLF_UART_BAUDRATE);

    // disable interrupts
    UART0_16550_DW.ier = 0;
    // init MCR
    UART0_16550_DW.mcr = 0;

    // setup LCR, baud rate
    UART0_16550_DW.lcr = UART_16550_LCR_INIT | UART_16550_LCR_DIVL;
    UART0_16550_DW.divlo = div & 0xff;
    UART0_16550_DW.divhi = (div >> 8) & 0xff;

    // setup fractional divisor
    if (!hal_qemu_sign()) // qemu does not support dw uart and dlf yet
        UART0_16550_DW.dlf = div_fract;

    // disable LCR access
    UART0_16550_DW.lcr = UART_16550_LCR_INIT;

    // init FIFO
    UART0_16550_DW.fcr = UART_16550_FCR_R_RST | UART_16550_FCR_T_RST | UART_16550_FCR_EN;
    UART0_16550_DW.fcr = UART_16550_FCR_INIT;
}

#ifdef PLF_UART0_16550_DW
static inline void plf_con_init(void)
{
#ifndef PLF_UART0_NOINIT
    uart0_16550_dw_init();
#endif
}

static inline void plf_con_put(int c)
{
    uart0_16550_dw_put(c);
}

static inline int plf_con_getch_nowait(void)
{
    return uart0_16550_dw_getch_nowait();
}

static inline void plf_con_tx_flush(void)
{
    uart0_16550_dw_tx_flush();
}
#endif // PLF_UART0_16550_DW

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !__ASSEMBLER__
#endif // PLF_UART0_BASE || UART0_16550_DW
#endif // SCR_HAL_UART_16550_H
