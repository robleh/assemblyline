##############################################################################
# Options
##############################################################################
option(INSTALL_AL "Install Assembly Line targets" OFF)
option(AL_MERGE_RDATA "Merge .rdata with code" ON)
option(AL_MERGE_DATA "Merge .data data with code" OFF)
option(AL_PATCH_ENTRY "Patch start of PIC with jump to entry" OFF)
option(AL_ASM "Use assembler for core Assemblyline functions" ON)
option(AL_C "Enable C support for tests" ON)
option(AL_TESTS "Build Assembly Line unit tests" ON)
option(AL_EXAMPLES "Build Assembly Line examples" OFF)
option(AL_PHNT "Make https://github.com/mrexodia/phnt-single-headers available" OFF)
option(AL_MESSAGEBOX_EXAMPLE "Build Assembly Line messagebox example" ${AL_EXAMPLES})
option(AL_CREATEPROCESS_EXAMPLE "Build Assembly Line createprocess example" ${AL_EXAMPLES})

# Entry function to be specified by the linker for executables.
if(NOT DEFINED AL_ENTRY)
    set(AL_ENTRY entry)
endif()

##############################################################################
# Toolchain Option Interfaces
##############################################################################
add_library(al-compiler INTERFACE)
add_library(al::compiler ALIAS al-compiler)
set_target_properties(al-compiler PROPERTIES EXPORT_NAME compiler)

# Allow position independent object files to be linked with MSVCRT by
# disabling FAILIFMISMATCH directives.
function(al_disable_msvcrt_directives)
    target_compile_definitions(
        al-compiler
        INTERFACE
            _ALLOW_RUNTIME_LIBRARY_MISMATCH
            _ALLOW_MSC_VER_MISMATCH
            _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
            _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
    )
endfunction()

add_library(al-linker INTERFACE)
add_library(al::linker ALIAS al-linker)
set_target_properties(al-linker PROPERTIES EXPORT_NAME linker)

add_library(al-assembler INTERFACE)
add_library(al::assembler ALIAS al-assembler)
set_target_properties(al-assembler PROPERTIES EXPORT_NAME assembler)

if (INSTALL_AL)
    install(
        TARGETS
            al-assembler
            al-compiler
            al-linker
        EXPORT
            al-targets
    )
endif()

##############################################################################
# Compiler Configuration
##############################################################################
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    if (NOT DEFINED AL_CXX_FLAGS)
        set(
            AL_CXX_FLAGS
            /nologo                            # No banner in logs
            /c                                 # Compile without linking
            /GS-                               # Disable buffer security
            /Zl                                # No default lib name in obj
            /Oi                                # Enable intrinsics
            /Zc:preprocessor                   # Enable VA_OPT for macros
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_DEBUG)
        set(
            AL_CXX_FLAGS_DEBUG
            /Zi                                # Generate full PDB
            /Od                                # Disable optimizations
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_RELEASE)
        set(
            AL_CXX_FLAGS_RELEASE
            /O1                                # Optimize for binary size
        )
    endif()

    al_disable_msvcrt_directives()

    target_compile_options(
        al-compiler
        INTERFACE
            ${AL_CXX_FLAGS}
            $<$<CONFIG:Debug>:${AL_CXX_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_CXX_FLAGS_RELEASE}>
    )

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    if (NOT DEFINED AL_CXX_FLAGS)
        set(
            AL_CXX_FLAGS
            -c
            --target=amd64-pc-windows-msvc
            -fdiagnostics-absolute-paths
            -fno-stack-protector
            -nodefaultlibs
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_DEBUG)
        set(
            AL_CXX_FLAGS_DEBUG
            -g
            -O0
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_RELEASE)
        set(
            AL_CXX_FLAGS_RELEASE
            -Oz
        )
    endif()

    al_disable_msvcrt_directives()

    target_compile_options(
        al-compiler
        INTERFACE
            ${AL_CXX_FLAGS}
            $<$<CONFIG:Debug>:${AL_CXX_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_CXX_FLAGS_RELEASE}>
    )

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    if (NOT DEFINED AL_CXX_FLAGS)
        set(
            AL_CXX_FLAGS
            -mconsole
            -nostdlib
            -fno-exceptions
            -fno-asynchronous-unwind-tables
            -ffunction-sections
            -fno-ident
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_DEBUG)
        set(
            AL_CXX_FLAGS_DEBUG
            -g
            -O0
        )
    endif()

    if (NOT DEFINED AL_CXX_FLAGS_RELEASE)
        set(
            AL_CXX_FLAGS_RELEASE
            -O2
        )
    endif()

    al_disable_msvcrt_directives()

    target_compile_options(
        al-compiler
        INTERFACE
            ${AL_CXX_FLAGS}
            $<$<CONFIG:Debug>:${AL_CXX_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_CXX_FLAGS_RELEASE}>
    )

