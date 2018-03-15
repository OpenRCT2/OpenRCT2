#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#include <map>
#include <string>
#include "../common.h"
#include "../interface/Window.h"

struct IntentData
{
    enum DATATYPE { DT_INT, DT_STRING, DT_POINTER, DT_CLOSE_CALLBACK } type;

    union {
        uint32 unsignedInt;
        sint32 signedInt;
    } intVal;
    std::string stringVal;
    close_callback closeCallbackVal;
    void * pointerVal;
};

class Intent
{
private:
    rct_windowclass _Class;
    std::map<uint32, IntentData> _Data;
public:
    explicit Intent(rct_windowclass windowclass);
    rct_windowclass GetWindowClass() const;
    void * GetPointerExtra(uint32 key) const;
    std::string GetStringExtra(uint32 key) const;
    uint32 GetUIntExtra(uint32 key) const;
    sint32 GetSIntExtra(uint32 key) const;
    close_callback GetCloseCallbackExtra(uint32 key) const;
    Intent * putExtra(uint32 key, uint32 value);
    Intent * putExtra(uint32 key, void * value);
    Intent * putExtra(uint32 key, sint32 value);
    Intent * putExtra(uint32 key, std::string value);
    Intent * putExtra(uint32 key, close_callback value);
};

enum
{
    INTENT_EXTRA_GUEST_LIST_FILTER,
    INTENT_EXTRA_RIDE_ID,
    INTENT_EXTRA_PATH,
    INTENT_EXTRA_PEEP,
    INTENT_EXTRA_LOADSAVE_TYPE,
    INTENT_EXTRA_CALLBACK,
    INTENT_EXTRA_TRACK_DESIGN,
    INTENT_EXTRA_RIDE_TYPE,
    INTENT_EXTRA_RIDE_ENTRY_INDEX,
    INTENT_EXTRA_TILE_ELEMENT,
    INTENT_EXTRA_VEHICLE,
    INTENT_EXTRA_MESSAGE,
    INTENT_EXTRA_LIST,
    INTENT_EXTRA_LIST_COUNT,
    INTENT_EXTRA_PAGE,
    INTENT_EXTRA_BANNER_INDEX,
};

enum
{
    INTENT_ACTION_MAP,
    INTENT_ACTION_NEW_RIDE_OF_TYPE,
    INTENT_ACTION_REFRESH_NEW_RIDES,
    INTENT_ACTION_REFRESH_RIDE_LIST,
    INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION,
    INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES,
    INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS,
    INTENT_ACTION_INIT_SCENERY,
    INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG,
    INTENT_ACTION_REFRESH_SCENERY,
    INTENT_ACTION_INVALIDATE_TICKER_NEWS,
    INTENT_ACTION_REFRESH_GUEST_LIST,
    INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD,
    INTENT_ACTION_SET_TILE_INSPECTOR_PAGE,
    INTENT_ACTION_SET_TILE_INSPECTOR_BUTTONS,
    INTENT_ACTION_REFRESH_STAFF_LIST,
    INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW,
    INTENT_ACTION_UPDATE_CLIMATE,
    INTENT_ACTION_UPDATE_GUEST_COUNT,
    INTENT_ACTION_UPDATE_PARK_RATING,
    INTENT_ACTION_UPDATE_DATE,
    INTENT_ACTION_UPDATE_CASH,
    INTENT_ACTION_UPDATE_BANNER,
    INTENT_ACTION_UPDATE_RESEARCH,
};
