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
/// @brief AIA init function
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

#include "drivers/aia.h"

#if PLF_APLIC_BASE_M

static void aia_aplic_enable_domain(size_t domain)
{
    uint32_t domaincfg = aplic_get_domaincfg(APLIC_M);

    // enable interrupt delivery from the domain
    domaincfg |= APLIC_DOMAINCFG_IE_BIT;
    aplic_set_domaincfg(APLIC_M, domaincfg);
}

static void aia_aplic_clear(size_t domain)
{
    // clear pending interrupts (if possible)
    for (int reg = APLIC_MIN_IRQ_REG; reg <= APLIC_MAX_IMPL_IRQ_REG; ++reg) {
        aplic_set_clrip(domain, reg, APLIC_REG_ALL_BITS_MASK);
        aplic_set_clrie(domain, reg, APLIC_REG_ALL_BITS_MASK);
    }
}

static void aia_aplic_init_msi(void)
{
    struct aplic_msicfg_data msicfg = {0};

    msicfg.base_addr = IMSIC_M_BASE_ADDR;
    msicfg.lhxw = APLIC_LHXW_M;
    msicfg.lhxs = APLIC_LHXS_M;
    msicfg.hhxw = APLIC_HHXW_M;
    msicfg.hhxs = APLIC_HHXS_M;

    aplic_write_msiconfig(&msicfg,
                          APLIC_M_BASE_ADDR + APLIC_MMSIADDRCFG,
                          APLIC_M_BASE_ADDR + APLIC_MMSIADDRCFGH);
}

static void aia_aplic_init(void)
{
    aia_aplic_init_msi();

    aia_aplic_clear(APLIC_M);
    aia_aplic_enable_domain(APLIC_M);
}


static void aia_imsic_clear_all(size_t priv_level)
{
    for (int i = 0; i <= IMSIC_REG_MAX; i += IMSIC_REG_STEP) {
        imsic_write(priv_level, IMSIC_EIP_BASE + i, 0);
    }
}

void aia_init(void)
{

    for (int source = APLIC_SOURCE_MIN; source <= APLIC_SOURCE_MAX; ++source) {
        // configure all sources as detached
        aplic_set_sourcecfg(APLIC_M, source, APLIC_SOURCECFG_SM_DETACH);
    }

    aia_imsic_clear_all(IMSIC_M);

    imsic_write(IMSIC_M, IMSIC_EIDELIVERY,  1);

    aplic_set_domaincfg(APLIC_M, APLIC_DOMAINCFG_IE_BIT | APLIC_DOMAINCFG_DM_BIT);

    aia_aplic_init();

    write_csr(mideleg, SIE_SEIE);
    set_csr(mstatus, GLOBAL_INTERRUPT_EN | SUPERVISOR_INTERRUPT_EN);
    set_csr(mie, MIE_MEIE);
}

int aia_irq_setup(int line, int mode)
{
    aplic_set_sourcecfg(APLIC_M, line, mode);

    aplic_set_target(APLIC_M, line, aplic_make_target(arch_hartid(), 0, line));

    aplic_set_setienum(APLIC_M, line);

    return line;
}

void aia_irq_enable(int irq)
{
    imsic_set_eienum(IMSIC_M, irq);
}

void aia_irq_disable(int irq)
{
    imsic_set_eienum(IMSIC_M, irq);
}

int aia_soi(void)
{
    uint64_t mtopei = swap_csr(IMSIC_CSR_MTOPEI, 0);

    uint32_t interrupt_id = imsic_topei_identity(mtopei);

    return interrupt_id;
}

void aia_eoi(int line)
{
    uint64_t mtopei = line << IMSIC_XTOPEI_ID_SHIFT;

    swap_csr(IMSIC_CSR_MTOPEI, mtopei);
}

#endif // PLF_APLIC_BASE_M

