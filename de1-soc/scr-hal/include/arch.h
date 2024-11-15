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
/// @brief Architecture specific defs and inlines
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

#ifndef SCR_BSP_ARCH_H
#define SCR_BSP_ARCH_H

#if __riscv_xlen == 32
#define EXPAND32ADDR(x) (x)
#else
#define EXPAND32ADDR(x) (((((x) / 0x80000000) & 1) * 0xffffffff00000000) + (x))
#endif

#if __riscv_xlen == 32
#define TEST_ADDR      (0xff008000)
#else
#define TEST_ADDR      (0xffffffffff008000)
#endif
#define TEST_ADDR_BASE ((unsigned long *)(TEST_ADDR))

#include "plf.h"
#include "stringify.h"

#ifndef PLF_FPU_SUPPORT
#define PLF_FPU_SUPPORT (PLF_CORE_VARIANT_SCR > 3)
#endif // PLF_FPU_SUPPORT

#ifndef PLF_MPU_SUPPORT
#define PLF_MPU_SUPPORT (PLF_CORE_VARIANT_SCR > 1)
#endif // PLF_MPU_SUPPORT

#ifndef PLF_MMU_SUPPORT
#define PLF_MMU_SUPPORT (PLF_CORE_VARIANT_SCR >= 5)
#endif // PLF_MMU_SUPPORT

#ifndef PLF_PMP_SUPPORT
#define PLF_PMP_SUPPORT (PLF_CORE_VARIANT_SCR == 9)
#endif // PLF_PMP_SUPPORT

#ifndef PLF_CPU_CLK
#define PLF_CPU_CLK PLF_SYS_CLK
#endif

#ifndef PLF_RTC_SRC_EXTERNAL
#define PLF_RTC_SRC_EXTERNAL (0)
#endif // PLF_RTC_SRC_EXTERNAL

// platform info
#ifndef PLF_CPU_NAME
#if __riscv_xlen == 32
#define PLF_CPU_NAME STRINGIFY(PLF_CORE_VARIANT)
#else
#define PLF_CPU_ISA  "/RV" STRINGIFY(__riscv_xlen)
#define PLF_CPU_NAME STRINGIFY(PLF_CORE_VARIANT) PLF_CPU_ISA
#endif
#endif
#ifndef PLF_IMPL_STR
#define PLF_IMPL_STR "custom"
#endif

// default stack reserved space
#ifndef PLF_STACK_SIZE
#define PLF_STACK_SIZE 2048
#endif

// separate trap stack size (0 if not used)
#ifndef PLF_TRAP_STACK
#define PLF_TRAP_STACK 0
#endif // PLF_TRAP_STACK

#if PLF_SMP_SUPPORT
// minimal stack for SMP slaves
#ifndef PLF_HLS_MIN_STACK_SIZE
#define PLF_HLS_MIN_STACK_SIZE (2032 + PLF_TRAP_STACK)
#endif

// max cache line
#define PLF_MAX_CACHELINE_SIZE (64)

#ifndef PLF_SMP_HART_NUM
#define PLF_SMP_HART_NUM (__riscv_xlen / 8)
#endif

#ifndef PLF_SMP_HARTID_BASE
#define PLF_SMP_HARTID_BASE 0
#endif

#ifndef PLF_SMP_MASTER_HARTID
#define PLF_SMP_MASTER_HARTID PLF_SMP_HARTID_BASE
#endif

#if PLF_SMP_MASTER_HARTID >= (PLF_SMP_HART_NUM + PLF_SMP_HARTID_BASE)
#error PLF_SMP_MASTER_HARTID >= (PLF_SMP_HART_NUM + PLF_SMP_HARTID_BASE)
#endif

#ifndef PLF_SMP_SLAVE_ENTRY
#define PLF_SMP_SLAVE_ENTRY smp_slave_entry
#endif

#endif // PLF_SMP_SUPPORT

