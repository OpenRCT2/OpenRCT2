/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

struct LargeSceneryPlaceActionResult
{
    uint8_t GroundFlags{ 0 };
    int32_t firstTileHeight{ 0 };
    BannerIndex bannerId = BannerIndex::GetNull();
};

class LargeSceneryPlaceAction final : public GameActionBase<GameCommand::PlaceLargeScenery>
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _sceneryType{ OBJECT_ENTRY_INDEX_NULL };
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};
    uint8_t _tertiaryColour{};

public:
    LargeSceneryPlaceAction() = default;

    LargeSceneryPlaceAction(
        const CoordsXYZD& loc, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour,
        uint8_t tertiaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    int16_t GetTotalNumTiles(LargeSceneryTile* tiles) const;
    bool CheckMapCapacity(LargeSceneryTile* tiles, int16_t numTiles) const;
    int16_t GetMaxSurfaceHeight(LargeSceneryTile* tiles) const;
    void SetNewLargeSceneryElement(LargeSceneryElement& sceneryElement, uint8_t tileNum) const;
};
