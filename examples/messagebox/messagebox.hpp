#pragma once

extern "C" unsigned int entry(const wchar_t* text, const wchar_t* title);
using messagebox_t = decltype(&entry);