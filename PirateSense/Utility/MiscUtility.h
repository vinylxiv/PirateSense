// PirateSense.

#pragma once

#include <cstdint>
#include <string>

class MiscUtility
{
public:
    static uintptr_t MillisecondsNow();

    static std::string GetShortName(const std::string& Name);
};
