#pragma once

// Place entry function in dedicated section fragment
#ifndef __GNUC__
#define AL_ENTRY _Pragma("code_seg(\".al$200\")")
#else
#define AL_ENTRY __attribute__((section (".al$200")))
#endif

#ifdef __cplusplus

import al;

// Convenience macros for imports
//
// Cast a pointer to the type of a declaration known to the compiler
#define DECL_CAST(symbol, expression) reinterpret_cast<decltype(&symbol)>(expression)

// Get module handle by string, hash, or custom criteria
#define GM(dll, ...) al::get_module(dll ## __VA_OPT__(_) ## __VA_ARGS__)

// Get proc address by string, hash, or custom criteria
#define GP(dll, symbol, ...) DECL_CAST(symbol, al::get_proc(dll, # symbol ## __VA_OPT__(_) ## __VA_ARGS__))

#else // !__cplusplus

#include <wchar.h>

// Resolve base address of loaded DLL
void* get_module_handle(const wchar_t* name);
void* get_module_hash(unsigned long hash, unsigned long (*hasher)(const wchar_t*));

// Resolve address of DLL export
void* get_proc_address(void* module, const char* name);
void* get_proc_hash(void* module, unsigned long hash, unsigned long (*hasher)(const char*));

// String hashing algorithms
unsigned long ror13_ansi(const char*);
unsigned long ror13_utf16(const wchar_t*);
unsigned long djb2_ansi(const char*);
unsigned long djb2_utf16(const wchar_t*);

#endif // __cplusplus

