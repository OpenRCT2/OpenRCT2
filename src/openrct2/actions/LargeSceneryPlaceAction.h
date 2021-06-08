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
    LargeSceneryPlaceActionResult();
    LargeSceneryPlaceActionResult(GameActions::Status error);
    LargeSceneryPlaceActionResult(GameActions::Status error, rct_string_id message);
    LargeSceneryPlaceActionResult(GameActions::Status error, rct_string_id message, uint8_t* args);

    uint8_t GroundFlags{ 0 };
    TileElement* tileElement = nullptr;
    BannerIndex bannerId = BANNER_INDEX_NULL;
};

DEFINE_GAME_ACTION(LargeSceneryPlaceAction, GameCommand::PlaceLargeScenery, LargeSceneryPlaceActionResult)
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _sceneryType{ OBJECT_ENTRY_INDEX_NULL };
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};

public:
    LargeSceneryPlaceAction() = default;

    LargeSceneryPlaceAction(
        const CoordsXYZD& loc, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    int16_t GetTotalNumTiles(rct_large_scenery_tile * tiles) const;
    bool CheckMapCapacity(rct_large_scenery_tile * tiles, int16_t numTiles) const;
    int16_t GetMaxSurfaceHeight(rct_large_scenery_tile * tiles) const;
    void SetNewLargeSceneryElement(LargeSceneryElement & sceneryElement, uint8_t tileNum) const;
};
