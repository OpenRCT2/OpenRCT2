/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(SmallScenerySetColourAction, GAME_COMMAND_SET_SCENERY_COLOUR, GameActionResult)
{
private:
    CoordsXYZ _loc;
    uint8_t _quadrant;
    ObjectEntryIndex _sceneryType;
    uint8_t _primaryColour;
    uint8_t _secondaryColour;

public:
    SmallScenerySetColourAction() = default;

    SmallScenerySetColourAction(
        const CoordsXYZ& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _quadrant(quadrant)
        , _sceneryType(sceneryType)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour)
               << DS_TAG(_secondaryColour);
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
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = _loc.z;
        res->ErrorTitle = STR_CANT_REPAINT_THIS;

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_owned(_loc))
            {
                return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        auto sceneryElement = map_get_small_scenery_element_at(_loc, _sceneryType, _quadrant);

        if (sceneryElement == nullptr)
        {
            log_error("Small scenery not found at: x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(sceneryElement->IsGhost()))
        {
            return res;
        }

        if (isExecuting)
        {
            sceneryElement->SetPrimaryColour(_primaryColour);
            sceneryElement->SetSecondaryColour(_secondaryColour);

            map_invalidate_tile_full(_loc);
        }

        return res;
    }
};
