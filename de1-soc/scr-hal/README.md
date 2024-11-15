# SCR-HAL

The **SCR-HAL** project contains a low level start-up code, drivers to control HW components (cache, interrupt controller, UART, etc.) and simplified `printf()` implementation.
It also contains linker scripts and the set of platforms where each platform is described with:

| File                | Description                                             |
|:--------------------|:--------------------------------------------------------|
| plf.h               | Memory regions, available hardware and features         |
| plf.cmake           | **MARCH**, **MABI** and **MCPU** definitions            |
| mem.ld              | Platform memory map                                     |

The SCR-HAL can be used as CMake sub-project and can be built as a source or pre-built library.

Additionally you need to download the following repos to be able to build SCR-HAL:
**cmake-toolchains** project to be able to configure baremetal build environment.

# Environment setup

Make sure that you have **cmake-toolchains** project in your workspace additionally to SCR-HAL.

Setup toolchains:
```bash
# Setup GCC and Clang toolchains:
export SC_DT_PATH=/opt/sc-dt

# Setup paths to GCC and Clang toolchains:
export SC_GCC_PATH=${SC_DT_PATH}/riscv-gcc/
export SC_CLANG_PATH=${SC_DT_PATH}/llvm/
```
# Configure

Run the following CMake configuration command (`PLATFORM` is required, other parameters are optional):
```bash
cmake -S . -B ./build-clang-scr7 \
    -DCMAKE_TOOLCHAIN_FILE=../cmake-toolchains/riscv64-elf-clang.cmake \
    -DPLATFORM=scr7_l3
```
Possible output:
```
-- The C compiler identification is Clang 16.0.0
-- The CXX compiler identification is Clang 16.0.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /opt/syntacore/riscv-tools/scr_llvm_16.0.0_gd706a6a51f94_d230119-151852/bin/clang - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /opt/syntacore/riscv-tools/scr_llvm_16.0.0_gd706a6a51f94_d230119-151852/bin/clang - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- The ASM compiler identification is Clang with GNU-like command-line
-- Found assembler: /opt/syntacore/riscv-tools/scr_llvm_16.0.0_gd706a6a51f94_d230119-151852/bin/clang
-- Configuring done
-- Generating done
-- Build files have been written to: /home/sources/microbench_project/scr-hal/build-clang-scr7
```
## Additional parameters

