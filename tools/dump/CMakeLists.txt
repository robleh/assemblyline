add_executable(al-dump "main.cpp")
add_executable(al::dump ALIAS al-dump)

set_target_properties(
    al-dump
    PROPERTIES
        OUTPUT_NAME dump
        EXPORT_NAME dump
)

if (INSTALL_AL)
    install(
        TARGETS
            al-dump
        EXPORT
            al-targets
        RUNTIME DESTINATION
            ${CMAKE_INSTALL_BINDIR}
    )

    # Don't attempt to install PDB files when using GCC
    if (${CMAKE_CXX_LINKER_SUPPORTS_PDB})
        install(
            FILES
                $<TARGET_PDB_FILE:al-dump>
            DESTINATION
                ${CMAKE_INSTALL_BINDIR}
            OPTIONAL
        )
    endif()
endif()
