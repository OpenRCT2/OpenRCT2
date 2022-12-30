#pragma once

#include <sol/sol.hpp>

struct CarEntry;
namespace OpenRCT2::PaintScripting
{
    class PsCarEntry
    {
    public:
        PsCarEntry();
        PsCarEntry(const CarEntry& carEntry);

        static void Register(sol::state& lua);
    private:
        const CarEntry* _carEntry;
        uint32_t GetBaseImageId() const;
    };
}