| Parameter          | Description                             | Default value |
|:-------------------|:----------------------------------------|:--------------|
| ENABLE_RVV         | Enable vector extension                 | OFF           |
| HAL_APP_EXIT_PRINT_MSG | Deprecated, see [The application exit message()](#hal_app_exit_message) section | (none) |
| HAL_ENABLE_PERF    | Configure performance counters at startup | ON          |
| HAL_MARCH          | Override -march compiler parameter for HAL only | same as MARCH |
| HAL_META_INFO      | Add meta information to target ELF file | OFF |
| HAL_PRINTF_LEVEL   | printf() implementation levels          | 3             |
| HAL_QEMU_AUTOEXIT  | Build scr-hal with QEMU_AUTOEXIT feature | ON            |
| HAL_SKIP_BSS_INIT  | Do not clear BSS at startup | OFF           |
| HAL_SKIP_LD_SCRIPT | Do not use platform's linker script     | OFF           |
| HAL_ENABLE_SEMIHOST | Build scr-hal for semihosting usage also set HAL_PRINTF_LEVEL to 0    | OFF           |
| MARCH              | Override -march compiler parameter      |  Platform specific (defined in plf.cmake) |
| MABI               | Override -mabi compiler parameter       | the same as above |
| MCPU               | Override -mcpu compiler parameter       | the same as above |
| PLF_CPU_CLK        | Hart clock frequency | (depends on platform) |
| PLF_MASTER_HART    | Master hart id to performs main HAL initialization | 0             |
| PLF_SYS_CLK        | L3 cluster clock frequency | (depends on platform) |
| PLF_UART_CLK       | UART clock frequency | (depends on platform) |

see also platform/{PLATFORM}/README.md for platform-specific options.

## Additional 3 states (ON, OFF, AUTO) options

`AUTO` means that the hard reset state is used.

| Parameter          | Description                            | Default value |
|:-------------------|:---------------------------------------|:--------------|
| HAL_BPU_EARLY_BRANCH_RESOLUTION | Enable/disable BPU early branch resolution | AUTO |
| HAL_BPU_LOOP_PREDICTOR | Enable/disable BPU loop predictor | AUTO |
| HAL_L1D_STORE_MERGE | Enable/disable L1D store merge | AUTO |
| HAL_L1D_LOAD_SPEC | Enable/disable L1D loads speculation | AUTO |
| HAL_L1D_LOAD_OVER_STORE_SPEC | Enable/disable L1D loads over stores speculation | AUTO |
| HAL_L1D_LOAD_STORE_DEP_PREDICTOR | Enable/disable L1D load store dependency predictor | AUTO |
| HAL_L1D_PREFETCHER | Enable/disable L1D prefetcher | ON |
| HAL_L1I_PREFETCHER | Enable/disable L1I prefetcher | AUTO |
| HAL_PAGE_PREFETCHER | Enable/disable page prefetcher | AUTO |
| HAL_MISALIGNED_ACCESS | Enable/disable misaligned access support | AUTO |

### HAL_META_INFO (ELF file meta information)

An additional non-loadable section with meta information in ASCII string format is linked with the target ELF file when the HAL linker script is used (default behavior).
This data contains most information CMake variables. The user can provide their own supporting data using CMake variables, for example:

```bash
$ cmake -S . -B ./build \
    -DCMAKE_TOOLCHAIN_FILE=../cmake-toolchains/${TOOLCHAIN} \
    -DPLATFORM=scr1 \
    -DHAL_META_INFO=ON \
    -DPLF_MASTER_HART=0 \
    -DHAL_AUX_INFO="someinfo" \
    -DAPP_AUX_INFO="someinfo2" \
    ...
```

To read data:
```bash
$ readelf -p .metainfo sample_apps/build/scr1/sysinfo.elf

String dump of section '.metainfo':
  [     0]  HAL_BPU_EARLY_BRANCH_RESOLUTION=AUTO
  [    25]  HAL_BPU_LOOP_PREDICTOR=AUTO
  [    41]  HAL_ENABLE_PERF=ON
  [    54]  HAL_ENABLE_SEMIHOST=OFF
  [    6c]  HAL_GIT_COMMIT=c747952-dirty
  [    89]  HAL_L1D_LOAD_OVER_STORE_SPEC=AUTO
  [    ab]  HAL_L1D_LOAD_SPEC=AUTO
  [    c2]  HAL_L1D_LOAD_STORE_DEP_PREDICTOR=AUTO
  [    e8]  HAL_L1D_PREFETCHER=ON
  [    fe]  HAL_L1D_STORE_MERGE=AUTO
  [   117]  HAL_L1I_PREFETCHER=AUTO
  [   12f]  HAL_MARCH=rv32im_zifencei_zicsr
  [   14f]  HAL_MISALIGNED_ACCESS=AUTO
  [   16a]  HAL_PAGE_PREFETCHER=AUTO
  [   183]  HAL_PRINTF_LEVEL=3
  [   196]  HAL_QEMU_AUTOEXIT=ON
  [   1ab]  HAL_SKIP_BSS_INIT=OFF
  [   1c1]  HAL_SKIP_LD_SCRIPT=OFF
  [   1d8]  HAL_VERSION=1.1.0
  [   1ea]  MABI=ilp32
  [   1f5]  MARCH=rv32im
  [   202]  MCPU=scr1
  [   20c]  PLATFORM=scr1
  [   21a]  PLF_MASTER_HART=0
  [   22c]  QEMU_MACHINE=scr1
  [   23e]  QEMU_SMP=1
  [   249]  SAMPLE_APPS_VERSION=1.0.1
  [   263]  TARGET_MABI=ilp32
  [   275]  TARGET_MARCH=rv32im_zifencei_zicsr
  [   298]  TARGET_MCPU=scr1
  [   2a9]  cmake_toolchains_VERSION=1.0.0
  [   2c8]  hal_VERSION=1.1.0
  [   2da]  sample_apps_VERSION=1.0.1
```

### HAL_PRINTF_LEVEL
Supported internal `printf()` levels:

| HAL_PRINTF_LEVEL     | Description                                                                    |
|:---------------------|:-------------------------------------------------------------------------------|
| 0                    | Use external libc output functions                                             |
| 1                    | A minimal version that just offers integer formatting and usual modifiers      |
| 2                    | Long long and ptrdiff_t types support                                          |
| 3 (default)          | Floating point support                                                         |

# Build

This section describes how to make a pre-built SCR-HAL library.

Execute the following command:
```bash
cd build-clang-scr7 && make
```
which is the same as:
```bash
cmake --build build-clang-scr7 --target hal
```
Possible output:
```
...
[  7%] Building C object CMakeFiles/scr-hal.dir/src/drivers/mpu.c.obj
...
[ 53%] Building C object CMakeFiles/scr-hal.dir/src/sys/lock.c.obj
...
[100%] Linking CXX static library libhal.a
```

Note that SCR-HAL code is always built with `-Os` optimization.

# Install

This section describes how to install a pre-built SCR-HAL library.

Execute the following command:
```bash
cmake --install build-clang-scr7 --prefix install
```
Possible output:
```
-- Install configuration: ""
-- Installing: /home/sources/microbench_project/scr-hal/install/./include
-- Installing: /home/sources/microbench_project/scr-hal/install/./include/arch.h
...
-- Installing: /home/sources/microbench_project/scr-hal/install/./include/libc/ftoa_engine.h
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3/plf.h
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3/plf.mk
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3/plf.ini
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3/plf.cmake
-- Installing: /home/sources/microbench_project/scr-hal/install/platform/scr7_l3/ram.ld
-- Installing: /home/sources/microbench_project/scr-hal/install/./ldscripts
-- Installing: /home/sources/microbench_project/scr-hal/install/./ldscripts/bsp0.lds
-- Installing: /home/sources/microbench_project/scr-hal/install/./ldscripts/bsp_reloc.lds
-- Installing: /home/sources/microbench_project/scr-hal/install/lib/scr7_l3/libhal.a
-- Installing: /home/sources/microbench_project/scr-hal/install/lib/scr7_l3/cmake/halTargets.cmake
-- Installing: /home/sources/microbench_project/scr-hal/install/lib/scr7_l3/cmake/halTargets-noconfig.cmake
-- Installing: /home/sources/microbench_project/scr-hal/install/lib/scr7_l3/cmake/halConfig.cmake
-- Installing: /home/sources/microbench_project/scr-hal/install/lib/scr7_l3/cmake/halTargets-linker.cmake
```

All installation files are grouped into a component named `hal`.
From user projects, the hal they built can be installed with the following command:
```bash
cmake --install build-clang-scr7 --prefix install --component hal
```

# API

## Early initialization of application

To perform early peripheral initialization, use the `void app_early_init(bool boot_hart)` callback. During the bootstrap process, this function will be called with the `true` argument during common platform initialization and/or with the `false` argument during the platform slave smp initialization.
For example:
```
void app_early_init(bool boot_hart);

...

void app_early_init(bool boot_hart)
{
...
    if (boot_hart)
    {
        init_some();
    }
...
}
```

## The application exit message <a name="hal_app_exit_message">

The weak pointer `hal_app_exit_message` can be redefined by an application to output message after the application has finished working.
By default there is no output.

For example:
```
const char *hal_app_exit_message = "app_exit: application finished\n";

void main()
{
//  ...
    return 0;
}
```
