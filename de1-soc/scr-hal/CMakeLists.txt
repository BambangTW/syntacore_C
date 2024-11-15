# Copyright (C) 2023, Syntacore Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Syntacore's RISC-V HAL

cmake_minimum_required(VERSION 3.16)

include(${CMAKE_CURRENT_LIST_DIR}/version.cmake)

project(hal VERSION ${HAL_VERSION}
        LANGUAGES C CXX ASM
        DESCRIPTION "Hardware Abstraction Layer for Syntacore RISC-V clusters"
        HOMEPAGE_URL https://syntacore.com/page/products/sw-tools)

add_library(hal STATIC)
add_library(bsp ALIAS hal)

option(HAL_ENABLE_PERF     "Enable perf counters" ON)
option(HAL_META_INFO       "Add meta information to ELF file" OFF)
option(HAL_QEMU_AUTOEXIT   "Build with QEMU_AUTOEXIT feature" ON)
option(HAL_SKIP_BSS_INIT   "Do not clear BSS at startup" OFF)
option(HAL_SKIP_LD_SCRIPT  "Do not export ld script to users" OFF)
option(HAL_ENABLE_SEMIHOST "Enable RISC-V default semihost syscalls" OFF)

set(HAL_BPU_EARLY_BRANCH_RESOLUTION AUTO CACHE STRING "Enable/disable BPU early branch resolution in HAL")
set(HAL_BPU_LOOP_PREDICTOR AUTO CACHE STRING "Enable/disable BPU loop predictor in HAL")
set(HAL_L1D_STORE_MERGE AUTO CACHE STRING "Enable/disable L1D store merge in HAL")
set(HAL_L1D_LOAD_SPEC AUTO CACHE STRING "Enable/disable L1D loads speculation in HAL")
set(HAL_L1D_LOAD_OVER_STORE_SPEC AUTO CACHE STRING "Enable/disable L1D loads over stores speculation in HAL")
set(HAL_L1D_LOAD_STORE_DEP_PREDICTOR AUTO CACHE STRING "Enable/disable L1D load store dependency predictor in HAL")
set(HAL_L1D_PREFETCHER ON CACHE STRING "Enable/disable L1D prefetcher in HAL")
set(HAL_L1I_PREFETCHER AUTO CACHE STRING "Enable/disable L1I prefetcher in HAL")
set(HAL_MARCH ${TARGET_MARCH} CACHE STRING "The machine architecture (-march) for HAL code")
set(HAL_MISALIGNED_ACCESS AUTO CACHE STRING "Enable/disable misaligned access support in HAL")
set(HAL_PAGE_PREFETCHER AUTO CACHE STRING "Enable/disable page prefetcher in HAL")
set(HAL_PRINTF_LEVEL "3" CACHE STRING "printf() implementation levels (0-3)")
set(PLF_MASTER_HART "0" CACHE STRING "Master hart id to performs main HAL initialization")

function(var_alias _v _a)
    if(DEFINED ${_a})
        message(WARNING "${_a} is deprecated and will removed in the next major version. Please use ${_v} instead.")
        set(${_v} ${${_a}} CACHE STRING "" FORCE)
    endif()
endfunction(var_alias)

var_alias(HAL_APP_EXIT_PRINT_MSG BSP_APP_EXIT_PRINT_MSG)
var_alias(HAL_ENABLE_PERF ENABLE_PERF)
var_alias(HAL_QEMU_AUTOEXIT QEMU_AUTOEXIT)
var_alias(HAL_SKIP_BSS_INIT BSP_SKIP_BSS_INIT)
var_alias(HAL_SKIP_LD_SCRIPT BSP_SKIP_LD_SCRIPT)
var_alias(HAL_PRINTF_LEVEL PRINTF_LEVEL)

function(hal_add_on_off_auto_option _option)
    var_alias(HAL_${_option} BSP_${_option})
    if(HAL_${_option})
        if(NOT HAL_${_option} STREQUAL "AUTO")
            target_compile_definitions(hal PRIVATE ${_option}=1)
        endif()
    else()
        target_compile_definitions(hal PRIVATE ${_option}=0)
    endif()
endfunction()

if(NOT DEFINED PLATFORM_CONFIG_DIR)
    set(PLATFORM_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM})
endif()

include(${PLATFORM_CONFIG_DIR}/plf.cmake)

if(NOT HAL_MARCH STREQUAL TARGET_MARCH)
    macro(_set_march _FLAGS NEW_MARCH)
        string(REGEX REPLACE "-march=[^ ]*" "-march=${NEW_MARCH}" ${_FLAGS} ${${_FLAGS}})
    endmacro()

    _set_march(CMAKE_C_FLAGS ${HAL_MARCH})
    _set_march(CMAKE_CXX_FLAGS ${HAL_MARCH})
    _set_march(CMAKE_ASM_FLAGS ${HAL_MARCH})
