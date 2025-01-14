##############################################################################
# Options
##############################################################################
option(AL_ALIGN_STACK "Use x64 alignment stub as entrypoint (requires default entrypoint)" ON)
option(AL_MERGE_RDATA "Merge .rdata with .text" ON)
option(AL_TESTS "Build Assembly Line unit tests" ON)
option(AL_EXAMPLES "Build Assembly Line examples" OFF)
option(INSTALL_AL "Install Assembly Line targets" OFF)

if(AL_ALIGN_STACK)
    if(DEFINED AL_ENTRYPOINT)
        message(FATAL_ERROR "Assembly Line: AL_ALIGN_STACK is incompatible with AL_ENTRYPOINT")
    endif()

    set(AL_ENTRYPOINT align)

    set(
        AL_ORDER_FILE 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/x64/stack-align.txt>$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_SYSCONFDIR}/stack-align.txt>
    )

    message(STATUS "Assembly Line: x64 stack alignment stub will be used as entrypoint")
    message(VERBOSE "Assembly Line: AL_ENTRYPOINT = ${AL_ENTRYPOINT}")
    message(VERBOSE "Assembly Line: AL_ORDER_FILE = ${AL_ORDER_FILE}")
endif()

if(AL_MERGE_RDATA)
    # Prepend .jmp to .rdata when using MSVC
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(AL_MERGE_SECTIONS /MERGE:.jmp=.rdata /MERGE:.rdata=.text)
    else()
        set(AL_MERGE_SECTIONS /MERGE:.rdata=.text)
    endif()

    message(STATUS "Assembly Line: Linker will merge .rdata with .text")
    message(VERBOSE "Assembly Line: AL_MERGE_SECTIONS = ${AL_MERGE_SECTIONS}")
endif()

##############################################################################
# Compiler Defaults
##############################################################################
if(NOT DEFINED AL_CXX_FLAGS)
    set(AL_CXX_FLAGS /nologo /c /GS- /Zl /Oi)
endif()

if(NOT DEFINED AL_CXX_FLAGS_DEBUG)
    set(AL_CXX_FLAGS_DEBUG /Zi /Od)
endif()

if(NOT DEFINED AL_CXX_FLAGS_RELEASE)
    set(AL_CXX_FLAGS_RELEASE /O1)
endif()

##############################################################################
# Linker Defaults
##############################################################################
if(NOT DEFINED AL_ENTRYPOINT)
    set(AL_ENTRYPOINT entry)
endif()

if(NOT DEFINED AL_ORDER_FILE)
    set(AL_ORDER)
else()
    set(AL_ORDER /ORDER:@${AL_ORDER_FILE})
endif()

if(NOT DEFINED AL_MERGE_SECTIONS)
    set(AL_MERGE_SECTIONS)
endif()

if(NOT DEFINED AL_EXE_LINKER_FLAGS)
    set(AL_EXE_LINKER_FLAGS /nologo /NODEFAULTLIB /entry:${AL_ENTRYPOINT} ${AL_ORDER} ${AL_MERGE_SECTIONS})
endif()

if(NOT DEFINED AL_EXE_LINKER_FLAGS_DEBUG)
    set(AL_EXE_LINKER_FLAGS_DEBUG /DEBUG)
endif()

if(NOT DEFINED AL_EXE_LINKER_FLAGS_RELEASE)
    set(AL_EXE_LINKER_FLAGS_RELEASE /EMITPOGOPHASEINFO)
endif()

message(VERBOSE "Assembly Line: AL_CXX_FLAGS = ${AL_CXX_FLAGS}")
message(VERBOSE "Assembly Line: AL_EXE_LINKER_FLAGS = ${AL_EXE_LINKER_FLAGS}")
message(VERBOSE "Assembly Line: AL_CXX_FLAGS_DEBUG = ${AL_CXX_FLAGS_DEBUG}")
message(VERBOSE "Assembly Line: AL_EXE_LINKER_FLAGS_DEBUG = ${AL_EXE_LINKER_FLAGS_DEBUG}")
message(VERBOSE "Assembly Line: AL_CXX_FLAGS_RELEASE = ${AL_CXX_FLAGS_RELEASE}")
message(VERBOSE "Assembly Line: AL_EXE_LINKER_FLAGS_RELEASE = ${AL_EXE_LINKER_FLAGS_RELEASE}")

# should these even be here? they are becoming part of the installed interface
if (INSTALL_AL)
    install(
        FILES
            ${AL_ORDER_FILE}
        DESTINATION
            ${CMAKE_INSTALL_SYSCONFDIR}
        OPTIONAL
    )

    install(
        FILES
            ${PROJECT_SOURCE_DIR}/src/x64/stack-align.txt
        DESTINATION
            ${CMAKE_INSTALL_SYSCONFDIR}
    )
endif()

##############################################################################
# Interface Targets
##############################################################################
# What happens to these if they are part of the installation interface? Do
# they get overriden?
add_library(al-compiler INTERFACE)
add_library(al::compiler ALIAS al-compiler)
set_target_properties(al-compiler PROPERTIES EXPORT_NAME compiler)

target_compile_options(
    al-compiler
    INTERFACE
        $<$<CONFIG:Debug>:${AL_CXX_FLAGS} ${AL_CXX_FLAGS_DEBUG}>
        $<$<CONFIG:Release>:${AL_CXX_FLAGS} ${AL_CXX_FLAGS_RELEASE}>
)

