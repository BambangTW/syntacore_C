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

#include "drivers/cache.h"
#include "drivers/console.h"
#include "drivers/feature_enable.h"
#include "drivers/leds.h"
#include "drivers/mpu.h"
#include "drivers/pmp.h"
#include "drivers/rtc.h"
#if PLF_MRT_SUPPORT
#include "drivers/mrt.h"
#endif // PLF_MRT_SUPPORT
#include "memasm.h"
#include "perf.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h> /* sbrk() */

extern char _tdata_start[];
extern char _tdata_end[];
extern char _tbss_start[];
extern char _tbss_end[];

#ifdef PLF_MEM_MAP
//----------------------
// For MRT and PMP, the last entry in this table is the most prioritized
// (during PMP configuration entries are written in reverse order to match the
// hardware policy). For MPU, attributes for overlapping chunks are merged
// into the least permissive ones
//----------------------
const scr_mem_region_info mem_regions[] = {PLF_MEM_MAP};
const unsigned mem_region_count         = ARRAY_SIZE(mem_regions);
#endif // PLF_MEM_MAP

uint64_t bss_complete_cycles = 0ULL;

void plf_init_features(void);

// init hart local storage
static void plf_init_tls(void* tls)
{
    // copy tls data
    memcpy(tls, _tdata_start, (size_t)(_tdata_end - _tdata_start));
    // init tls bss
    memset((char*)tls + (_tbss_start - _tdata_start), 0, (size_t)(_tbss_end - _tbss_start));
#if (PLF_SMP_NON_COHERENT || PLF_ICCM_L3_SUPPORT)
    // For L3 cache: after master core inits tls, it can be possibly cached while slave core haven't done cache init
    cache_l1_flush(tls, _tbss_end - _tdata_start);
#endif // PLF_SMP_NON_COHERENT
}

// application early init callback
__attribute__((weak)) void app_early_init(bool boot_hart) { (void)boot_hart; }
__attribute__((weak)) const char *hal_app_exit_message = NULL;

// application exit callback
__attribute__((weak)) void app_exit(void)
{
#ifdef HAL_APP_EXIT_PRINT_MSG
    puts(HAL_APP_EXIT_PRINT_MSG);
#else
    if (hal_app_exit_message)
        puts(hal_app_exit_message);
#endif
}

#if PLF_SMP_SUPPORT
volatile __attribute__((section(".data"))) int plf_smp_sync_var = 0;

void plf_smp_slave_init(void)
{
#if defined(HAL_ENABLE_PERF) && (PLF_CORE_VARIANT_SCR > 1)
    perf_init_hpm();
#endif

    pmp_early_init();

    // application early init callback
    app_early_init(false);

    // enable/init caches
#if PLF_CACHE_L2_DEDICATED
    plf_l2cache_init();
#endif
    plf_l1cache_init();

    mpu_init();
    pmp_init();

    plf_init_features();

    do
    {
#if PLF_SMP_NON_COHERENT
        cache_l1_invalidate((void*)&plf_smp_sync_var, sizeof(plf_smp_sync_var));
#else  // PLF_SMP_NON_COHERENT
        fence();
#endif // PLF_SMP_NON_COHERENT
    } while (!plf_smp_sync_var);
}

// allocate stack and TLS, init TLS
// return init value of SP/TP
void* plf_alloc_thread_generic(void)
{
    unsigned long mem_size = get_hls_mem_size();

    void* hls_base = sbrk((ptrdiff_t)mem_size);

    if (hls_base == (void*)-ENOMEM)
        return hls_base;

    void* base = (char*)hls_base + PLF_MAX_CACHELINE_SIZE + PLF_HLS_MIN_STACK_SIZE;
    // align by 64
    base = (void*)((uintptr_t)base & (uintptr_t)-PLF_MAX_CACHELINE_SIZE);

    return base;
}
void* plf_alloc_thread(void) __attribute__((weak, alias("plf_alloc_thread_generic")));

void smp_slave_entry_default(void)  { /* bypass */ }

void PLF_SMP_SLAVE_ENTRY(void) __attribute__((weak, alias("smp_slave_entry_default")));

// hart_start_table array is used to propagate HLS from master hart to
// secondary harts and also is is used as per harts flags to synchronize
// "start all harts"/"end all harts" checkpoints.
// Array is placed to data section to prevent usage of uninitialized
// data in very early code in crt0.
volatile void* hart_start_table[PLF_SMP_HART_NUM] __attribute__((section(".data")));

#endif // PLF_SMP_SUPPORT

// The following plf_smp_init(), plf_smp_hart_finit() and plf_smp_wait_finit() functions
// operates with hart_start_table array values. Be careful making changes to them.
static void plf_smp_init(void)
{
#if PLF_SMP_SUPPORT
    for (int i = 0; i < PLF_SMP_HART_NUM; ++i)
    {
        if (i != PLF_SMP_MASTER_HARTID - PLF_SMP_HARTID_BASE)
        {
            void* hls = plf_alloc_thread();
            if (hls == (void*)-ENOMEM)
                break;
            plf_init_tls(hls);
            hart_start_table[i] = hls;
        }
        else {
            // a sign of the presence of a hart in the system
            // see plf_smp_hart_finit(), plf_smp_wait_finit()
            hart_start_table[i] = (void *)-1;
        }
    }

    cache_l1_flush(hart_start_table, sizeof(hart_start_table));
#endif // PLF_SMP_SUPPORT
}

void plf_smp_hart_finit(void)
{
#if PLF_SMP_SUPPORT
    hart_start_table[arch_hartid() - PLF_SMP_HARTID_BASE] = (void*)1;
#if PLF_SMP_NON_COHERENT
    cache_l1_flush(hart_start_table, sizeof(hart_start_table));
#endif
#endif  // PLF_SMP_SUPPORT
}

