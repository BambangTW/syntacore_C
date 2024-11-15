#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hal" for configuration ""
set_property(TARGET hal APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(hal PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "ASM;C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/scr7_l2/libhal.a"
  )

list(APPEND _cmake_import_check_targets hal )
list(APPEND _cmake_import_check_files_for_hal "${_IMPORT_PREFIX}/lib/scr7_l2/libhal.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
