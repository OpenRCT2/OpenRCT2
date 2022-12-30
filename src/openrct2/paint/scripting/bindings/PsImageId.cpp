#include "PsImageId.h"
#include "../../../drawing/Drawing.h"

namespace OpenRCT2::PaintScripting
{
    PsImageId::PsImageId()
    {
    }

    PsImageId::PsImageId(const ImageId& imageId)
        : _imageId(imageId)
    {
    }

    PsImageId::PsImageId(uint32_t index, uint8_t primaryColour, uint8_t secondaryColour)
    {
        _imageId = ImageId(index, primaryColour, secondaryColour);
    }

    void PsImageId::Register(sol::state& lua)
    {
        auto type = lua.new_usertype<PsImageId>(
            "ImageId",
            sol::constructors<
                PsImageId(), PsImageId(const ImageId&),
                PsImageId(uint32_t index, uint8_t primaryColour, uint8_t secondaryColour)>());
        type["IsRemap"] = &PsImageId::IsRemap;
        type["WithIndex"] = &PsImageId::WithIndex;
    }

    const ImageId& PsImageId::GetImageId() const
    {
        return _imageId;
    }

    bool PsImageId::IsRemap() const
    {
        return _imageId.ToUInt32() == IMAGE_TYPE_REMAP;
    }

    PsImageId PsImageId::WithIndex(uint32_t imageIndex)
    {
        return _imageId.WithIndex(imageIndex);
    }
} // namespace OpenRCT2::PaintScripting
