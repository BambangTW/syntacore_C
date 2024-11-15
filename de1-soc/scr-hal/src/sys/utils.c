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
/// @brief platform init funcs
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


#include <stdio.h>
#include "arch.h"
#include "drivers/cache.h"
#include "drivers/rtc.h"

#if PLF_MPU_SUPPORT
#include "drivers/mpu.h"
#elif PLF_PMP_SUPPORT
#include "drivers/pmp.h"
#endif /* PLF_MPU_SUPPORT */
#if PLF_MRT_SUPPORT
#include "drivers/mrt.h"
#endif /* PLF_MRT_SUPPORT */

#include "utils.h"

#if PLF_SMP_SUPPORT
extern volatile void* hart_start_table[PLF_SMP_HART_NUM];
#endif

void* memset_optnone(void *dest, int ch, size_t count)
{
    return memset(dest, ch, count);
}

int hal_get_sysinfo(char* buf, size_t len, bool extended_output)
{
    /* Common sysinfo block */
    int sz = 0;

#ifdef PLF_SYS_CLK
    sz += snprintf(buf, len, "Platform:      \t%s %s @ %u.%03u MHz\n", PLF_CPU_NAME, PLF_IMPL_STR,
           (unsigned int)(PLF_CPU_CLK / 1000000),
           (unsigned int)((PLF_CPU_CLK / 1000) % 1000));
#else /* PLF_SYS_CLK */
    sz += snprintf(buf, len, "Platform:      \t%s %s\n", PLF_CPU_NAME, PLF_IMPL_STR);
#endif /* PLF_SYS_CLK */
    if (get_system_id()) {
        sz += snprintf((buf + sz), (len - sz), "SystemID:      \t%lx\n", (unsigned long)get_system_id());
    }
    if (get_build_id()) {
        sz += snprintf((buf + sz), (len - sz), "Build ID:      \t%lx\n", (unsigned long)get_build_id());
    }

    const char* CPU_MXL[4] = {"??", "32", "64", "128"};
    const char* EXTENSIONS = "iemafdqlncbtpvx";

    sz += snprintf((buf + sz), (len - sz), "Hart ID:       \t%lu\n", arch_hartid());

    /* instruction set */
    long cpu_id = (long)arch_misa();
    unsigned mxl = 0;

    if (cpu_id < 0) {
        mxl |= 0x2;
    }
    if ((cpu_id << 1) < 0) {
        mxl |= 0x1;
    }

    sz += snprintf((buf + sz), (len - sz), "ISA:           \trv%s", CPU_MXL[mxl]);

    /* decode cpu extensions */
    for (const char* ep = EXTENSIONS; *ep; ++ep) {
        if (cpu_id & (1L << (*ep - 'a'))) {
            sz += snprintf((buf + sz), (len - sz), "%c", *ep);
        }
    }

    sz += snprintf((buf + sz), (len - sz), " [%lx]\n", cpu_id);

    /* vendor id */
    sz += snprintf((buf + sz), (len - sz), "Vendor:        \t%lx\n", arch_vendorid());

    /* architecture */
    sz += snprintf((buf + sz), (len - sz), "Architecture:  \t%lx\n", arch_archid());

    /* architecture scr core configuration */
    switch (hal_arch_coreconfig()) {
    case HAL_CORECONFIG_SCR9_LITE:
        sz += snprintf((buf + sz), (len - sz), "Core config:   \tSCR9 Lite\n");
        break;
    case HAL_CORECONFIG_SCR9_HEAVY:
        sz += snprintf((buf + sz), (len - sz), "Core config:   \tSCR9 Heavy\n");
        break;
    default:
        break;
    }

    /* core implementation */
    sz += snprintf((buf + sz), (len - sz), "Implementation:\t%lx\n", arch_impid());

    /* clock rate */
    const unsigned long cr = rtc_dyn_hart_clock_rate();
    sz += snprintf((buf + sz), (len - sz), "Clock rate:    \t%lu.%03lu MHz\n", cr / 1000000, (cr % 1000000) / 1000);

#ifdef PLF_CACHE_CFG
    /* L1 cache info block */

    char l1_cacheinfo_buf[128];
    memset(l1_cacheinfo_buf, '\0', sizeof(l1_cacheinfo_buf));

    if (cache_l1i_info(l1_cacheinfo_buf, sizeof(l1_cacheinfo_buf))) {
        sz += snprintf((buf + sz), (len - sz), "$L1 instr:     \t%s\n", l1_cacheinfo_buf);
    }

    if (cache_l1d_info(l1_cacheinfo_buf, sizeof(l1_cacheinfo_buf))) {
        sz += snprintf((buf + sz), (len - sz), "$L1 data:      \t%s\n", l1_cacheinfo_buf);
    }

    if (plf_l1cache_prefetcher_is_available()) {
        if (plf_l1cache_prefetcher_info(l1_cacheinfo_buf, sizeof(l1_cacheinfo_buf))) {
            sz += snprintf((buf + sz), (len - sz), "$L1 prefetcher:\t%s\n", l1_cacheinfo_buf);
        }
    }
#endif /* PLF_CACHE_CFG */

#ifdef PLF_L2CTL_BASE
    /* L2 cache info block */
    char l2_cacheinfo_buf[128];
    memset(l2_cacheinfo_buf, '\0', sizeof(l2_cacheinfo_buf));

    if (cache_l2_info(l2_cacheinfo_buf, sizeof(l2_cacheinfo_buf))) {
        sz += snprintf((buf + sz), (len - sz), "$L2 unified:   \t%s\n", l2_cacheinfo_buf);
    }
#endif /* PLF_L2CTL_BASE */

#ifdef PLF_L3CTL_BASE
    /* L3 cache info block */
    char l3_cacheinfo_buf[128];
    memset(l3_cacheinfo_buf, '\0', sizeof(l3_cacheinfo_buf));

    if (cache_l3_info(l3_cacheinfo_buf, sizeof(l3_cacheinfo_buf))) {
        sz += snprintf((buf + sz), (len - sz), "$L3 cluster:   \t%s\n", l3_cacheinfo_buf);
    }
#endif /* PLF_L3CTL_BASE */

#if PLF_SMP_SUPPORT
    /* SMP block info */
    sz += snprintf((buf + sz), (len - sz), "Max harts:     \t{");
    for (unsigned int i = 0; i < PLF_SMP_HART_NUM; ++i) {
        sz += snprintf((buf + sz), (len - sz), "[%i]", i);
    }
    sz += snprintf((buf + sz), (len - sz), "}\n");

    sz += snprintf((buf + sz), (len - sz), "Active harts:  \t{");
    for (unsigned int i = 0; i < PLF_SMP_HART_NUM; ++i) {
        /* secondary hart, "-1" - presense and run, "1" - presense and finish job */
        if ((hart_start_table[i] == (void*)-1) || (hart_start_table[i] == (void*)1)) {
            sz += snprintf((buf + sz), (len - sz), "[+]");
        }
        else {
            sz += snprintf((buf + sz), (len - sz), "[-]");
        }
    }
    sz += snprintf((buf + sz), (len - sz), "}\n");
#endif /* PLF_SMP_SUPPORT */

    /* add more data */
    if (extended_output) {
#ifdef PLF_MEM_MAP
#if (PLF_MPU_SUPPORT || PLF_PMP_SUPPORT)
        /* Memory map info block */
        sz += snprintf((buf + sz), (len - sz), "\nMemory map:\n");

        for (unsigned int i = 0; i < mem_region_count; ++i) {
            const uintptr_t mreg_sz = mem_regions[i].size;
#if __riscv_xlen == 32
            sz += snprintf((buf + sz), (len - sz), "%08x-%08x  ",
#else  /* __riscv_xlen == 32 */
            sz += snprintf((buf + sz), (len - sz), "%016lX-%016lX  ",
#endif /* __riscv_xlen == 32 */
            mem_regions[i].base, mem_regions[i].base + (mreg_sz - 1));

            if (mreg_sz >= (1 << 30) && ((mreg_sz & ((1 << 30) - 1)) == 0)) {
                sz += snprintf((buf + sz), (len - sz), "%luG", (unsigned long)mreg_sz >> 30);
            }
            else if (mreg_sz >= (1 << 20) && ((mreg_sz & ((1 << 20) - 1)) == 0)) {
                sz += snprintf((buf + sz), (len - sz), "%luM", (unsigned long)mreg_sz >> 20);
            }
            else if (mreg_sz >= (1 << 10) && ((mreg_sz & ((1 << 10) - 1)) == 0)) {
                sz += snprintf((buf + sz), (len - sz), "%luK", (unsigned long)mreg_sz>> 10);
            }
            else {
                sz += snprintf((buf + sz), (len - sz), "%lu", (unsigned long)mreg_sz);
            }

#if PLF_MPU_SUPPORT
        static const char* reg_mode[4] = {" cwo", "ncso", "ncwo", "mcfg"};
        unsigned attr                  = (unsigned int)mem_regions[i].attr;
        char attr_str[10];
        attr_str[0] = (attr & SCR_MPU_CTRL_SX) ? 'x' : '-';
        attr_str[1] = (attr & SCR_MPU_CTRL_SW) ? 'w' : '-';
        attr_str[2] = (attr & SCR_MPU_CTRL_SR) ? 'r' : '-';
        attr_str[3] = (attr & SCR_MPU_CTRL_UX) ? 'x' : '-';
        attr_str[4] = (attr & SCR_MPU_CTRL_UW) ? 'w' : '-';
        attr_str[5] = (attr & SCR_MPU_CTRL_UR) ? 'r' : '-';
        attr_str[6] = (attr & SCR_MPU_CTRL_MX) ? 'x' : '-';
        attr_str[7] = (attr & SCR_MPU_CTRL_MW) ? 'w' : '-';
        attr_str[8] = (attr & SCR_MPU_CTRL_MR) ? 'r' : '-';
        attr_str[9] = '\0';

        sz += snprintf((buf + sz), (len - sz), "\t%s %s %s  %s\n",
                (attr & SCR_MPU_CTRL_LOCK) ? "L" : " ",
                reg_mode[(attr >> 16) & 0x3],
                attr_str,
                mem_regions[i].name);

#elif PLF_PMP_SUPPORT
        static const char* reg_mode[4] = {"   weak", " strong", "weak nc", "   mcfg"};
        unsigned int attr              = (unsigned int)mem_regions[i].attr;
        char attr_str[4];
        attr_str[0] = (attr & PMP_X) ? 'x' : '-';
        attr_str[1] = (attr & PMP_W) ? 'w' : '-';
        attr_str[2] = (attr & PMP_R) ? 'r' : '-';
        attr_str[3] = '\0';

        sz += snprintf((buf + sz), (len - sz), "\t%s %s %s %s  %s\n",
                (attr & PMP_L) ? "L" : " ",
                (attr & PMP_A) ? "A" : " ",
                reg_mode[((attr & SCR_PMP_CTRL_MT_MASK) >> SCR_PMP_CTRL_MATTR_OFF)],
                attr_str,
                mem_regions[i].name);
#else /* PLF_MPU_SUPPORT */
        sz += snprintf((buf + sz), (len - sz), "\t  %s\n", mem_regions[i].name);
#endif /* PLF_MPU_SUPPORT */
        }

#if PLF_MRT_SUPPORT
        const unsigned int entries_max = (*(volatile size_t*)MRT_VID >> 8) & 0xFF;

        sz += snprintf((buf + sz), (len - sz), "\nMRT entries (%u):\n", entries_max);
        sz += snprintf((buf + sz), (len - sz), "   addr             mask             ctrl\n");
        sz += snprintf((buf + sz), (len - sz), "---------------------------------------------\n");

        for (unsigned int entry = 0; entry < entries_max; ++entry) {
            sz += snprintf((buf + sz), (len - sz), "%02u %016lx %016lx %08lx\n",
                    entry,
                    *(volatile size_t*)MRT_S_ENTRY_N_ADDR(entry),
                    *(volatile size_t*)MRT_S_ENTRY_N_MASK(entry),
                    *(volatile size_t*)MRT_S_ENTRY_N_CTRL(entry));
        }
#endif /* PLF_MRT_SUPPORT */
#endif /* (PLF_MPU_SUPPORT || PLF_PMP_SUPPORT) */
#endif /* PLF_MEM_MAP */
    }

    return sz;
}
