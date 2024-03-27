# Tutorial
## PIC Crash Course
### Terminology
- Position Independent Code (PIC): Compiled CPU instructions that can be
executed independent of their position in memory.
- Position Independent PE (PIC PE): A [PE](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format)
containing a position independent `.text` section. This intermediate format is
a temporary container that also enables easier debugging.
- PIC Library: A static library the contains PIC that can be linked into PIC
PEs.

### Coding Position Independently
Compiler and linker settings can only take one so far. Ultimately, it is the
responsibility of the programmer to write code that is position independent.
This can be accomplished by following a single rule:

> PIC source cannot use language features that rely on PE sections other than
`.text`.

However, the implications of that rule are far reaching:
1. No linking to DLL import libraries (`.idata`)
2. No readonly strings (`.rdata`)
3. No global variables (`.data`/`.bss`)
4. No VTables (`.data`)
5. No implicit TLS (`.tls`)
6. No exceptions (`.pdata`)
7. No linking to static libraries which violate the previous rules
8. etc.

Furthermore, these contraints prevent the use of the C runtime. C++ relies on
a C runtime for `new` and `delete` implementations. Their absence prevents PIC
source from using _dynamic_ standard library types such as `std::vector` and
`std::string`.

The C runtime also provides the compiler with some intrinsic functions.
For example, the `memcpy` intrinsic may be required by some standard library
types. So, in order to use them we need to provide our own implementation.

It may be impossible to create an exhaustive list of _dont's_ to abide by.
Nevertheless, violating position independence will likely result in an access
violation. As such, Assembly Line takes the approach of asking for forgiveness
not permission. Use the testing framework to iterate quickly and test for
crashes.

### So what does work?
Again, it may be impossible to create an exhaustive list. However, the
following C++ features, keywords, and types work well:

- Classes
- Constructors
- Destructors
- Templates
- Compile-time evaluation (`consteval` and `constexpr`) 
- Concepts
- Operator overloads
- User defined conversions
- User defined literals
- Iterators
- `decltype`
- `auto`
- `<string_view>`
- `<span>`
- `<ranges>`
- `<algorithm>`
- `<expected>` (useful to replace exceptions)
- etc.

Additionally, there's no limitation on source code layout. Code can still be
split over multiple translation units (source and header files) if desired.

