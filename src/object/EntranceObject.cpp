#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../core/IStream.hpp"
#include "EntranceObject.h"

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
};

void EntranceObject::ReadLegacy(IStream * stream)
{
    _legacyType.string_idx = stream->ReadValue<rct_string_id>();
    _legacyType.image_id = stream->ReadValue<uint32>();
    _legacyType.scrolling_mode = stream->ReadValue<uint8>();
    _legacyType.text_height = stream->ReadValue<uint8>();

    StringTable.Read(stream, OBJ_STRING_ID_NAME);
    ImageTable.Read(stream);
}

void EntranceObject::Load()
{

}

void EntranceObject::Unload()
{

}

const utf8 * EntranceObject::GetName()
{
    return StringTable.GetString(OBJ_STRING_ID_NAME);
}
