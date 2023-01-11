/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LandLowerAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../actions/LandSetHeightAction.h"
#include "../audio/audio.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/RideConstruction.h"
#include "../ride/RideData.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"

LandLowerAction::LandLowerAction(const CoordsXY& coords, MapRange range, uint8_t selectionType)
    : _coords(coords)
    , _range(range)
    , _selectionType(selectionType)
{
}

void LandLowerAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_coords);
    visitor.Visit(_range);
    visitor.Visit("selectionType", _selectionType);
}

uint16_t LandLowerAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void LandLowerAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_coords) << DS_TAG(_range) << DS_TAG(_selectionType);
}

GameActions::Result LandLowerAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result LandLowerAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result LandLowerAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();
    size_t tableRow = _selectionType;

    // The selections between MAP_SELECT_TYPE_FULL and MAP_SELECT_TYPE_EDGE_0 are not included in the tables
    if (_selectionType >= MAP_SELECT_TYPE_EDGE_0 && _selectionType <= MAP_SELECT_TYPE_EDGE_3)
        tableRow -= MAP_SELECT_TYPE_EDGE_0 - MAP_SELECT_TYPE_FULL - 1;

    auto validRange = ClampRangeWithinMap(_range);

    res.Position = { _coords.x, _coords.y, TileElementHeight(_coords) };
    res.Expenditure = ExpenditureType::Landscaping;

    if (isExecuting)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, { _coords.x, _coords.y, TileElementHeight(_coords) });
    }

    uint8_t maxHeight = MapGetHighestLandHeight(validRange);
    bool withinOwnership = false;

    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
        {
            if (!LocationValid({ x, y }))
                continue;
            auto* surfaceElement = MapGetSurfaceElementAt(CoordsXY{ x, y });
            if (surfaceElement == nullptr)
                continue;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!MapIsLocationInPark(CoordsXY{ x, y }))
                {
                    continue;
                }
            }
            withinOwnership = true;

            uint8_t height = surfaceElement->base_height;
            if (surfaceElement->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK)
                height += 2;
            if (surfaceElement->GetSlope() & TILE_ELEMENT_SURFACE_DIAGONAL_FLAG)
                height += 2;

            if (height < maxHeight)
                continue;

            height = surfaceElement->base_height;
            uint8_t currentSlope = surfaceElement->GetSlope();
            uint8_t newSlope = tile_element_lower_styles[tableRow][currentSlope];
            if (newSlope & SURFACE_STYLE_FLAG_RAISE_OR_LOWER_BASE_HEIGHT)
                height -= 2;

            newSlope &= TILE_ELEMENT_SURFACE_SLOPE_MASK;

            auto landSetHeightAction = LandSetHeightAction({ x, y }, height, newSlope);
            landSetHeightAction.SetFlags(GetFlags());
            auto result = isExecuting ? GameActions::ExecuteNested(&landSetHeightAction)
                                      : GameActions::QueryNested(&landSetHeightAction);
            if (result.Error == GameActions::Status::Ok)
            {
                res.Cost += result.Cost;
            }
            else
            {
                result.ErrorTitle = STR_CANT_LOWER_LAND_HERE;
                return result;
            }
        }
    }

    if (!withinOwnership)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_LOWER_LAND_HERE, STR_LAND_NOT_OWNED_BY_PARK);
    }

    // Force ride construction to recheck area
    _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;

    return res;
}
