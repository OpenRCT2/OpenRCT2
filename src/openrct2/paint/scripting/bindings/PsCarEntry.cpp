#include "PsCarEntry.h"
#include "../../../ride/VehicleEntry.h"

namespace OpenRCT2::PaintScripting
{
    PsCarEntry::PsCarEntry()
        : _carEntry(nullptr)
    {
    }

    PsCarEntry::PsCarEntry(const CarEntry& carEntry)
        : _carEntry(&carEntry)
    {
    }


    void PsCarEntry::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsCarEntry>("CarEntry", sol::constructors<PsCarEntry(), PsCarEntry(const CarEntry&)>());
        type["GetBaseImageId"] = &PsCarEntry::GetBaseImageId;
    }

    uint32_t PsCarEntry::GetBaseImageId() const
    {
        return _carEntry->base_image_id;
    }
}
