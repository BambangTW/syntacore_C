cmake_minimum_required(VERSION 3.16)

project(de1-soc C ASM)

set(CMAKE_VERBOSE_MAKEFILE ON)

enable_testing()

function(setup_build_options)
    if(OPT STREQUAL "3lto")
        set(COMPILE_OPTIONS -O3 -funroll-loops)
        set(LINK_OPTIONS -flto)
    elseif(OPT STREQUAL "3")
        set(COMPILE_OPTIONS -O3)
    elseif(OPT STREQUAL "2")
        set(COMPILE_OPTIONS -O2)
    elseif(OPT STREQUAL "2g")
        set(COMPILE_OPTIONS -O2 -g3 -gdwarf-4)
    elseif(OPT STREQUAL "2lto")
        set(COMPILE_OPTIONS -O2 -funroll-loops)
        set(LINK_OPTIONS -flto)
    elseif(OPT STREQUAL "0")
        set(COMPILE_OPTIONS -O0)
    elseif(OPT STREQUAL "s")
        set(COMPILE_OPTIONS -Os)
    elseif(OPT STREQUAL "nano")
        set(COMPILE_OPTIONS -Os)
        set(LINK_OPTIONS --specs=nano.specs)
    elseif(OPT STREQUAL "gnano")
        set(COMPILE_OPTIONS -Og -g3 -gdwarf-4)
        set(LINK_OPTIONS --specs=nano.specs)
    else()
        set(OPT "g")
        set(COMPILE_OPTIONS -Og -g3 -gdwarf-4)
    endif()

    target_compile_options(de1-soc PRIVATE ${COMPILE_OPTIONS})
    target_link_options(de1-soc PRIVATE ${LINK_OPTIONS})

    string(REPLACE ";" " " COMPILE_OPTIONS "${COMPILE_OPTIONS}")
    string(REPLACE "SHELL:" "" COMPILE_OPTIONS "${COMPILE_OPTIONS}")
    target_compile_definitions(de1-soc PRIVATE FLAGS_STR="${COMPILE_OPTIONS}")
    message(STATUS "de1-soc OPT=${BUILD_OPTIONS} (${COMPILE_OPTIONS})")
endfunction()


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_ASM_FLAGS_RELEASE "")

if(CMAKE_SYSTEM_NAME MATCHES Generic)
    if(EXISTS ${SC_HAL_PATH}/CMakeLists.txt)
        add_subdirectory(${SC_HAL_PATH} hal)
        message(STATUS "SCR_HAL library will be built with '${PLATFORM}' platform")
    else()
        set(hal_DIR ${SC_HAL_PATH}/lib/${PLATFORM}/cmake)
        find_package(hal REQUIRED)
        message(STATUS "Using pre-built SCR_HAL library with '${PLATFORM}' platform")
    endif()
    string(TOLOWER ${PLATFORM} OUTPUT_DIR)
else()
    string(REGEX REPLACE "-" "_" LOWER_CPU ${CPU})
    string(TOLOWER ${LOWER_CPU} OUTPUT_DIR)
endif()

set(SAMPLE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SAMPLE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SAMPLE_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SAMPLE_OUTPUT_DIR})

add_executable(de1-soc main.c)

setup_build_options()

set(SAMPLE_ELF $<TARGET_FILE:de1-soc>)
set(SAMPLE_MAP ${SAMPLE_OUTPUT_DIR}/de1-soc.map)

target_link_options(de1-soc PRIVATE
    -Wl,-Map=${SAMPLE_MAP}
    -Wl,--gc-sections)

if(CMAKE_SYSTEM_NAME MATCHES Generic)
    target_link_libraries(de1-soc hal)
else()
    target_link_options(de1-soc PRIVATE -lm -lc)
endif()
