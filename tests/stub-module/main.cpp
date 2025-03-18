#include <windows.h>
#include <iostream>

bool DllMain(HINSTANCE, unsigned long, void*) {
    return true;
}

void stub_export() {
    std::cout << "stub_export success";
}