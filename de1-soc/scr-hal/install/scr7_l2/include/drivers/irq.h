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
/// @brief Syntacore external interrupt primitives
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

#ifndef SCR_BSP_IRQ_H
#define SCR_BSP_IRQ_H

#include "arch.h"

#if PLF_APLIC_BASE_M
#include "drivers/aia.h"
#define IRQ_TYPE_LEVEL_HIGH         APLIC_SOURCECFG_SM_LEVEL1
#define IRQ_TYPE_LEVEL_LOW          APLIC_SOURCECFG_SM_LEVEL0
#define IRQ_TYPE_EDGE_RISING        APLIC_SOURCECFG_SM_EDGE1
#define IRQ_TYPE_EDGE_FALLING       APLIC_SOURCECFG_SM_EDGE0

//----------------------
// generic IRQ routines
//----------------------

#ifdef __cplusplus
extern "C" {
#endif

static inline void irq_init(void)
{
    aia_init();
}

static inline int irq_setup(int line, int mode)
{
    return aia_irq_setup(line, mode);
}

static inline void irq_enable(int irq)
{
    aia_irq_enable(irq);
}

static inline void irq_disable(int irq)
{
    aia_irq_disable(irq);
}

static inline int irq_soi(void)
{
    return aia_soi();
}

static inline void irq_eoi(int line)
{
    aia_eoi(line);
}

#ifdef __cplusplus
}
#endif

#elif PLF_PLIC_BASE
#include "drivers/plic.h"
#define IRQ_TYPE_LEVEL_HIGH         PLIC_SRC_MODE_LEVEL_HIGH
#define IRQ_TYPE_LEVEL_LOW          PLIC_SRC_MODE_LEVEL_LOW
#define IRQ_TYPE_EDGE_RISING        PLIC_SRC_MODE_EDGE_RISING
#define IRQ_TYPE_EDGE_FALLING       PLIC_SRC_MODE_EDGE_FALLING

#define IRQ_NONE (0)

//----------------------
// generic IRQ routines
//----------------------

#ifdef __cplusplus
extern "C" {
#endif

static inline void irq_init(void)
{
    plic_init();
}

static inline int irq_setup(int line, int mode)
{
    return plic_irq_setup(line, mode);
}

static inline void irq_enable(int irq)
{
    plic_irq_enable(irq);
}

static inline void irq_disable(int irq)
{
    plic_irq_disable(irq);
}

static inline int irq_soi(void)
{
    return plic_soi();
}

static inline void irq_eoi(int line)
{
    plic_eoi(line);
}

#ifdef __cplusplus
}
#endif
#else  // PLF_PLIC_BASE
#include "drivers/ipic.h"

#define IRQ_TYPE_LEVEL_HIGH   (IPIC_IRQ_LEVEL)
#define IRQ_TYPE_LEVEL_LOW    (IPIC_IRQ_LEVEL | IPIC_IRQ_INV)
#define IRQ_TYPE_EDGE_RISING  (IPIC_IRQ_EDGE)
#define IRQ_TYPE_EDGE_FALLING (IPIC_IRQ_EDGE | IPIC_IRQ_INV)

#define IRQ_NONE (IPIC_IRQ_VEC_VOID)

//----------------------
// generic IRQ routines
//----------------------
#ifdef __cplusplus
extern "C" {
#endif

static inline void irq_init(void)
{
    ipic_init();
}

static inline int irq_setup(int line, int mode)
{
    return ipic_irq_setup(-1, line, mode, IPIC_IRQ_PRIV_MMODE);
}

static inline void irq_enable(int irq)
{
    ipic_irq_enable(irq);
}

static inline void irq_disable(int irq)
{
    ipic_irq_disable(irq);
}

static inline int irq_soi(void)
{
    return ipic_soi();
}

static inline void irq_eoi(int line)
{
    (void)line;
    ipic_eoi();
}

#ifdef __cplusplus
}
#endif
#endif // PLF_PLIC_BASE
#endif // SCR_BSP_IRQ_H

