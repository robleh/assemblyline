# Dump
Dumps the `.text` section of a PE to disk. It is intended to be used as a
custom command within CMake via the target `al::dump`.

## Usage
dump.exe <pe_filepath> [patch_entrypoint]

- `pe_filepath` path to the PE who's `.text` section contains PIC
- `patch_entrypoint` optional boolean that determines whether to patch the
beginning of the PIC with a `jmp` instruction to the PE entrypoint