void plf_smp_wait_finit(void)
{
#if PLF_SMP_SUPPORT
    int i;
    do {
#if PLF_SMP_NON_COHERENT
        cache_l1_invalidate(hart_start_table, sizeof(hart_start_table));
#else
        fence();
#endif
        // wait until the harts signal the end of their
        // work through the plf_smp_hart_finit()
        for (i = 0; i < PLF_SMP_HART_NUM; ++i) {
            if (hart_start_table[i] == (void *)-1)
                break;
        }

        cpu_relax();
    } while(i < PLF_SMP_HART_NUM);
#endif // PLF_SMP_SUPPORT
}

extern void plf_init_relocate(void) __attribute__((weak));

extern char __bss_start[], __bss_end[];

void __init plf_init_noreloc(void)
{
    // do nothing
}

void plf_init_features(void)
{
    if (plf_bpu_early_branch_resolution_is_available()) {

        plf_bpu_early_branch_resolution_reset();

#if (BPU_EARLY_BRANCH_RESOLUTION == 1)
        plf_bpu_early_branch_resolution_enable();
#else
        plf_bpu_early_branch_resolution_disable();
#endif // BPU_EARLY_BRANCH_RESOLUTION
    }

    if(plf_bpu_loop_predictor_is_available()) {

        plf_bpu_loop_predictor_reset();

#if (BPU_LOOP_PREDICTOR == 1)
        plf_bpu_loop_predictor_enable();
#else
        plf_bpu_loop_predictor_disable();
#endif // BPU_LOOP_PREDICTOR
    }

    if(plf_l1cache_store_merge_is_available()) {

        plf_l1cache_store_merge_reset();

#if (L1D_STORE_MERGE == 1)
        plf_l1cache_store_merge_enable();
#else
        plf_l1cache_store_merge_disable();
#endif // L1D_STORE_MERGE
    }

    if(plf_l1cache_spec_ld_is_available()) {

        plf_l1cache_spec_ld_reset();

#if (L1D_LOAD_SPEC == 1)
        plf_l1cache_spec_ld_enable();
#else
        plf_l1cache_spec_ld_disable();
#endif // L1D_LOAD_SPEC
    }

    if(plf_l1cache_spec_ld_over_st_is_available()) {

        plf_l1cache_spec_ld_over_st_reset();

#if (L1D_LOAD_OVER_STORE_SPEC == 1)
        plf_l1cache_spec_ld_over_st_enable();
#else
        plf_l1cache_spec_ld_over_st_disable();
#endif // L1D_LOAD_OVER_STORE_SPEC
    }

    if(plf_l1cache_lsdp_is_available()) {

        plf_l1cache_lsdp_reset();

#if (L1D_LOAD_STORE_DEP_PREDICTOR == 1)
        plf_l1cache_lsdp_enable();
#else
        plf_l1cache_lsdp_disable();
#endif // L1D_LOAD_STORE_DEP_PREDICTOR
    }

    if(plf_l1cache_prefetcher_is_available()) {

        plf_l1cache_prefetcher_reset();

#if (L1D_PREFETCHER == 1)
        plf_l1cache_prefetcher_enable();
#else
        plf_l1cache_prefetcher_disable();
#endif // L1D_PREFETCHER
    }

    if(plf_l1icache_prefetcher_is_available()) {

        plf_l1icache_prefetcher_reset();

#if (L1I_PREFETCHER == 1)
        plf_l1icache_prefetcher_enable();
#else
        plf_l1icache_prefetcher_disable();
#endif // L1I_PREFETCHER
    }

    if (plf_page_prefetcher_is_available()) {

        plf_page_prefetcher_reset();

#if (PAGE_PREFETCHER == 1)
        plf_page_prefetcher_enable();
#else
        plf_page_prefetcher_disable();
#endif // PAGE_PREFETCHER
    }

    if (plf_misaligned_access_is_available()) {

        plf_misaligned_access_reset();

#if (MISALIGNED_ACCESS == 1)
        plf_misaligned_access_enable();
#else
        plf_misaligned_access_disable();
#endif // MISALIGNED_ACCESS
    }
}

void __init plf_init_generic(void)
{
    // do relocations
    plf_init_relocate();
    // init TLS
    void* tp_ptr = NULL;
    asm volatile ("mv %0, tp;" : "=r"(tp_ptr) ::);
    plf_init_tls(tp_ptr);

#if defined(HAL_ENABLE_PERF) && (PLF_CORE_VARIANT_SCR > 1)
    perf_init_hpm();
#endif

    // init BSS
#ifdef HAL_SKIP_BSS_INIT
    memset_optnone(__bss_start, 0, 0);
#else
    memset_optnone(__bss_start, 0, (size_t)(__bss_end - __bss_start));
#endif // HAL_SKIP_BSS_INIT

    bss_complete_cycles = arch_cycle();

    pmp_early_init();

    // application early init callback
    app_early_init(true);

    // enable/init caches
    plf_l3cache_init();
    plf_l2cache_init();
    plf_l1cache_init();

    // enable/init MPU
    mpu_init();

    pmp_init();

#if PLF_MRT_SUPPORT
    mrt_init();
#endif // PLF_MRT_SUPPORT

    plf_init_features();

    // init peripheries
    con_init();
    rtc_init();
#ifdef PLF_LEDS_SUPPORT
    leds_init();
#endif

    // init smp
    plf_smp_init();
}

void plf_init(void) __attribute__((weak, alias("plf_init_generic")));
