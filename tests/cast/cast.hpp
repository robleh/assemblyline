#pragma once

extern "C" int entry();
using cast_t = decltype(&entry);