add_library(al-linker INTERFACE)
add_library(al::linker ALIAS al-linker)
set_target_properties(al-linker PROPERTIES EXPORT_NAME linker)

target_link_options(
    al-linker
    INTERFACE
        $<$<CONFIG:Debug>:${AL_EXE_LINKER_FLAGS} ${AL_EXE_LINKER_FLAGS_DEBUG}>
        $<$<CONFIG:Release>:${AL_EXE_LINKER_FLAGS} ${AL_EXE_LINKER_FLAGS_RELEASE}>
)

if (INSTALL_AL)
    install(
        TARGETS
            al-compiler
            al-linker
        EXPORT
            al-targets
    )
endif()

##############################################################################
# Functions and Macros
##############################################################################
function(_add_pic name)
    cmake_parse_arguments(PARSE_ARGV 1 PIC "" "PUBLIC_HEADER" "LIBRARIES")

    # Use the provided PUBLIC_HEADER if given, but fallback to ${name}.hpp by
    # default. If no public header is given warn the user.
    if(NOT DEFINED PIC_PUBLIC_HEADER)
        list(FIND PIC_UNPARSED_ARGUMENTS ${name}.hpp PUBLIC_HEADER_INDEX)

        if (PUBLIC_HEADER_INDEX EQUAL -1)
            message(WARNING "Assembly Line: PIC target ${name} has no public header")
        else()
            list(GET PIC_UNPARSED_ARGUMENTS ${PUBLIC_HEADER_INDEX} PIC_PUBLIC_HEADER)
            list(REMOVE_AT PIC_UNPARSED_ARGUMENTS ${PUBLIC_HEADER_INDEX})
        endif()
    endif()

    # If no LIBRARIES provided use al::lib.
    if(NOT DEFINED PIC_LIBRARIES)
        set(PIC_LIBRARIES al::al)
    endif()

    # Format lists for inline use
    list(JOIN PIC_UNPARSED_ARGUMENTS " " PIC_SOURCES)
    list(JOIN PIC_LIBRARIES " " PIC_LIBRARIES)

    message(DEBUG "Assembly Line: ${name} PIC source files are ${PIC_SOURCES}")
    message(DEBUG "Assembly Line: ${name} PIC header is ${PIC_PUBLIC_HEADER}")
    message(DEBUG "Assembly Line: ${name} PIC libraries are ${PIC_LIBRARIES}")

    # Name the container PE to name.exe not name-pe.exe
    add_executable(${name}-pe)
    set_target_properties(${name}-pe PROPERTIES OUTPUT_NAME ${name})
    set_target_properties(${name}-pe PROPERTIES EXPORT_COMPILE_COMMANDS ON)

    target_sources(
        ${name}-pe
        PRIVATE
            ${PIC_UNPARSED_ARGUMENTS}
        PUBLIC
            FILE_SET
                ${name}_header
            TYPE
                HEADERS
            FILES
                ${PIC_PUBLIC_HEADER}
    )
    
    # Static libraries must be position independent
    target_link_libraries(${name}-pe PRIVATE al::linker ${PIC_LIBRARIES})

    target_compile_definitions(${name}-pe PUBLIC AL_PRNG_SEED=0x${AL_PRNG_SEED})

    # Target-dependent expressions are not permitted in the BYPRODUCT field
    # of add_custom_command.
    set(PIC_PATH "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${name}.bin")
    message(DEBUG "Assembly Line: ${name} PIC_PATH = ${PIC_PATH}")

    # Dump the .text section
    add_custom_command(
        TARGET
            ${name}-pe
        POST_BUILD
        COMMAND
            al::dump "$<TARGET_FILE:${name}-pe>" $<$<CXX_COMPILER_ID:MSVC>:patch_entry>
        BYPRODUCTS
            "${PIC_PATH}"
        WORKING_DIRECTORY
            "$<TARGET_FILE_DIR:${name}-pe>"
        COMMENT
            "Assembly Line: Dumping .text section of $<TARGET_FILE:${name}-pe> $<$<CXX_COMPILER_ID:MSVC>:and patching entrypoint>"
    )

    add_library(${name} INTERFACE)

    target_sources(
        ${name}
        PRIVATE
            "${PIC_PATH}"
        INTERFACE
            FILE_SET
                ${name}_header
            TYPE
                HEADERS
    )

    string(TOUPPER ${name} PREFIX)

    target_compile_definitions(
        ${name}
        INTERFACE
            ${PREFIX}_PIC_PATH="${PIC_PATH}"
    )
endfunction()

function(_add_pic_library)
#unimplemented
endfunction()

# We need to mask default language flags for the caller. Could not get this to
# work in a function or by setting properties directly. Otherwise, langauge
# flags need to be disabled globally and non-pic targets need to have them set
# again via an interface library.
macro(mask_default_flags)
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
    set(CMAKE_C_STANDARD_LIBRARIES "")
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "")

    # Trust me CMake, this works.
    set(CMAKE_CXX_COMPILER_WORKS true)
    set(CMAKE_C_COMPILER_WORKS true)
endmacro()

# Provide a new PRNG seed each time assemblyline is configured
macro(seed_prng)
    string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" AL_PRNG_SEED)
    message(DEBUG "Assembly Line: PRNG seed 0x${AL_PRNG_SEED}")
endmacro()

macro(add_pic)
    mask_default_flags()
    seed_prng()
    _add_pic(${ARGV})
endmacro()

