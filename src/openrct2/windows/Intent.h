/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <map>
#include <string>
#include "../common.h"
#include "../interface/Window.h"

struct IntentData
{
    enum DATATYPE { DT_INT, DT_STRING, DT_POINTER, DT_CLOSE_CALLBACK } type;

    union {
        uint32_t unsignedInt;
        int32_t signedInt;
    } intVal;
    std::string stringVal;
    close_callback closeCallbackVal;
    void * pointerVal;
};

class Intent
{
private:
    rct_windowclass _Class;
    std::map<uint32_t, IntentData> _Data;
public:
    explicit Intent(rct_windowclass windowclass);
    rct_windowclass GetWindowClass() const;
    void * GetPointerExtra(uint32_t key) const;
    std::string GetStringExtra(uint32_t key) const;
    uint32_t GetUIntExtra(uint32_t key) const;
    int32_t GetSIntExtra(uint32_t key) const;
    close_callback GetCloseCallbackExtra(uint32_t key) const;
    Intent * putExtra(uint32_t key, uint32_t value);
    Intent * putExtra(uint32_t key, void * value);
    Intent * putExtra(uint32_t key, int32_t value);
    Intent * putExtra(uint32_t key, std::string value);
    Intent * putExtra(uint32_t key, close_callback value);
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
    INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS,
    INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES,
    INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS,
    INTENT_ACTION_INIT_SCENERY,
    INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG,
    INTENT_ACTION_REFRESH_SCENERY,
    INTENT_ACTION_INVALIDATE_TICKER_NEWS,
    INTENT_ACTION_REFRESH_GUEST_LIST,
    INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD,
    INTENT_ACTION_REFRESH_STAFF_LIST,
    INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW,
    INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE,
    INTENT_ACTION_UPDATE_CLIMATE,
    INTENT_ACTION_UPDATE_GUEST_COUNT,
    INTENT_ACTION_UPDATE_PARK_RATING,
    INTENT_ACTION_UPDATE_DATE,
    INTENT_ACTION_UPDATE_CASH,
    INTENT_ACTION_UPDATE_BANNER,
    INTENT_ACTION_UPDATE_RESEARCH,
};
