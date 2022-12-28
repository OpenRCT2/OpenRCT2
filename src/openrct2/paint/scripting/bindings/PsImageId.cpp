#include "PsImageId.h"
#include "../../../drawing/ImageId.hpp"

namespace OpenRCT2::PaintScripting
{
    PsImageId::PsImageId(): _imageId(nullptr)
    {

    }

    PsImageId::PsImageId(const ImageId& imageId)
        : _imageId(&imageId)
    {
    }

    void PsImageId::Register(sol::state& lua)
    {
        lua.new_usertype<PsImageId>(
            "ImageId", sol::constructors<PsImageId(), PsImageId(const ImageId&)>());
    }

    const ImageId& PsImageId::GetImageId() const
    {
        return *_imageId;
    }
}
