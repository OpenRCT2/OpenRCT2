/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/LargeScenery.h"
#include "../world/Scenery.h"
#include "GameAction.h"

class LargeSceneryPlaceActionResult final : public GameActions::Result
{
public:
    LargeSceneryPlaceActionResult()
        : GameActions::Result(GameActions::Status::Ok, STR_CANT_POSITION_THIS_HERE)
    {
    }
    LargeSceneryPlaceActionResult(GameActions::Status error)
        : GameActions::Result(error, STR_CANT_POSITION_THIS_HERE)
    {
    }
    LargeSceneryPlaceActionResult(GameActions::Status error, rct_string_id message)
        : GameActions::Result(error, STR_CANT_POSITION_THIS_HERE, message)
    {
    }
    LargeSceneryPlaceActionResult(GameActions::Status error, rct_string_id message, uint8_t* args)
        : GameActions::Result(error, STR_CANT_POSITION_THIS_HERE, message, args)
    {
    }

    uint8_t GroundFlags{ 0 };
    TileElement* tileElement = nullptr;
};

DEFINE_GAME_ACTION(LargeSceneryPlaceAction, GAME_COMMAND_PLACE_LARGE_SCENERY, LargeSceneryPlaceActionResult)
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _sceneryType{ OBJECT_ENTRY_INDEX_NULL };
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};
    BannerIndex _bannerId{ BANNER_INDEX_NULL };

    int16_t GetTotalNumTiles(rct_large_scenery_tile * tiles) const;
    int16_t GetMaxSurfaceHeight(rct_large_scenery_tile * tiles) const;
    void SetNewLargeSceneryElement(LargeSceneryElement & sceneryElement, uint8_t tileNum) const;

public:
    LargeSceneryPlaceAction() = default;

    LargeSceneryPlaceAction(const CoordsXYZD& loc, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _sceneryType(sceneryType)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
        rct_scenery_entry* sceneryEntry = get_large_scenery_entry(_sceneryType);
        if (sceneryEntry != nullptr)
        {
            if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
            {
                _bannerId = create_new_banner(0);
            }
        }
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;
};
