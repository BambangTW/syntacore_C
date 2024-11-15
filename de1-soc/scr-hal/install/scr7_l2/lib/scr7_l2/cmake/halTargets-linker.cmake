if(NOT HAL_SKIP_LD_SCRIPT)
    target_link_options(hal INTERFACE -T ${_IMPORT_PREFIX}/platform/scr7_l2/mem.ld)
endif()
