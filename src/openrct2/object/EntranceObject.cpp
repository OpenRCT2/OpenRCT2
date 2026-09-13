/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EntranceObject.h"

#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../paint/tile_element/Paint.TileElement.h"

namespace OpenRCT2
{
    void EntranceObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        stream->Seek(6, STREAM_SEEK_CURRENT);
        _legacyType.scrolling_mode = stream->ReadValue<uint8_t>();
        _legacyType.text_height = stream->ReadValue<uint8_t>();

        GetStringTable().Read(context, stream, ObjectStringID::name);
        GetImageTable().Read(context, stream);
    }

    void EntranceObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.string_idx = LanguageAllocateObjectString(GetName());
        _legacyType.image_id = LoadImages();
    }

    void EntranceObject::Unload()
    {
        LanguageFreeObjectString(_legacyType.string_idx);
        UnloadImages();

        _legacyType.string_idx = 0;
        _legacyType.image_id = 0;
    }

    void EntranceObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto screenCoords = ScreenCoordsXY{ width / 2, height / 2 };
        GfxDrawSprite(rt, ImageId(_legacyType.image_id + 1), screenCoords + ScreenCoordsXY{ -32, 14 });
        GfxDrawSprite(rt, ImageId(_legacyType.image_id + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
        GfxDrawSprite(rt, ImageId(_legacyType.image_id + 2), screenCoords + ScreenCoordsXY{ 32, 44 });
    }

    void EntranceObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "EntranceObject::ReadJson expects parameter root to be object");

        json_t properties = root["properties"];

        if (properties.is_object())
        {
            _legacyType.scrolling_mode = Json::GetNumber<uint8_t>(properties["scrollingMode"]);
            _legacyType.text_height = Json::GetNumber<uint8_t>(properties["textHeight"]);
        }

        PopulateTablesFromJson(context, root);
    }

    ImageIndex EntranceObject::GetImage(uint8_t sequence, Direction direction) const
    {
        if (sequence > 2)
            return kImageIndexUndefined;
        return _legacyType.image_id + ((direction & 3) * 3) + sequence;
    }

    uint8_t EntranceObject::GetScrollingMode() const
    {
        return _legacyType.scrolling_mode;
    }

    uint8_t EntranceObject::GetTextHeight() const
    {
        return _legacyType.text_height;
    }
} // namespace OpenRCT2