else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} compiler not supported")

endif()

##############################################################################
# Linker Configuration
##############################################################################
if (${CMAKE_CXX_COMPILER_LINKER_ID} STREQUAL MSVC)
    if(NOT DEFINED AL_MERGE)
        set(AL_MERGE /merge:.al=.text /merge:.text=.pic)
    endif()

    if(AL_MERGE_RDATA)
        set(AL_MERGE /merge:.rdata=.al ${AL_MERGE})
        message(STATUS "Assembly Line: Linker will merge .rdata with code")
    endif()

    if(AL_MERGE_DATA)
        set(AL_MERGE /merge:.data=.al ${AL_MERGE})
        message(STATUS "Assembly Line: Linker will merge .data with code")
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS)
        set(
            AL_EXE_LINKER_FLAGS
            /machine:x64
            /nologo
            /nodefaultlib
            /map
            /manifest:no
            /emittoolversioninfo:no
            ${AL_MERGE}
        )
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_DEBUG)
        set(
            AL_CXX_FLAGS_DEBUG
            /debug
            /opt:ref
        )
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_RELEASE)
        set(
            AL_EXE_LINKER_FLAGS_RELEASE
            /emitpogophaseinfo
            /merge:.xdata=.xxxdata
        )
    endif()

    target_link_options(
        al-linker
        INTERFACE
            $<$<STREQUAL:EXECUTABLE,$<TARGET_PROPERTY:TYPE>>:/entry:${AL_ENTRY}>
            $<$<STREQUAL:SHARED_LIBRARY,$<TARGET_PROPERTY:TYPE>>:/noentry>
            ${AL_EXE_LINKER_FLAGS}
            $<$<CONFIG:Debug>:${AL_EXE_LINKER_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_EXE_LINKER_FLAGS_RELEASE}>
    )

elseif (${CMAKE_CXX_COMPILER_LINKER_ID} STREQUAL LLD)
    if(NOT DEFINED AL_MERGE)
        set(AL_MERGE /merge:.al=.text /merge:.text=.pic)
    endif()

    if(AL_MERGE_RDATA)
        set(AL_MERGE /merge:.rdata=.al ${AL_MERGE})
        message(STATUS "Assembly Line: Linker will merge .rdata with code")
    endif()

    if(AL_MERGE_DATA)
        set(AL_MERGE /merge:.data=.al ${AL_MERGE})
        message(STATUS "Assembly Line: Linker will merge .data with code")
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS)
        set(
            AL_EXE_LINKER_FLAGS
            /machine:x64
            /nologo
            /nodefaultlib
            /map
            /manifest:no
            ${AL_MERGE}
        )
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_DEBUG)
        set(
            AL_EXE_LINKER_FLAGS_DEBUG
            /debug
            /opt:ref
        )
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_RELEASE)
        set(
            AL_EXE_LINKER_FLAGS_RELEASE
            /emitpogophaseinfo
            /merge:.xdata=.xxxdata
        )
    endif()

    # CMake passes lld-link options via clang.exe using the -Xlinker flag. In
    # order to properly prefix our parameters the LINKER: modifier must be
    # specified. This modifier expects comma separated items.
    target_link_options(
        al-linker
        INTERFACE
            $<$<STREQUAL:EXECUTABLE,$<TARGET_PROPERTY:TYPE>>:LINKER:/entry:${AL_ENTRY}>
            $<$<STREQUAL:SHARED_LIBRARY,$<TARGET_PROPERTY:TYPE>>:LINKER:/noentry>
            LINKER:$<JOIN:${AL_EXE_LINKER_FLAGS},,>
            $<$<CONFIG:Debug>:LINKER:$<JOIN:${AL_EXE_LINKER_FLAGS_DEBUG},,>>
            $<$<CONFIG:Release>:LINKER:$<JOIN:${AL_EXE_LINKER_FLAGS_RELEASE},,>>
    )

