/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    void FootpathObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(10, STREAM_SEEK_CURRENT);
        _legacyType.support_type = static_cast<RailingEntrySupportType>(stream->ReadValue<uint8_t>());
        _legacyType.flags = stream->ReadValue<uint8_t>();
        _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
        stream->Seek(1, STREAM_SEEK_CURRENT);

        GetStringTable().Read(context, stream, ObjectStringID::name);
        GetImageTable().Read(context, stream);

        // Validate properties
        if (_legacyType.support_type >= RailingEntrySupportType::count)
        {
            context->LogError(ObjectError::invalidProperty, "RailingEntrySupportType not supported.");
        }
    }

    void FootpathObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.string_idx = LanguageAllocateObjectString(GetName());
        _legacyType.image = LoadImages();
        _legacyType.bridge_image = _legacyType.image + 109;

        _pathSurfaceDescriptor.name = _legacyType.string_idx;
        _pathSurfaceDescriptor.image = _legacyType.image;
        _pathSurfaceDescriptor.previewImage = _legacyType.GetPreviewImage();
        _pathSurfaceDescriptor.flags = _legacyType.flags;

        _queueSurfaceDescriptor.name = _legacyType.string_idx;
        _queueSurfaceDescriptor.image = _legacyType.GetQueueImage();
        _queueSurfaceDescriptor.previewImage = _legacyType.GetQueuePreviewImage();
        _queueSurfaceDescriptor.flags = _legacyType.flags | FOOTPATH_ENTRY_FLAG_IS_QUEUE;

        _pathRailingsDescriptor.name = _legacyType.string_idx;
        _pathRailingsDescriptor.bridgeImage = _legacyType.bridge_image;
        _pathRailingsDescriptor.previewImage = _legacyType.GetPreviewImage();
        _pathRailingsDescriptor.flags = _legacyType.flags;
        _pathRailingsDescriptor.scrollingMode = _legacyType.scrolling_mode;
        _pathRailingsDescriptor.supportType = _legacyType.support_type;
        _pathRailingsDescriptor.railingsImage = _legacyType.GetRailingsImage();
    }

    void FootpathObject::Unload()
    {
        LanguageFreeObjectString(_legacyType.string_idx);
        UnloadImages();

        _legacyType.string_idx = 0;
        _legacyType.image = 0;
    }

    void FootpathObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
        GfxDrawSprite(rt, ImageId(_pathSurfaceDescriptor.previewImage), screenCoords - ScreenCoordsXY{ 49, 17 });
        GfxDrawSprite(rt, ImageId(_queueSurfaceDescriptor.previewImage), screenCoords + ScreenCoordsXY{ 4, -17 });
    }

    void FootpathObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
    }
} // namespace OpenRCT2
