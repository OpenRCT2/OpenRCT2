#pragma once

#include "../../../scripting/Duktape.hpp"

struct PaintSession;

namespace OpenRCT2::PaintScripting
{
    class PsImageId
    {
    public:
        PsImageId(const ImageId& imageId);
        static void Register(duk_context* context);

        ImageId& GetImageId();
    private:
        ImageId _imageId;
    };
} // namespace OpenRCT2::PaintScripting