endif()

macro(_rm_opt _FLAGS OPT)
    string(REGEX REPLACE "${OPT}=[^ ]*" "" ${_FLAGS} ${${_FLAGS}})
endmacro()

# Don't use -mcpu when building scr-hal code
_rm_opt(CMAKE_C_FLAGS "-mcpu")
_rm_opt(CMAKE_CXX_FLAGS "-mcpu")
_rm_opt(CMAKE_ASM_FLAGS "-mcpu")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(HAL_QEMU_AUTOEXIT)
    target_compile_definitions(hal PUBLIC HAL_QEMU_AUTOEXIT QEMU_AUTOEXIT)
endif()

if(HAL_SKIP_BSS_INIT)
    target_compile_definitions(hal PRIVATE HAL_SKIP_BSS_INIT)
endif()

if(ENABLE_RVV OR MARCH MATCHES "rv64[^\\s]*v" OR HAL_MARCH MATCHES "rv64[^\\s]*v")
    # Enable VPU if RVV is enabled explicitly or implicitly
    target_compile_definitions(hal PRIVATE PLF_VPU_SUPPORT)
endif()

if(HAL_ENABLE_PERF)
    target_compile_definitions(hal PRIVATE HAL_ENABLE_PERF)
endif()

if(HAL_APP_EXIT_PRINT_MSG)
    message(WARNING "HAL_APP_EXIT_PRINT_MSG is deprecated and will be removed in the next version.")
    target_compile_definitions(hal PRIVATE HAL_APP_EXIT_PRINT_MSG="${HAL_APP_EXIT_PRINT_MSG}")
endif()

# Export all PLF_* variables
get_cmake_property(_plf_vars VARIABLES)
list(FILTER _plf_vars INCLUDE REGEX "^PLF_")
foreach (_var ${_plf_vars})
    target_compile_definitions(hal PUBLIC ${_var}=${${_var}})
endforeach()

target_compile_definitions(hal PUBLIC PLF_SMP_MASTER_HARTID=${PLF_MASTER_HART})

if(HAL_ENABLE_SEMIHOST)
    target_compile_definitions(hal PUBLIC HAL_ENABLE_SEMIHOST)
    if(NOT HAL_PRINTF_LEVEL STREQUAL "0")
        set(HAL_PRINTF_LEVEL 0)
        message(WARNING "\"HAL_PRINTF_LEVEL\" changed to 0 due to \"HAL_ENABLE_SEMIHOST\" option activated")
    endif()
endif()

hal_add_on_off_auto_option(BPU_EARLY_BRANCH_RESOLUTION)
hal_add_on_off_auto_option(BPU_LOOP_PREDICTOR)
hal_add_on_off_auto_option(L1D_STORE_MERGE)
hal_add_on_off_auto_option(L1D_LOAD_OVER_STORE_SPEC)
hal_add_on_off_auto_option(L1D_LOAD_SPEC)
hal_add_on_off_auto_option(L1D_LOAD_STORE_DEP_PREDICTOR)
hal_add_on_off_auto_option(L1D_PREFETCHER)
hal_add_on_off_auto_option(L1I_PREFETCHER)
hal_add_on_off_auto_option(PAGE_PREFETCHER)
hal_add_on_off_auto_option(MISALIGNED_ACCESS)

target_sources(hal PRIVATE
               src/drivers/aia.c
               src/drivers/cache.c
               src/drivers/console.c
               src/drivers/ctrl_l3.c
               src/drivers/htif.c
               src/drivers/leds.c
               src/drivers/mpu.c
               src/drivers/pmp.c
               src/drivers/pmu.c
               src/drivers/rtc.c

               src/libc/stubs.c
               src/libc/syscalls.c

               src/sys/arch.c
               src/sys/crt0_110.S
               src/sys/lock.c
               src/sys/startup.cpp
               src/sys/sys_init.c
               src/sys/sys_reloc.c
               src/sys/sys_utils.S
               src/sys/utils.c)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM}/plf.c)
    target_sources(hal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM}/plf.c)
endif()

# make hal_VERSION visible to external projects
set(hal_VERSION ${hal_VERSION} CACHE STRING "SCR-HAL version" FORCE)

find_package(Git)

