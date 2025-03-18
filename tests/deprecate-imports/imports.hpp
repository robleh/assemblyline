#pragma once

struct results {
    void* module_name;
    void* module_ror13;
    void* module_djb2;
    void* import_name;
    void* import_ror13;
    void* import_djb2;
};

extern "C" void entry(results* imports);
using imports_t = decltype(&entry);
