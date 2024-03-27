#include <Windows.h>
#include <format>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string_view>
#include <span>
#include <bit>

std::vector<std::byte> read_file(const std::filesystem::path& path) {
    std::basic_ifstream<std::byte> file{ path, std::ios::binary };

    // ~basic_ifstream() closes the file handle
    return std::vector<std::byte>{
        std::istreambuf_iterator<std::byte>{file},
        std::istreambuf_iterator<std::byte>()
    };
}

void write_file(
    std::span<std::byte> content,
    const std::filesystem::path& path
) {
    std::basic_ofstream<std::byte> outfile{ path, std::ios::binary };

    try {
        outfile.write(content.data(), content.size_bytes());
    }
    catch(const std::exception& e) {
        std::cerr << std::format(
            "Failed to dump section to output file: {}",
            e.what()
        );
        exit(EXIT_FAILURE);
    }
}

auto get_text_section(std::span<std::byte> file) {
    auto base = reinterpret_cast<uintptr_t>(file.data());
    auto dos  = reinterpret_cast<PIMAGE_DOS_HEADER>(file.data());

    if (IMAGE_DOS_SIGNATURE != dos->e_magic) {
        std::wcerr << std::format(L"File is not a PE\n");
        exit(EXIT_FAILURE);
    }

    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(dos->e_lfanew + base);
    auto header = IMAGE_FIRST_SECTION(nt);
    auto end = header + nt->FileHeader.NumberOfSections;

    while (0 != strcmp(".text", reinterpret_cast<char*>(header->Name))) {
        if (header == end) {
            std::wcerr << std::format(L"PE does not have a .text section\n");
            exit(EXIT_FAILURE);
        }
        header++;
    }

    return std::tuple(
        std::span<std::byte>{
            reinterpret_cast<std::byte*>(base + header->PointerToRawData),
            header->Misc.VirtualSize // SizeOfRawData includes page alignment padding
        },
        header->VirtualAddress - header->PointerToRawData
    );
}

uintptr_t get_entrypoint(std::span<std::byte> pe) {
    auto base = reinterpret_cast<uintptr_t>(pe.data());
    auto dos  = reinterpret_cast<PIMAGE_DOS_HEADER>(pe.data());

    if (IMAGE_DOS_SIGNATURE != dos->e_magic) {
        std::wcerr << std::format(L"Target is not a PE file");
        exit(EXIT_FAILURE);
    }

    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(dos->e_lfanew + base);
    return base + nt->OptionalHeader.AddressOfEntryPoint;
}

std::ptrdiff_t calculate_offset(
    std::span<std::byte> pe,
    std::span<std::byte> text,
    unsigned long        vo
) {
    return get_entrypoint(pe) - reinterpret_cast<uintptr_t>(text.data()) - vo;
}

uint64_t encode_jmp(uint32_t offset) {
    // 0xE9 is the jmp opcode
    // 0x5 is the length of a jmp instruction
    // Offset is little-endian encoded
    return 0xE900000000 + std::byteswap(offset - 0x5);
}

void patch_text_section(std::span<std::byte> text, uint64_t instruction) {
    // Copying one byte at a time so we need to reverse endianness 
    instruction = std::byteswap(instruction << 24);
    std::copy_n(reinterpret_cast<std::byte*>(&instruction), 5, text.data());
}


int wmain(int argc, wchar_t* argv[]) {
    if (2 > argc) {
        std::wcerr << std::format(L"Usage: {} <pe_filepath> [patch_entrypoint]", argv[0]);
        return EXIT_FAILURE;
    }

    auto input = std::filesystem::absolute(argv[1]);
    auto pe = read_file(std::filesystem::absolute(input));
    auto [text, virtual_offset] = get_text_section(pe);

    if (3 == argc) {
        auto instruction = encode_jmp(calculate_offset(
            pe,
            text,
            virtual_offset
        ));

        std::wcout << std::format(
            L"Assembly Line: al::dump is patching .text with jmp to entrypoint (0x{:x})\n",
            instruction
        );
        patch_text_section(text, instruction);
        write_file(pe, input);
    }

    input.replace_extension(".bin");
    write_file(text, input);

    return EXIT_SUCCESS;
}