## Assembly Line
[Once incorporated into the build system](../README.md#quickstart), Assembly
Line can be included like so:

```C++
#include <al/al.hpp>
```

> [!Note]
> Assembly Line includes phnt-single-header as a replacement for `Windows.h`
> which exposes Native API types.

### Entrypoint
Since the C runtime (CRT) is position dependent it cannot be linked to.
Without the CRT, the typical entrypoint functions, `main`, `wmain`, `WinMain`,
or `wWinMain`, are not called. As such, the program's entrypoint function must
be specified to the linker.

Assembly Line expects PIC to provide a function named `entry` to serve as the
entrypoint. However, this can be overriden by the CMake cache variable
`AL_ENTRYPOINT`.

> [!Note]
> x64 has some instructions that require the stack to be aligned to a 32 bit
> boundary. For cases when the alignment of PIC is not controlled, Assembly
> Line provides the `align` entrypoint which aligns the stack prior to calling
> `entry`. To use it set the CMake cache variable `AL_ALIGN_STACK` to `ON`.

For now the entrypoint has some requirements.
- It must be `extern "C"` or you need to know the mangled C++ name i guess
- limited number of parameters when using `align_stack`
- think about the caller vs compiler

### Order File
Under normal circumstances, a compiled PE documents the offset to its
entrypoint in its Optional header. Once a PE is loaded, the OS simply passes
execution to this offset. However when executing the raw PIC from a PE's
`.text` section, execution must be passed to the entrypoint manually. This
can be achieved by placing it at the beginning of the `.text` section or by
jumping to it.

For simple scenarios the linker can be instructed to order functions using an
order file. This ensures that our desired entrypoint is placed before any
other code. An order file is a simple newline separated text file that
contains functions by name in the desired order. Assembly Line can be directed
to use a custom order file by specifing the absolute path to one in the CMake
cache variable `AL_ORDER_FILE`.

> [!Note]
> Currently a custom `AL_ORDER_FILE` value cannot be set when `AL_ALIGN_STACK`
> is enabled.

### Strings
Read-only strings are generally compiled into the `.rdata` section of a PE.
Not only will `.rdata` not be available alongside of `.text` once we have
dumped it, but compiled instructions in `.text` reference data across section
boundaries by absolute address. This violates position independence and will
lead to undefined behavior (likely an access violation).

Without using assembly stubs, this is typically solved in one of three ways:
1. Stack strings
2. Hashes
3. Merging `.rdata` with `.text`

### Stack Strings
By coercing the compiler to assemble a string from immediate values rather than
storing it in `.rdata`. The syntax usually looks something like this:

```C++
wchar_t kernel32[]{ 'K', 'E', 'R', 'N', 'E', 'L', '3', '2', '.', 'D', 'L', 'L', 0 };
```

Assembly Line provides the `_stack` User Defined Literal suffix to accomplish
the same thing:
```C++
// kernel32 is of type al::buffer but implictly converts to a wchar_t* or
// std::wstring_view.
auto kernel32 = L"KERNEL32.DLL"_stack;

// Also usable inline
HMODULE kernel32_handle = GetModuleHandleW(L"kernel32.dll"_stack);
```

> [!Caution]
> When compiler optimizations are enabled on x64, stack strings cannot exceed
> 16 bytes in length.

### Hashes
Alternatively, for strings used in comparisons, hashes can be used instead. For
example to search for the exporrted Windows API function `GetProcAddress`, the
ROR13 hash of the string `GetProcAddress` can be computed in advance and used
as the search criteria:

```C++
#define GETPROCADDRESS_HASH 0x7802f749
```

Assembly Line provides compile-time functions to pre-compute hashes:

```C++
auto gpa_ror13 = al::ror13(L"GetProcAddress");

// or
gpa_ror13 = L"GetProcAddress"_ror13;
```

### Merging `.text` with `.rdata`
On x64, merging read-only data into our PIC is another option. By directing
the linker to merge `.rdata` with `.text` we can reintroduce the use of
read-only strings into our PIC. This can be enabled by setting the CMake cache
variable `AL_MERGE_RDATA` to `ON`.

> [!Important]
> This technique works out of the box with Clang because the linker appends
> `.rdata` to `.text`, keeping the entrypoint intact. However, MSVC
> _prepends_ `.rdata` to `.text` which means the designated entrypoint function
> is no longer the beginning of our PIC. To fix this Assembly Line patches the
> beginning of the combined sections with a `jmp` instruction to the
> entrypoint.

### XOR'd Strings
A side effect of merging `.rdata` into `.text` is that the PIC now contains
plaintext strings. Assembly Line includes a User Defined Literal suffix to XOR
encrypt strings at compile-time. XOR'd strings use a random key generated by
compile-time PRNG that is seeded by CMake.

```C++
auto kernel32_xor = L"KERNEL32.DLL"_xor

// Also usable inline
HMODULE kernel32_handle = GetModuleHandleW(L"kernel32.dll"_xor);
```

> [!Note]
> XOR'd strings are decrypted on first use and subsequently wiped when they go
> out of scope. For temporary values that means they are wiped after the
> expression in which they are evaluated.

### Function Resolution
The included `GM` (Get Module) and `GP` (Get Proc) macros are the simplest way
to resolve functions at run-time. Both macros accept a search function to
customize their behavior.

Get the base address of `Kernel32` by searching for its name:
```C+
HMODULE kernel32 = GM(L"KERNEL32.DLL"_stack, al::by_name);
```

Resolve `GetProcAddress` from `Kernel32` by searching for its name:
```C+
auto gpa = GP(kernel32, GetProcAddress, al::by_name);
```

> [!Tip]
> `GP`'s second parameter is a symbol not a string. It will cast the return
> value to the symbols's type. This means no function typedef is needed and
> the return value can be used immediately without a cast.

The search function can easily be exchanged to search by ROR13. For example:
```C+
HMODULE kernel32 = GM(L"KERNEL32.DLL"_stack, al::by_ror13);
auto gpa = GP(kernel32, GetProcAddress, al::by_ror13);
```

> [!Note]
> Alternatively, `al::get_module_handle` and `al::get_proc_address` are also
> provided which serve as drop in replacements for the Windows API functions
> `GetModuleHandleW` and `GetProcAddress` respectively.

### CMake
Assembly Line provides several functions to facilitate PIC builds.

> [!CAUTION]
> These functions disable some C and C++ default options for the current CMake
> directory. As such, it is recommended to keep PIC targets in their own
> `CMakeLists.txt` scope/directory.

#### `add_pic`
The CMake `add_pic` function instructs CMake to build a PIC PE and dump its
`.text` section.

```CMake
add_pic(messagebox entry.cpp messagebox.hpp)
```

`entry.cpp` contains a custom entrypoint that replaces what would typically
be our `main` function.

`add_pic` has the following optional parameters:

- `PUBLIC_HEADER` which defines the PIC interface for use in external programs.

- `LIBRARIES` which are the PIC libraries that we are linking against.

In the above example, `messagebox` is the target name and `entry.cpp` and
`messagebox.hpp` are its source files.

By default `add_pic` will use any .hpp file named after the target as the
public header. In this example, `messagebox.hpp`. This behavior can be
overridden by specifying the `PUBLIC_HEADER` option.

```CMake
add_pic(
    messagebox
    entry.cpp
    messagebox.hpp
    PUBLIC_HEADER
        public.hpp
)
```

In the above example, `messagebox.hpp` is treated as a regular source file and
`public.hpp` is used as the public header.

By default PIC targets are linked to `al::al` but that can be overridden like
so:
```CMake
add_pic(
    messagebox
    entry.cpp
    messagebox.hpp
    LIBRARIES
        al::al your-pic-lib
)
```

`add_pic` creates several targets, in this example `messagebox-pe` is the PE
compiled with a position independent `.text` section. `messagebox` is an
interface library that serves two purposes. First, it triggers the `.text`
section to be dumped from the `messagebox-pe`. Next, it packages information
about the built PIC for consumer's convenience. By attaching your PIC
interface to a target via `target_link_libraries`, the target will get the
following access to the following:

- the PIC public header
- the compile definition `MESSAGEBOX_PIC_PATH` accessible from source

The generator expression `$<TARGET_PROPERTY:messagebox,SOURCES>` can be used to
get the binary file path.

### Debugging
Generally, there are two types of bugs when developing PIC:
1. Normal bugs
2. Position independence bugs

In most cases, debugging using the position independent PE is ideal. This
allows for a normal debugging workflow using your preferred IDE.

However, there are some issues that will only manifest when trying to run
code independently from the PE it was compiled into. Assembly Line provides
a GTest extension `<al/gtest/pic.hpp>` to test PIC blobs.