elseif (${CMAKE_CXX_COMPILER_LINKER_ID} STREQUAL GNU)
    if (NOT DEFINED AL_EXE_LINKER_FLAGS)
        set(
            AL_EXE_LINKER_FLAGS
            -M
            --entry=${AL_ENTRY}
            -T${AL_TOOLCHAIN_DIR}/$<CONFIG>/script.ld
            --no-seh
        )
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_DEBUG)
        set(AL_EXE_LINKER_FLAGS_DEBUG)
    endif()

    if (NOT DEFINED AL_EXE_LINKER_FLAGS_RELEASE)
        set(
            AL_EXE_LINKER_FLAGS_RELEASE
            -s
        )
    endif()

    target_link_options(
        al-linker
        BEFORE
        INTERFACE
            -nostdlib
            $<$<STREQUAL:EXECUTABLE,$<TARGET_PROPERTY:TYPE>>:--entry=${AL_ENTRY}>
            $<$<STREQUAL:SHARED_LIBRARY,$<TARGET_PROPERTY:TYPE>>:--no-entry>
            ${AL_EXE_LINKER_FLAGS}
            $<$<CONFIG:Debug>:${AL_EXE_LINKER_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_EXE_LINKER_FLAGS_RELEASE}>
    )

    file(
        GENERATE
        OUTPUT
            ${AL_TOOLCHAIN_DIR}/$<CONFIG>/script.ld
        CONTENT
"SECTIONS\n
{\n
  . = SIZEOF_HEADERS;\n
  . = ALIGN(__section_alignment__);\n
  .pic  __image_base__ + ( __section_alignment__ < 0x1000 ? . : __section_alignment__ ) :\n
  {\n
    *(.al$*)\n
    *(.rdata)\n
    *(SORT(.rdata$*))\n
    *(.data)\n
    *(SORT(.data$*))\n
    *(.text)\n
    *(SORT(.text$*))\n
  }\n
}"
)

else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} linker not supported")

endif()

##############################################################################
# Assembler Configuration
##############################################################################
# ASM_MASM is not always set so we need to check if it's defined to avoid
# issues.
if (DEFINED ${CMAKE_ASM_MASM_COMPILER_ID} AND ${CMAKE_ASM_MASM_COMPILER_ID} STREQUAL MSVC)
    if (NOT DEFINED AL_ASM_MASM_FLAGS)
        set(
            AL_ASM_MASM_FLAGS
            /nologo
            /Gy
        )
    endif()

    if (NOT DEFINED AL_ASM_MASM_FLAGS_DEBUG)
        set(
            AL_ASM_MASM_FLAGS_DEBUG
        )
    endif()

    if (NOT DEFINED AL_ASM_MASM_FLAGS_RELEASE)
        set(
            AL_ASM_MASM_FLAGS_RELEASE
        )
    endif()

    target_compile_options(
        al-assembler
        INTERFACE
            ${AL_ASM_MASM_LINKER_FLAGS}
            $<$<CONFIG:Debug>:${AL_ASM_MASM_LINKER_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_ASM_MASM_LINKER_FLAGS_RELEASE}>
    )

# Off brand MASM compatible compilers don't have compiler ID defined.
elseif (NOT DEFINED ${CMAKE_ASM_MASM_COMPILER_ID})
    if(${CMAKE_ASM_MASM_COMPILER} MATCHES ".*uasm.*")
        if (NOT DEFINED AL_ASM_MASM_FLAGS)
            set(
                AL_ASM_MASM_FLAGS
                -win64
            )
        endif()

        if (NOT DEFINED AL_ASM_MASM_FLAGS_DEBUG)
            set(
                AL_ASM_MASM_FLAGS_DEBUG
            )
        endif()

        if (NOT DEFINED AL_ASM_MASM_FLAGS_RELEASE)
            set(
                AL_ASM_MASM_LINKER_FLAGS_RELEASE
            )
        endif()
    endif()

    target_compile_options(
        al-assembler
        INTERFACE
            ${AL_ASM_MASM_LINKER_FLAGS}
            $<$<CONFIG:Debug>:${AL_ASM_MASM_LINKER_FLAGS_DEBUG}>
            $<$<CONFIG:Release>:${AL_ASM_MASM_LINKER_FLAGS_RELEASE}>
    )

