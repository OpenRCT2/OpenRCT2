/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/TileElement.h"
#include "GameAction.h"

struct ScatterToolPlaceActionResult
{
    uint8_t GroundFlags{};
    int32_t BaseHeight{};
    uint8_t SceneryQuadrant{};
};

class ScatterToolPlaceAction final : public GameActionBase<GameCommand::PlaceScenery>
{
private:
    CoordsXYZD _loc;
    uint8_t _quadrant{};
    ObjectEntryIndex _sceneryType{};
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};
    uint8_t _tertiaryColour{};

public:
    ScatterToolPlaceAction() = default;
    ScatterToolPlaceAction(
        const CoordsXYZD& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour,
        uint8_t tertiaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint32_t GetCooldownTime() const override;
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};

void scatterItems(int quantity, int gridPos_x, int gridPos_y, int selectedScenery, int quadrant, colour_t primaryColour, colour_t secondaryColour, colour_t tertiaryColour, uint16_t sceneryScatterSize);