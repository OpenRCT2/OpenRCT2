#include "PsImageId.h"
#include "../../../drawing/ImageId.hpp"
#include "../../../scripting/Duktape.hpp"

namespace OpenRCT2::PaintScripting
{
    PsImageId::PsImageId(const ImageId& imageId): _imageId(imageId)
    {

    }

    void PsImageId::Register(duk_context* context)
    {
        //we just need to be able to build an imageId, we don't need to access any field
        dukglue_register_constructor<PsImageId, const ImageId&>(context, "ImageId");
    }

    ImageId& PsImageId::GetImageId()
    {
        return _imageId;
    }
}
