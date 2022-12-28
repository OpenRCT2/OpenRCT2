#pragma once

#include <sol/sol.hpp>

struct ImageId;
namespace OpenRCT2::PaintScripting
{
    class PsImageId
    {
    public:
        PsImageId();
        PsImageId(const ImageId& imageId);

        static void Register(sol::state& lua);

        const ImageId& GetImageId() const;
    private:
        const ImageId* _imageId;
    };
}
