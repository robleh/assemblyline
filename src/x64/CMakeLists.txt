add_library(
    al-x64
    OBJECT
        entry.asm
        import.asm
)

target_link_libraries(al-x64 PRIVATE al::assembler)

target_sources(
    al
    PRIVATE
        $<TARGET_OBJECTS:al-x64>
)
