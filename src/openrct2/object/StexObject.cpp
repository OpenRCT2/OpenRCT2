/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/IStream.hpp"
#include "StexObject.h"

#include "../localisation/Localisation.h"

void StexObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(6, STREAM_SEEK_CURRENT);
    _legacyType.var_06 = stream->ReadValue<uint8_t>();
    stream->Seek(1, STREAM_SEEK_CURRENT);

    GetStringTable().Read(context, stream, OBJ_STRING_ID_SCENARIO_NAME);
    GetStringTable().Read(context, stream, OBJ_STRING_ID_PARK_NAME);
    GetStringTable().Read(context, stream, OBJ_STRING_ID_SCENARIO_DETAILS);
}

void StexObject::Load()
{
    GetStringTable().Sort();
    _legacyType.scenario_name = language_allocate_object_string(GetScenarioName());
    _legacyType.park_name = language_allocate_object_string(GetParkName());
    _legacyType.details = language_allocate_object_string(GetScenarioDetails());
}

void StexObject::Unload()
{
    language_free_object_string(_legacyType.scenario_name);
    language_free_object_string(_legacyType.park_name);
    language_free_object_string(_legacyType.details);

    _legacyType.scenario_name = 0;
    _legacyType.park_name = 0;
    _legacyType.details = 0;
}

void StexObject::DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const
{
    // Write (no image)
    int32_t x = width / 2;
    int32_t y = height / 2;
    gfx_draw_string_centred(dpi, STR_WINDOW_NO_IMAGE, x, y, COLOUR_BLACK, nullptr);
}

std::string StexObject::GetName() const
{
    return GetScenarioName();
}

std::string StexObject::GetScenarioName() const
{
    return GetStringTable().GetString(OBJ_STRING_ID_SCENARIO_NAME);
}

std::string StexObject::GetScenarioDetails() const
{
    return GetStringTable().GetString(OBJ_STRING_ID_SCENARIO_DETAILS);
}

std::string StexObject::GetParkName() const
{
    return GetStringTable().GetString(OBJ_STRING_ID_PARK_NAME);
}
