#include <format>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string_view>
#include <span>
#include <bit>
#include <Windows.h>

std::vector<std::byte> aread_file(const std::filesystem::path& path) {
    std::basic_ifstream<std::byte> file{ path, std::ios::binary };

    // ~basic_ifstream() closes the file handle
    return std::vector<std::byte>{
        std::istreambuf_iterator<std::byte>{file},
        std::istreambuf_iterator<std::byte>()
    };
}

auto get_pic_section(std::span<std::byte> file) {
    auto base = reinterpret_cast<uintptr_t>(file.data());
    auto dos  = reinterpret_cast<PIMAGE_DOS_HEADER>(file.data());

    if (IMAGE_DOS_SIGNATURE != dos->e_magic) {
        std::wcerr << std::format(L"File is not a PE\n");
        exit(EXIT_FAILURE);
    }

    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(dos->e_lfanew + base);
    auto header = IMAGE_FIRST_SECTION(nt);
    auto end = header + nt->FileHeader.NumberOfSections;

    while (0 != strcmp(".pic", reinterpret_cast<char*>(header->Name))) {
        if (header == end) {
            std::wcerr << std::format(L"PE does not have a .pic section\n");
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
    // Offset is little-endian encoded
    // We are including the length of the jmp instruction since we've prepended to PIC
    return 0xE900000000 + std::byteswap(offset);
}

int main(int argc, char* argv[]) {
    if (2 > argc) {
        std::cerr << std::format("Usage: {} <pe_filepath> [patch_entrypoint]", argv[0]);
        return EXIT_FAILURE;
    }

    auto input = std::filesystem::absolute(argv[1]);
    auto pe = aread_file(input);
    auto [pic, virtual_offset] = get_pic_section(pe);
    auto output = input.replace_extension(input.extension().string() + ".pic");

    std::basic_ofstream<std::byte> outfile{ output, std::ios::binary };

    if (3 == argc) {
        auto instruction = encode_jmp(calculate_offset(
            pe,
            pic,
            virtual_offset
        ));

        std::cout << std::format(
            "Assembly Line: al::dump is patching .text with jmp to entrypoint (0x{:x})\n",
            instruction
        );
        instruction = std::byteswap(instruction << 24);

        try {
            outfile.write(reinterpret_cast<std::byte*>(&instruction), 5);
        }
        catch(const std::exception& e) {
            std::cerr << std::format(
                "Failed to write jmp instruction to PIC file: {}",
                e.what()
            );
            exit(EXIT_FAILURE);
        }
    }

    try {
        outfile.write(pic.data(), pic.size_bytes());
    }
    catch(const std::exception& e) {
        std::cerr << std::format(
            "Failed to dump PIC section to output file: {}",
            e.what()
        );
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}