else()
    message(FATAL_ERROR "${CMAKE_ASM_MASM_COMPILER} assembler not supported")

endif()

##############################################################################
# Functions and Macros
##############################################################################
function(al_dump_target name)
    get_target_property(target_type ${name} TYPE)

    # Only dump executable types
    if(NOT target_type STREQUAL "STATIC_LIBRARY")
        add_custom_command(
            TARGET
                ${name}
            POST_BUILD
            COMMAND
                al::dump "$<TARGET_FILE:${name}>" $<$<BOOL:${AL_PATCH_ENTRY}>:patch_entry>
            WORKING_DIRECTORY
                "$<TARGET_FILE_DIR:${name}>"
            COMMENT
                "Assembly Line: Dumping PIC section from $<TARGET_FILE:${name}>"
        )
    endif()
endfunction()

function(al_configure_target name)
    set_target_properties(${name} PROPERTIES EXPORT_COMPILE_COMMANDS ON)
    target_link_libraries(${name} PRIVATE al::al al::compiler)

    get_target_property(target_type ${name} TYPE)
    if(NOT target_type STREQUAL "STATIC_LIBRARY")
        target_link_libraries(${name} PRIVATE al::intrinsic al::linker)
    endif()

    target_compile_definitions(
        ${name}
        PRIVATE
            AL_PRNG_SEED=0x${AL_PRNG_SEED}
    )

    al_dump_target(${name})
endfunction()

function(_al_add_library name)
    add_library(${name} ${ARGN})
    al_configure_target(${name})
endfunction()

function(_al_add_executable name)
    add_executable(${name} ${ARGN})
    al_configure_target(${name})
endfunction()

function(add_pic_test name)
    if(AL_TESTS)
        # Need target to be built but does not require relinking
        # Currently each time the PIC executable is built al-test needs to rebuild
        add_dependencies(al-test ${name})

        set_property(
            SOURCE
                ${ARGN}
                TARGET_DIRECTORY
                    al-test
            APPEND
            PROPERTY
                COMPILE_DEFINITIONS AL_EXE_PATH="$<TARGET_FILE:${name}>"
                COMPILE_DEFINITIONS AL_PIC_PATH="$<TARGET_FILE:${name}>.pic"
        )

        target_sources(al-test PRIVATE ${ARGN})
    endif()
endfunction()

# We need to mask default language flags for the caller. Could not get this to
# work in a function or by setting properties directly. Otherwise, langauge
# flags need to be disabled globally and non-pic targets need to have them set
# again via an interface library.
macro(al_mask_flags)
    # Clear default compiler and linker flags that conflict with position
    # independence. Should only affect the current CMakeLists.txt scope.
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
    set(CMAKE_CXX_STANDARD_LIBRARIES "")
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_C_FLAGS_MINSIZEREL "")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "")
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "")
    set(CMAKE_C_STANDARD_LIBRARIES "")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "")

    # Trust me CMake, this works.
    set(CMAKE_CXX_COMPILER_WORKS true)
    set(CMAKE_C_COMPILER_WORKS true)
endmacro()

# Provide a new PRNG seed each time assemblyline is configured
macro(al_seed_prng)
    string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" AL_PRNG_SEED)
    message(DEBUG "Assembly Line: PRNG seed 0x${AL_PRNG_SEED}")
endmacro()

macro(al_init)
    al_mask_flags()
    al_seed_prng()
endmacro()

macro(add_pic_library)
    al_init()
    _al_add_library(${ARGV})
endmacro()

macro(add_pic_executable)
    al_init()
    _al_add_executable(${ARGV})
endmacro()

##############################################################################
# External
##############################################################################
if (AL_PHNT)
    set(phnt_TAG "v1.4-ed73b907")
    message(STATUS "Fetching phnt (${phnt_TAG})...")
    FetchContent_Declare(phnt
        URL "https://github.com/mrexodia/phnt-single-header/releases/download/${phnt_TAG}/phnt.zip"
        URL_HASH "SHA256=a41def8d91204dc8c1d322a9d20b5fa107f99138ed0ad8bf52d6353137000dd5"
    )
    FetchContent_MakeAvailable(phnt)

    install(
        FILES
            ${phnt_SOURCE_DIR}/phnt.h
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
