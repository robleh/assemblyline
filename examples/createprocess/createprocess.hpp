#pragma once

extern "C" unsigned int entry(wchar_t* cmd);
using createprocess_t = decltype(&entry);