if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE HAL_GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

    execute_process(COMMAND ${GIT_EXECUTABLE} diff-index --name-only HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE _git_uncommited
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

    if(_git_uncommited)
        set(HAL_GIT_COMMIT "${HAL_GIT_COMMIT}-dirty")
    endif()

    target_compile_definitions(hal PUBLIC HAL_GIT_COMMIT="${HAL_GIT_COMMIT}")
endif()

target_include_directories(hal PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/drivers>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/libc>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/common>"
    "$<BUILD_INTERFACE:${PLATFORM_CONFIG_DIR}>"
    "$<INSTALL_INTERFACE:include/drivers>"
    "$<INSTALL_INTERFACE:include/libc>"
    "$<INSTALL_INTERFACE:platform/common>"
    "$<INSTALL_INTERFACE:platform/${PLATFORM}>")

if(NOT HAL_PRINTF_LEVEL STREQUAL "0")
    target_sources(hal PRIVATE
                   src/libc/printf.c)

    target_include_directories(hal PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/libc>"
        "$<INSTALL_INTERFACE:include/libc>")
endif()

target_compile_definitions(hal PUBLIC PRINTF_LEVEL=${HAL_PRINTF_LEVEL})

if(HAL_META_INFO)
    # Link meta information section to the ELF file when
    # the application uses the HAL linker script.
    # See "ELF meta information" in README.md
    get_cmake_property(_vars VARIABLES)

    list(FILTER _vars INCLUDE REGEX
        "^APP_|^BSP_|^HAL_|^PLF_|^PLATFORM$|^TARGET_M|^MABI$|^MARCH$|^MTUNE$|^MCPU$|^QEMU_|^ENABLE_|^MB_|^SC_FPGA_|^GROUND_RULES$|^LTO$|_VERSION$")
    list(FILTER _vars EXCLUDE REGEX "CMAKE|^PROJECT_|^HAL_META_INFO|_DIR|DIR_|DIRECTOR|_PATH|PATH_")
    list(REMOVE_DUPLICATES _vars)

    foreach (_var ${_vars})
        set(_mi "${_mi}\t\"${_var}=${${_var}}\\0\"\n")
    endforeach()

    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metainfo.c CONTENT
        "__attribute__((section(\".metainfo\"),used)) char _scr_hal_metainfo[] = {\n${_mi}};\n")

    target_sources(hal PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/metainfo.c)
endif()

target_compile_options(hal PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
    $<$<COMPILE_LANGUAGE:CXX>:
    -fno-asynchronous-unwind-tables
    -fno-non-call-exceptions
    -fno-exceptions
    -fno-use-cxa-atexit
    -fno-rtti>
    -static
    -Os)

target_link_directories(hal PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform/common>"
    "$<BUILD_INTERFACE:${PLATFORM_CONFIG_DIR}>"
    "$<INSTALL_INTERFACE:platform/common>"
    "$<INSTALL_INTERFACE:platform/${PLATFORM}>")

if(NOT DEFINED HAL_PLATFORM_LD_SCRIPT)
    set(HAL_PLATFORM_LD_SCRIPT ${PLATFORM_CONFIG_DIR}/mem.ld)
endif()

configure_file(${HAL_PLATFORM_LD_SCRIPT}
               ${CMAKE_CURRENT_BINARY_DIR}/mem.ld @ONLY)

if(NOT HAL_SKIP_LD_SCRIPT)
    target_link_options(hal PUBLIC "$<BUILD_INTERFACE:-T${CMAKE_CURRENT_BINARY_DIR}/mem.ld>")
endif()

install(DIRECTORY include
    DESTINATION . COMPONENT hal)

install(DIRECTORY platform/common
    DESTINATION platform  COMPONENT hal)

install(DIRECTORY platform/${PLATFORM}
    DESTINATION platform COMPONENT hal)

# overwrite by real linker script
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mem.ld
    DESTINATION platform/${PLATFORM} COMPONENT hal)

install(TARGETS hal EXPORT halTargets
    ARCHIVE DESTINATION lib/${PLATFORM}
    COMPONENT hal
    INCLUDES DESTINATION include)

install(TARGETS hal EXPORT bspTargets
    ARCHIVE DESTINATION lib/${PLATFORM}
    COMPONENT hal
    INCLUDES DESTINATION include)

install(EXPORT halTargets
    DESTINATION lib/${PLATFORM}/cmake COMPONENT hal)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/halConfigVersion.cmake"
    VERSION ${Upstream_VERSION}
    COMPATIBILITY AnyNewerVersion
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/halConfig.cmake
           "include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)\n")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/halTargets-linker.cmake
           "if(NOT HAL_SKIP_LD_SCRIPT)\n"
           "    target_link_options(hal INTERFACE -T \$\{_IMPORT_PREFIX\}/platform/${PLATFORM}/mem.ld)\n"
           "endif()\n")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/bspConfig.cmake
           "include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)\n"
           "add_library(bsp ALIAS hal)\n")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/bspTargets-linker.cmake
           "if(NOT HAL_SKIP_LD_SCRIPT)\n"
           "    target_link_options(hal INTERFACE -T \$\{_IMPORT_PREFIX\}/platform/${PLATFORM}/mem.ld)\n"
           "endif()\n")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/halConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/halTargets-linker.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/halConfigVersion.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/bspConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/bspTargets-linker.cmake"
              DESTINATION lib/${PLATFORM}/cmake COMPONENT hal)

if(CMAKE_PROJECT_NAME MATCHES "hal")
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
    endif()
endif()
