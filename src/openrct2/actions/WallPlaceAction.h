/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

struct WallPlaceActionResult
{
    int32_t BaseHeight{};
    BannerIndex BannerId = BannerIndex::GetNull();
};

class WallPlaceAction final : public GameActionBase<GameCommand::PlaceWall>
{
private:
    ObjectEntryIndex _wallType{ OBJECT_ENTRY_INDEX_NULL };
    CoordsXYZ _loc;
    Direction _edge{ INVALID_DIRECTION };
    int32_t _primaryColour{ COLOUR_BLACK };
    int32_t _secondaryColour{ COLOUR_BLACK };
    int32_t _tertiaryColour{ COLOUR_BLACK };

public:
    WallPlaceAction() = default;
    WallPlaceAction(
        ObjectEntryIndex wallType, const CoordsXYZ& loc, uint8_t edge, int32_t primaryColour, int32_t secondaryColour,
        int32_t tertiaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override final;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    /**
     *
     *  rct2: 0x006E5CBA
     */
    bool WallCheckObstructionWithTrack(
        WallSceneryEntry* wall, int32_t z0, TrackElement* trackElement, bool* wallAcrossTrack) const;
    /**
     *
     *  rct2: 0x006E5C1A
     */
    GameActions::Result WallCheckObstruction(WallSceneryEntry* wall, int32_t z0, int32_t z1, bool* wallAcrossTrack) const;

    /**
     * Gets whether the given track type can have a wall placed on the edge of the given direction.
     * Some thin tracks for example are allowed to have walls either side of the track, but wider tracks can not.
     */
    static bool TrackIsAllowedWallEdges(ride_type_t rideType, track_type_t trackType, uint8_t trackSequence, uint8_t direction);
};
