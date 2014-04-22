# ----------------------------------------------------------------------------
# Target: wflinfo (executable)
# ----------------------------------------------------------------------------

add_executable(wflinfo wflinfo.c)
target_link_libraries(wflinfo ${waffle_libname})
set_target_properties(wflinfo
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

if(waffle_on_mac)
    set_target_properties(wflinfo
        PROPERTIES
        COMPILE_FLAGS "-ObjC"
        )
endif()

install(TARGETS wflinfo DESTINATION bin)

# ----------------------------------------------------------------------------
# Target: wfl-swapinterval
# ----------------------------------------------------------------------------

# TODO: Not enough checks are done here.

add_executable(wfl-swapinterval wfl-swapinterval.c)
target_link_libraries(wfl-swapinterval
    ${waffle_libname}
    EGL
    GLESv2
    )
set_target_properties(wfl-swapinterval
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

install(TARGETS wfl-swapinterval DESTINATION bin)
