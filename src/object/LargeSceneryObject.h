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

#pragma once

#include "Object.h"

extern "C"
{
    #include "../world/scenery.h"
}

class LargeSceneryObject : public Object
{
private:
    rct_scenery_entry           _legacyType;
    rct_object_entry            _sceneryTabEntry;
    rct_large_scenery_text *    _3dFont;
    rct_large_scenery_tile *    _tiles;

public:
    explicit LargeSceneryObject(const rct_object_entry &entry) : Object(entry) { };
    ~LargeSceneryObject();

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadLegacy(IStream * stream) override;
    void Load() override;
    void Unload() override;

    const utf8 * GetName() override;

private:
    static rct_large_scenery_tile * ReadTiles(IStream * stream);
};