#ifndef PLF_ATOMIC_SUPPORTED
#ifdef __riscv_atomic
#define PLF_ATOMIC_SUPPORTED 1
#else // __riscv_atomic
#define PLF_ATOMIC_SUPPORTED 0
#endif // __riscv_atomic
#endif // !PLF_ATOMIC_SUPPORTED

// default console headers
#ifndef PLF_DEFAULT_CONSOLE
#if defined(PLF_UART0_16550)
#define PLF_DEFAULT_CONSOLE "drivers/uart_16550.h"
#elif defined(PLF_UART0_16550_DW)
#define PLF_DEFAULT_CONSOLE "drivers/uart_dw.h"
#elif defined(PLF_SIM_CONSOLE)
#define PLF_DEFAULT_CONSOLE "drivers/uart_sim.h"
#elif defined(PLF_UART0_SIFIVE)
#define PLF_DEFAULT_CONSOLE "drivers/uart_sifive.h"
#elif defined(PLF_HTIF_CONSOLE)
#define PLF_DEFAULT_CONSOLE "drivers/htif.h"
#elif defined(PLF_VIRTIO_CONSOLE)
#define PLF_DEFAULT_CONSOLE "drivers/virtio.h"
#endif // search default console
#endif // PLF_DEFAULT_CONSOLE

// trap handler smart save/restore
#ifndef PLF_SAVE_RESTORE_REGS331_SUB
#define PLF_SAVE_RESTORE_REGS331_SUB 0
#endif // PLF_SAVE_RESTORE_REGS331_SUB

#ifndef PLF_ARCH_BASIC_SPINLOCKS
#define PLF_ARCH_TICKET_SPINLOCKS
#endif //PLF_ARCH_BASIC_SPINLOCKS

// trap causes
// interrupts
#define TRAP_CAUSE_INTERRUPT_FLAG (1UL << (__riscv_xlen - 1))
#define TRAP_CAUSE_INT_SSOFT      (1)
#define TRAP_CAUSE_INT_MSOFT      (3)
#define TRAP_CAUSE_INT_STIME      (5)
#define TRAP_CAUSE_INT_MTIME      (7)
#define TRAP_CAUSE_INT_SEXT       (9)
#define TRAP_CAUSE_INT_MEXT       (11)
// exceptions
#define TRAP_CAUSE_EXC_FETCH_ALIGN  (0)
#define TRAP_CAUSE_EXC_FETCH_ACCESS (1)
#define TRAP_CAUSE_EXC_ILLEGAL      (2)
#define TRAP_CAUSE_EXC_BREAKPOINT   (3)
#define TRAP_CAUSE_EXC_LOAD_ALIGN   (4)
#define TRAP_CAUSE_EXC_LOAD_ACCESS  (5)
#define TRAP_CAUSE_EXC_STORE_ALIGN  (6)
#define TRAP_CAUSE_EXC_STORE_ACCESS (7)
#define TRAP_CAUSE_EXC_UECALL       (8)
#define TRAP_CAUSE_EXC_SECALL       (9)
#define TRAP_CAUSE_EXC_RESERVED1    (10)
#define TRAP_CAUSE_EXC_MECALL       (11)
#define TRAP_CAUSE_EXC_FETCH_PAGE   (12)
#define TRAP_CAUSE_EXC_LOAD_PAGE    (13)
#define TRAP_CAUSE_EXC_RESERVED2    (14)
#define TRAP_CAUSE_EXC_STORE_PAGE   (15)
#define TRAP_CAUSE_EXC_G_FETCH_PAGE (20)
#define TRAP_CAUSE_EXC_G_LOAD_PAGE  (21)
#define TRAP_CAUSE_EXC_G_VIRT_INST  (22)
#define TRAP_CAUSE_EXC_G_STORE_PAGE (23)

