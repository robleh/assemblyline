#pragma once

extern "C" __declspec(dllexport) unsigned int entry();
using messagebox_t = decltype(&entry);