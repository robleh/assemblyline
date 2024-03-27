# Assembly Line
A CMake based library to compile C++23 to Position Independent
x64 on Windows.

> [!Note]
> This project is in an experimental state and the interface may change
> frequently.

## Introduction
Position Indpendent Code (PIC) is a unit of compiled CPU instructions that can
executed from an arbitrary memory location. It requires compiling an executable
with a code section that has no dependencies on its encapsulating executable.
Once the code section is extracted it is referred to as PIC.

PIC can then be retrieved by a second program and executed as if it were one of
its own functions. Conceptually this is similar to a shared library, however
PIC does not need to be _loaded_ by the OS into a consuming program. Once it is
in executable memory, it can simply be run.

> [!Note]
> PIC and shellcode are semantically similar. The distinction is that
> shellcode is assembled whereas PIC is compiled. However, the products can be
> used interchangeably. While PIC may be more convenient for higher-level
> programmers, it sacrifices the control and smaller byte size of handwritten
> assembly.

## Components

| **CMake Target**     | **Description**                                                           |
|----------------------|---------------------------------------------------------------------------|
| `al::al`             | A static library which contains the essential functions to bootstrap PIC. |
| `AssemblyLine.cmake` | A CMake module which manages PIC options and targets.                     |
| `al::dump`           | A tool to dump PIC and, if needed, patch its entrypoint.                  |
| `al::gtest`          | A header-only extension to GoogleTest that provides a PIC test harness.   |

## Quickstart
The [assemblyline-start](https://github.com/robleh/assemblyline-start) project
provides the boilerplate to get going right away.

Otherwise, there are two ways to incorporate Assembly Line into a CMake project:
1. `FetchContent` which downloads Assembly Line targets as source within a
project.
2. `find_package` which requires building and/or installing the Assembly Line
CMake package beforehand.

> [!TIP]
> Once set up, check out the [Assembly Line tutorial](docs/README.md).

### CMake `FetchContent`
Include Assembly Line into an existing CMake project using the
[`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)
command. If you want to customize the Assembly Line static library alongside
of your project this the best option. `FetchContent` includes Assembly Line
from source and builds its targets within the consuming project.

```CMake
include(FetchContent)

FetchContent_Declare(
    assemblyline
    GIT_REPOSITORY
        "https://github.com/robleh/assemblyline.git"
)
FetchContent_MakeAvailable(assemblyline)
```

### CMake `find_package`
Clone this repository, and run one of the provided CMake workflow presets.
This will build, test, and install Assembly Line.
```console
cmake.exe --workflow --fresh --preset windows-x64-msvc
```

Once installed, it can subsequently be included as a CMake dependency using
[`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html):
```CMake
find_package(assemblyline CONFIG REQUIRED)
```

For more details see the [Install section](#install-).

## Build 🏗️
### Requirements
- CMake version 3.27
- Ninja
- MSVC or Clang toolchain
- MASM for x64
- Windows SDK

### Presets
Assembly Line uses [CMakePresets.json](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
to manage optional build configurations. Currently there are four build
presets:

1. `windows-x64-msvc-release`
2. `windows-x64-msvc-debug`
3. `windows-x64-clang-release`
4. `windows-x64-clang-debug`

The examples in this documentation show MSVC usage but, Clang can used in its
place by using the equivalent preset.

### Options
Assembly Line builds can be customized by setting the cache variables listed
below. `CMakePresets.json` or `CMakeUserPresets.json` is the recommended way
to set them.

| **Variable**                  | **Default**                                                                             | **Description**                                                          |
|-------------------------------|-----------------------------------------------------------------------------------------|--------------------------------------------------------------------------|
| AL_MERGE_DATA                 | ON                                                                                      | Merge .rdata with .text                                                  |
| AL_ALIGN_STACK                | ON                                                                                      | Use x64 stack alignment stub as entrypoint (requires default entrypoint) |
| AL_ENTRYPOINT                 | entry                                                                                   | Specify the entrypoint function name                                     |
| AL_ORDER_FILE                 | C:/repo/src/x64/order.txt                                                               | Absolute path to an order file                                           |
| AL_CXX_FLAGS                  | /nologo /c /GS- /Zl /Oi                                                                 | PIC compiler flags                                                       |
| AL_CXX_FLAGS_DEBUG            | /Zi /Od                                                                                 | PIC compiler debug flags                                                 |
| AL_CXX_FLAGS_RELEASE          | /O1                                                                                     | PIC compiler release flags                                               |
| AL_EXE_LINKER_FLAGS           | /nologo /NODEFAULTLIB /entry:entry /ORDER:@C:/repo/src/x64/order.txt /MERGE:.rdata=.txt | PIC linker flags                                                         |
| AL_EXE_LINKER_FLAGS_DEBUG     | /DEBUG                                                                                  | PIC linker debug flags                                                   |
| AL_EXE_LINKER_FLAGS_RELEASE   | /EMITPOGOPHASEINFO                                                                      | PIC linker release flags                                                 |
| AL_TESTS                      | ON                                                                                      | Build Assembly Line tests                                                |
| AL_EXAMPLES                   | OFF                                                                                     | Build Assembly Line examples                                             |
| INSTALL_AL                    | OFF                                                                                     | Install Assembly Line targets                                            |

### Command Line
```console
cmake.exe --build --preset windows-x64-msvc-release
```

### Visual Studio
- Open the project as a folder.
- Select a build preset
- `Build -> Build All`

### Layout
All build artifacts are located in the top-level `build` directory. Its layout
mirrors the source tree along with an additional directory for the
configuration.

For example, building the Assembly Line static library using
`windows-x64-msvc-release` will output to:
```console
build/windows-x64-msvc/src/Release/al.lib
```

Whereas, building using `windows-x64-msvc-debug` will output to:
```console
build/windows-x64-msvc/src/Debug/al.lib
```

## Install 📦
Once built, Assembly Line can optionally install a CMake package to the
specified [`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html).
This enables other projects on the local system to consume Assembly Line via
[`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html).
It's recommended to build and install both Debug and Release versions of
Assembly Line side-by-side.

> [!Tip]
> Assembly Line's CMake package is relocatable. It can be moved, even to
> another machine, and function properly as long as its on the consumer's
> `find_package` search path.

### Command Line
```console
cmake.exe --install build/windows-x64-msvc --config Debug --prefix C:/Users/dev/packages
cmake.exe --install build/windows-x64-msvc --config Release --prefix C:/Users/dev/packages
```

### Visual Studio
- Open the project as a folder.
- Select a build preset (it does not have to be one named Install)
- `Build -> Install al`

### Usage
Assembly Line will install a versioned package at the given install prefix. To
make use of it, consumers need to set their [`CMAKE_PREFIX_PATH`](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html)
or use an equivalent mechanism.

Given the command line example above Assembly Line would be located at the
the following path:
```console
C:/Users/dev/packages/assemblyline-msvc-x.y.z
```

Consumers would then add the following path to their `CMAKE_PREFIX_PATH` cache
or environment variable:
```console
C:/Users/dev/packages
```

## Test 🧪
Assembly Line uses GTest for both unit and position indpendence tests. For
more information see the [Assembly Line Tests](tests/README.md) documentation.

### Command Line
```console
ctest.exe --preset windows-x64-msvc-release
```

### Visual Studio
- Open the project as a folder.
- Select your build preset in the drop down menu.
- Open the `Test Explorer`
- `Build -> Build All`

Alternatively:
- Select your configuration preset in the drop down menu.
- `Test -> Run Test preset for x64 MSVC Release -> windows-x64-msvc-release`
