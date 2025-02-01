/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Identifier.hpp"
#include "../interface/Window.h"

#include <map>
#include <sfl/static_vector.hpp>
#include <string>
#include <variant>

namespace OpenRCT2
{
    enum IntentAction
    {
        INTENT_ACTION_MAP,
        INTENT_ACTION_NEW_RIDE_OF_TYPE,
        INTENT_ACTION_REFRESH_CAMPAIGN_RIDE_LIST,
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
        INTENT_ACTION_UPDATE_VEHICLE_SOUNDS,
        INTENT_ACTION_SET_MAP_TOOLTIP,
        INTENT_ACTION_NEW_SCENERY,
        INTENT_ACTION_TILE_MODIFY,
        INTENT_ACTION_PROGRESS_OPEN,
        INTENT_ACTION_PROGRESS_SET,
        INTENT_ACTION_PROGRESS_CLOSE,
        INTENT_ACTION_REMOVE_PROVISIONAL_ELEMENTS,
        INTENT_ACTION_RESTORE_PROVISIONAL_ELEMENTS,
        INTENT_ACTION_REMOVE_PROVISIONAL_FOOTPATH,
        INTENT_ACTION_REMOVE_PROVISIONAL_TRACK_PIECE,

        INTENT_ACTION_NULL = 255,
    };

    // The maximum amount of data the Intent can hold, 8 should be sufficient, raise this if needed.
    static constexpr size_t kIntentMaxDataSlots = 8;

    using IntentData = std::variant<int64_t, std::string, CloseCallback, void*>;
    using IntentDataEntry = std::pair<uint32_t, IntentData>;
    using IntentDataStorage = sfl::static_vector<IntentDataEntry, kIntentMaxDataSlots>;

    class Intent
    {
        WindowClass _Class{ WindowClass::Null };
        WindowDetail _WindowDetail{ WD_NULL };
        IntentAction _Action{ INTENT_ACTION_NULL };
        IntentDataStorage _Data;

    public:
        explicit Intent(WindowClass windowClass);
        explicit Intent(WindowDetail windowDetail);
        explicit Intent(IntentAction windowclass);

        WindowClass GetWindowClass() const;
        WindowDetail GetWindowDetail() const;
        IntentAction GetAction() const;

        void* GetPointerExtra(uint32_t key) const;
        std::string GetStringExtra(uint32_t key) const;
        uint32_t GetUIntExtra(uint32_t key) const;
        int32_t GetSIntExtra(uint32_t key) const;
        CloseCallback GetCloseCallbackExtra(uint32_t key) const;

        Intent* PutExtra(uint32_t key, uint32_t value);
        Intent* PutExtra(uint32_t key, void* value);
        Intent* PutExtra(uint32_t key, int32_t value);
        Intent* PutExtra(uint32_t key, std::string value);
        Intent* PutExtra(uint32_t key, CloseCallback value);

        template<typename T, T TNull, typename TTag>
        Intent* PutExtra(uint32_t key, const TIdentifier<T, TNull, TTag>& value)
        {
            const auto val = value.ToUnderlying();
            return PutExtra(key, static_cast<uint32_t>(val));
        }
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
        INTENT_EXTRA_FORMATTER,
        INTENT_EXTRA_SCENERY_GROUP_ENTRY_INDEX,
        INTENT_EXTRA_PROGRESS_OFFSET,
        INTENT_EXTRA_PROGRESS_TOTAL,
        INTENT_EXTRA_STRING_ID,
    };
} // namespace OpenRCT2
