/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.hpp"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(LandRaiseAction, GAME_COMMAND_RAISE_LAND, GameActionResult)
{
private:
    CoordsXY _coords;
    MapRange _range;
    uint8_t _selectionType;

public:
    LandRaiseAction()
    {
    }
    LandRaiseAction(CoordsXY coords, MapRange range, uint8_t selectionType)
        : _coords(coords)
        , _range(range)
        , _selectionType(selectionType)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_coords) << DS_TAG(_range) << DS_TAG(_selectionType);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();
        size_t tableRow = _selectionType;

        // The selections between MAP_SELECT_TYPE_FULL and MAP_SELECT_TYPE_EDGE_0 are not included in the tables
        if (_selectionType >= MAP_SELECT_TYPE_EDGE_0 && _selectionType <= MAP_SELECT_TYPE_EDGE_3)
            tableRow -= MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1;

        // Keep big coordinates within map boundaries
        auto aX = std::max<decltype(_range.GetLeft())>(32, _range.GetLeft());
        auto bX = std::min<decltype(_range.GetRight())>(gMapSizeMaxXY, _range.GetRight());
        auto aY = std::max<decltype(_range.GetTop())>(32, _range.GetTop());
        auto bY = std::min<decltype(_range.GetBottom())>(gMapSizeMaxXY, _range.GetBottom());

        MapRange validRange = MapRange{ aX, aY, bX, bY };

        res->Position = { _coords.x, _coords.y, tile_element_height(_coords) };
        res->Expenditure = ExpenditureType::Landscaping;

        if (isExecuting)
        {
            audio_play_sound_at_location(SoundId::PlaceItem, { _coords.x, _coords.y, tile_element_height(_coords) });
        }

        uint8_t minHeight = map_get_lowest_land_height(validRange);

        for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
        {
            for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
            {
                auto* surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
                if (surfaceElement == nullptr)
                    continue;
                uint8_t height = surfaceElement->base_height;

                if (height > minHeight)
                    continue;

                uint8_t currentSlope = surfaceElement->GetSlope();
                uint8_t newSlope = tile_element_raise_styles[tableRow][currentSlope];
                if (newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                    height += 2;

                newSlope &= TILE_ELEMENT_SURFACE_SLOPE_MASK;

                auto landSetHeightAction = LandSetHeightAction({ x, y }, height, newSlope);
                landSetHeightAction.SetFlags(GetFlags());
                auto result = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                                          : GameActions::QueryNested(&landSetHeightAction);
                if (result->Error == GA_ERROR::OK)
                {
                    res->Cost += result->Cost;
                }
                else
                {
                    result->ErrorTitle = STR_CANT_RAISE_LAND_HERE;
                    return result;
                }
            }
        }

        // Force ride construction to recheck area
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

        return res;
    }
};
