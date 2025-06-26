##############################################################################
# Assembly Line
##############################################################################
cmake_minimum_required(VERSION 3.28)

project(
    al
    VERSION
        0.0.1
    LANGUAGES
        CXX ASM_MASM
    DESCRIPTION
        "Assembly Line: C++ to Windows Position Independent Code"
    HOMEPAGE_URL
        "https://github.com/robleh/assemblyline"
)

##############################################################################
# External
##############################################################################
include(FetchContent)

# mrexodia/phnt-single-header
set(phnt_TAG "v1.4-ed73b907")
message(STATUS "Assembly Line: Fetching phnt (${phnt_TAG})...")

FetchContent_Declare(
    phnt
    URL "https://github.com/mrexodia/phnt-single-header/releases/download/${phnt_TAG}/phnt.zip"
    URL_HASH "SHA256=a41def8d91204dc8c1d322a9d20b5fa107f99138ed0ad8bf52d6353137000dd5"
)
FetchContent_MakeAvailable(phnt)

##############################################################################
# Source
##############################################################################
include(GNUInstallDirs)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} PROJECT_COMPILER)
set(AL_PACKAGE_DIR assemblyline-${PROJECT_COMPILER}-${PROJECT_VERSION})

set(CMAKE_INSTALL_BINDIR ${AL_PACKAGE_DIR}/${CMAKE_INSTALL_BINDIR}/$<CONFIG>)
set(CMAKE_INSTALL_SYSCONFDIR ${AL_PACKAGE_DIR}/${CMAKE_INSTALL_SYSCONFDIR})
set(CMAKE_INSTALL_LIBDIR ${AL_PACKAGE_DIR}/${CMAKE_INSTALL_LIBDIR}/$<CONFIG>)
set(CMAKE_INSTALL_INCLUDEDIR ${AL_PACKAGE_DIR}/${CMAKE_INSTALL_INCLUDEDIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(AssemblyLine)

if (AL_TESTS)
    # This allows tests in sub-directories to be registered
    include(CTest)
    add_subdirectory("tests")
endif()

add_subdirectory("tools")
add_subdirectory("src")

if (AL_EXAMPLES AND PROJECT_IS_TOP_LEVEL)
    add_subdirectory("examples")
endif()

##############################################################################
# Installation
##############################################################################
if(PROJECT_IS_TOP_LEVEL)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/assemblyline-config.cmake.in
        ${PROJECT_BINARY_DIR}/assemblyline-config.cmake
        INSTALL_DESTINATION
            ${AL_PACKAGE_DIR}/cmake
    )

    write_basic_package_version_file(
        assemblyline-config-version.cmake
        VERSION
            ${PROJECT_VERSION}
        COMPATIBILITY
            SameMajorVersion
    )

    install(
        FILES
            ${PROJECT_BINARY_DIR}/assemblyline-config.cmake
            ${PROJECT_BINARY_DIR}/assemblyline-config-version.cmake
            ${PROJECT_SOURCE_DIR}/cmake/AssemblyLine.cmake
        DESTINATION
            ${AL_PACKAGE_DIR}/cmake
    )

    install(
        EXPORT
            al-targets
        NAMESPACE
            al::
        DESTINATION
            ${AL_PACKAGE_DIR}/cmake
    )
endif()