// mie/mip bits
#define MIE_MSOFTWARE (1 << TRAP_CAUSE_INT_MSOFT)
#define MIE_MTIMER    (1 << TRAP_CAUSE_INT_MTIME)
#define MIE_MEXTERNAL (1 << TRAP_CAUSE_INT_MEXT)
// mstatus bits
#define MSTATUS_SIE   (1UL << 1)
#define MSTATUS_MIE   (1UL << 3)
#define MSTATUS_SPIE  (1UL << 5)
#define MSTATUS_UBE   (1UL << 6)
#define MSTATUS_MPIE  (1UL << 7)
#define MSTATUS_SPP   (1UL << 8)
#define MSTATUS_VS    (3UL << 9)
#define MSTATUS_MPP   (3UL << 11)
#define MSTATUS_FS    (3UL << 13)
#define MSTATUS_XS    (3UL << 15)
#define MSTATUS_MPRV  (1UL << 17)
#define MSTATUS_SUM   (1UL << 18)
#define MSTATUS_MXR   (1UL << 19)
#define MSTATUS_TVM   (1UL << 20)
#define MSTATUS_TW    (1UL << 21)
#define MSTATUS_UXL   (3UL << 32)
#define MSTATUS_SXL   (3UL << 34)
#define MSTATUS_SBE   (1UL << 36)
#define MSTATUS_MBE   (1UL << 37)
#define MSTATUS_SD    (1UL << 63)

#define MSTATUS_MPP_SHIFT (11)

#define PRV_U (0)
#define PRV_S (1)
#define PRV_M (3)

// sstatus bits
#define SSTATUS_SIE   (1UL << 1)
#define SSTATUS_SPIE  (1UL << 5)
#define SSTATUS_UBE   (1UL << 6)
#define SSTATUS_SPP   (1UL << 8)
#define SSTATUS_VS    (3UL << 9)
#define SSTATUS_FS    (3UL << 13)
#define SSTATUS_XS    (3UL << 15)
#define SSTATUS_SUM   (1UL << 18)
#define SSTATUS_MXR   (1UL << 19)
#define SSTATUS_UXL   (3UL << 32)
#define SSTATUS_SD    (1UL << 63)

#define SSTATUS_SPP_SHIFT (8)

// hstatus bits
#define HSTATUS_VSBE  (1UL << 5)
#define HSTATUS_GVA   (1UL << 6)
#define HSTATUS_SPV   (1UL << 7)
#define HSTATUS_SPVP  (1UL << 8)
#define HSTATUS_HU    (1UL << 9)
#define HSTATUS_VGEIN (63UL << 12)
#define HSTATUS_VTVM  (1UL << 20)
#define HSTATUS_VTW   (1UL << 21)
#define HSTATUS_VTSR  (1UL << 22)
#define HSTATUS_VSXL  (3UL << 32)

// SCR9 core configuration
#define HAL_CORECONFIG_SCR9_HEAVY 1
#define HAL_CORECONFIG_SCR9_LITE  2

#ifndef __ASSEMBLER__

#include "csr.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long read_safe_m(unsigned long addr, unsigned long* val);

static inline __attribute__((always_inline)) unsigned long arch_misa(void) { return read_csr(misa); }

static inline __attribute__((always_inline)) unsigned long arch_cpuid(void) { return arch_misa(); }

static inline __attribute__((always_inline)) unsigned long arch_impid(void) { return read_csr(mimpid); }

static inline __attribute__((always_inline)) unsigned long arch_vendorid(void) { return read_csr(mvendorid); }

static inline __attribute__((always_inline)) unsigned long arch_archid(void) { return read_csr(marchid); }

static inline __attribute__((always_inline)) unsigned long arch_hartid(void) { return read_csr(mhartid); }

static inline __attribute__((always_inline)) unsigned long arch_mtval(void) { return read_csr(mtval); }

unsigned long arch_coreid(void);
unsigned long hal_arch_coreconfig();

static inline __attribute__((always_inline)) uint64_t arch_cycle(void)
{
#if __riscv_xlen == 32
    return read_csr64(cycle);
#else  //  __riscv_xlen == 32
    return read_csr(cycle);
#endif // __riscv_xlen == 32
}

