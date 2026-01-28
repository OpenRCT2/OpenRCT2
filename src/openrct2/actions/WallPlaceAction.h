/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/RideData.h"
#include "../ride/TrackData.h"
#include "../world/Banner.h"
#include "../world/Scenery.h"
#include "GameAction.h"

namespace OpenRCT2
{
    struct WallSceneryEntry;
}

namespace OpenRCT2::GameActions
{
    struct WallPlaceActionResult
    {
        int32_t BaseHeight{};
        BannerIndex BannerId = BannerIndex::GetNull();
    };

    class WallPlaceAction final : public GameActionBase<GameCommand::PlaceWall>
    {
    private:
        ObjectEntryIndex _wallType{ kObjectEntryIndexNull };
        CoordsXYZ _loc;
        Direction _edge{ kInvalidDirection };
        Drawing::Colour _primaryColour{ Drawing::Colour::black };
        Drawing::Colour _secondaryColour{ Drawing::Colour::black };
        Drawing::Colour _tertiaryColour{ Drawing::Colour::black };

    public:
        WallPlaceAction() = default;
        WallPlaceAction(
            ObjectEntryIndex wallType, const CoordsXYZ& loc, uint8_t edge, Drawing::Colour primaryColour,
            Drawing::Colour secondaryColour, Drawing::Colour tertiaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        /**
         *
         *  rct2: 0x006E5CBA
         */
        bool WallCheckObstructionWithTrack(
            const WallSceneryEntry* wall, int32_t z0, TrackElement* trackElement, bool* wallAcrossTrack) const;
        /**
         *
         *  rct2: 0x006E5C1A
         */
        Result WallCheckObstruction(const WallSceneryEntry* wall, int32_t z0, int32_t z1, bool* wallAcrossTrack) const;

        /**
         * Gets whether the given track type can have a wall placed on the edge of the given direction.
         * Some thin tracks for example are allowed to have walls either side of the track, but wider tracks can not.
         */
        static bool TrackIsAllowedWallEdges(
            ride_type_t rideType, TrackElemType trackType, uint8_t trackSequence, uint8_t direction);
    };
} // namespace OpenRCT2::GameActions
