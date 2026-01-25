/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2
{
    struct LargeSceneryTile;
    struct LargeSceneryElement;
} // namespace OpenRCT2

namespace OpenRCT2::GameActions
{
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
        ObjectEntryIndex _sceneryType{ kObjectEntryIndexNull };
        Drawing::Colour _primaryColour{};
        Drawing::Colour _secondaryColour{};
        Drawing::Colour _tertiaryColour{};

    public:
        LargeSceneryPlaceAction() = default;

        LargeSceneryPlaceAction(
            const CoordsXYZD& loc, ObjectEntryIndex sceneryType, Drawing::Colour primaryColour, Drawing::Colour secondaryColour,
            Drawing::Colour tertiaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool CheckMapCapacity(std::span<const LargeSceneryTile> tiles, size_t numTiles) const;
        int16_t GetMaxSurfaceHeight(std::span<const LargeSceneryTile> tiles) const;
        void SetNewLargeSceneryElement(LargeSceneryElement& sceneryElement, uint8_t tileNum) const;
    };
} // namespace OpenRCT2::GameActions