static inline __attribute__((always_inline)) uint64_t arch_instret(void)
{
#if __riscv_xlen == 32
    return read_csr64(instret);
#else  //  __riscv_xlen == 32
    return read_csr(instret);
#endif // __riscv_xlen == 32
}

static inline __attribute__((always_inline)) void ifence(void) {
#ifdef __riscv_zifencei
    asm volatile("fence.i" ::: "memory");
#endif
}

static inline __attribute__((always_inline)) void fence(void) { asm volatile("fence" ::: "memory"); }

static inline void wfi(void) { asm volatile("wfi" ::: "memory"); }

void __attribute__((noreturn)) _hart_halt(int ret_code);
void __attribute__((noreturn)) _hart_halt1(void);

static inline void cpu_relax(void)
{
#ifdef __riscv_m
    long dummy;
    asm volatile("div %0, %0, zero" : "=r"(dummy)::"memory");
#else
    asm volatile ("nop");
#endif
}

static inline void sfence_vma(void) { asm volatile("sfence.vma" ::: "memory"); }

static inline void sfence_vma_addr(uintptr_t addr) { asm volatile("sfence.vma %0, zero" ::"r"(addr) : "memory"); }

static inline void sfence_vma_asid(unsigned asid) { asm volatile("sfence.vma zero, %0" ::"r"(asid) : "memory"); }

static inline void sfence_vma_addr_asid(uintptr_t addr, unsigned asid)
{
    asm volatile("sfence.vma %0, %1" ::"r"(addr), "r"(asid) : "memory");
}

static inline void flush_tlb(void) { sfence_vma(); }

static inline __attribute__((always_inline)) void hfence_gvma(void)
{
    // asm volatile("hfence.gvma" ::: "memory");
    // hfence.gvma (hfence.gvma zero,zero) = 0x62000073
    asm volatile (".insn r 0x73, 0, 0x31, x0, x0, x0" ::: "memory");
}

static inline __attribute__((always_inline)) void hfence_vvma(void)
{
    // asm volatile("hfence.vvma" ::: "memory");
    // hfence.vvma (hfence.vvma zero,zero) = 0x22000073
    asm volatile (".insn r 0x73, 0, 0x11, x0, x0, x0" ::: "memory");
}

static inline unsigned long get_build_id(void)
{
#if PLF_BLD_ID_ADDR
    unsigned long val = 0;

    if (!read_safe_m(PLF_BLD_ID_ADDR, &val)) {
        return val;
    }
#endif // PLF_BLD_ID_ADDR
    return 0;
}

static inline uint32_t get_system_id(void)
{
#ifdef PLF_SYS_ID_ADDR
    return *(volatile uint32_t*)PLF_SYS_ID_ADDR;
#else
    return 0;
#endif
}

static inline int supports_extension(char ext) { return (arch_misa() & (1UL << (ext - 'A'))) != 0; }

#if PLF_SMP_SUPPORT
static inline unsigned long get_hls_mem_size(void) {
   extern char __TLS_SIZE_OFFSET__[], __TEXT_START__[];
   return (unsigned long)(__TLS_SIZE_OFFSET__ - __TEXT_START__ + PLF_MAX_CACHELINE_SIZE + PLF_HLS_MIN_STACK_SIZE);
}
#endif

static inline __attribute__((always_inline)) unsigned int hal_qemu_sign(void) {
    // MIMPID bits 8..31 (day/month/year fields) contain bitstream release date.
    // QEMU-emulated harts zeroize these fields.
    // Pre 9.0.2 QEMU used another scheme, when MIMPID was set to MARCHID.
    return ((arch_impid() & 0xffffff00U) == 0U) ||
        (arch_impid() == arch_archid());
}

// generic platform initialization function
void plf_init_generic(void);

#ifdef __cplusplus
}
#endif
#endif // __ASSEMBLER__
#endif // SCR_BSP_ARCH_H
