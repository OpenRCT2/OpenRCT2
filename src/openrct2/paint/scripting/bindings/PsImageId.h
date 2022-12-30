#pragma once

#include <sol/sol.hpp>
#include "../../../drawing/ImageId.hpp"

struct ImageId;
namespace OpenRCT2::PaintScripting
{
    class PsImageId
    {
    public:
        PsImageId();
        PsImageId(const ImageId& imageId);
        PsImageId(uint32_t index, uint8_t primaryColour, uint8_t secondaryColour);

        static void Register(sol::state& lua);

        const ImageId& GetImageId() const;
    private:
        ImageId _imageId;
        bool IsRemap() const;
        PsImageId WithIndex(uint32_t imageIndex);
    };
}
