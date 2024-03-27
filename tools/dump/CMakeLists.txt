add_executable(al-dump "main.cpp")
add_executable(al::dump ALIAS al-dump)

set_target_properties(al-dump PROPERTIES OUTPUT_NAME dump)
set_target_properties(al-dump PROPERTIES EXPORT_NAME dump)
target_compile_options(al-dump PUBLIC /nologo)

if (INSTALL_AL)
    install(
        TARGETS
            al-dump
        EXPORT
            al-targets
        RUNTIME DESTINATION
            ${CMAKE_INSTALL_BINDIR}
    )

    install(
        FILES
            $<TARGET_PDB_FILE:al-dump>
        DESTINATION
            ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()
