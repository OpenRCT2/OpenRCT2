/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FootpathObject.h"

#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../world/Footpath.h"

void FootpathObject::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    stream->Seek(10, OpenRCT2::STREAM_SEEK_CURRENT);
    _legacyType.support_type = static_cast<RailingEntrySupportType>(stream->ReadValue<uint8_t>());
    _legacyType.flags = stream->ReadValue<uint8_t>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
    stream->Seek(1, OpenRCT2::STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, ObjectStringID::NAME);
    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.support_type >= RailingEntrySupportType::Count)
    {
        context->LogError(ObjectError::InvalidProperty, "RailingEntrySupportType not supported.");
    }
}

void FootpathObject::Load()
{
    GetStringTable().Sort();
    _legacyType.string_idx = LanguageAllocateObjectString(GetName());
    _legacyType.image = LoadImages();
    _legacyType.bridge_image = _legacyType.image + 109;

    _pathSurfaceDescriptor.Name = _legacyType.string_idx;
    _pathSurfaceDescriptor.Image = _legacyType.image;
    _pathSurfaceDescriptor.PreviewImage = _legacyType.GetPreviewImage();
    _pathSurfaceDescriptor.Flags = _legacyType.flags;

    _queueSurfaceDescriptor.Name = _legacyType.string_idx;
    _queueSurfaceDescriptor.Image = _legacyType.GetQueueImage();
    _queueSurfaceDescriptor.PreviewImage = _legacyType.GetQueuePreviewImage();
    _queueSurfaceDescriptor.Flags = _legacyType.flags | FOOTPATH_ENTRY_FLAG_IS_QUEUE;

    _pathRailingsDescriptor.Name = _legacyType.string_idx;
    _pathRailingsDescriptor.BridgeImage = _legacyType.bridge_image;
    _pathRailingsDescriptor.PreviewImage = _legacyType.GetPreviewImage();
    _pathRailingsDescriptor.Flags = _legacyType.flags;
    _pathRailingsDescriptor.ScrollingMode = _legacyType.scrolling_mode;
    _pathRailingsDescriptor.SupportType = _legacyType.support_type;
    _pathRailingsDescriptor.RailingsImage = _legacyType.GetRailingsImage();
}

void FootpathObject::Unload()
{
    LanguageFreeObjectString(_legacyType.string_idx);
    UnloadImages();

    _legacyType.string_idx = 0;
    _legacyType.image = 0;
}

void FootpathObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
    GfxDrawSprite(dpi, ImageId(_pathSurfaceDescriptor.PreviewImage), screenCoords - ScreenCoordsXY{ 49, 17 });
    GfxDrawSprite(dpi, ImageId(_queueSurfaceDescriptor.PreviewImage), screenCoords + ScreenCoordsXY{ 4, -17 });
}

void FootpathObject::ReadJson(IReadObjectContext* context, json_t& root)
{
}
