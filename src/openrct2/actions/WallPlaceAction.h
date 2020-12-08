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
#include "../world/Scenery.h"
#include "GameAction.h"

class WallPlaceActionResult final : public GameActions::Result
{
public:
    WallPlaceActionResult()
        : GameActions::Result(GameActions::Status::Ok, STR_CANT_BUILD_PARK_ENTRANCE_HERE)
    {
    }

    WallPlaceActionResult(GameActions::Status err)
        : GameActions::Result(err, STR_CANT_BUILD_PARK_ENTRANCE_HERE)
    {
    }

    WallPlaceActionResult(GameActions::Status err, rct_string_id msg)
        : GameActions::Result(err, STR_CANT_BUILD_PARK_ENTRANCE_HERE, msg)
    {
    }

    WallPlaceActionResult(GameActions::Status error, rct_string_id msg, uint8_t* args)
        : GameActions::Result(error, STR_CANT_BUILD_PARK_ENTRANCE_HERE, msg, args)
    {
    }

    TileElement* tileElement = nullptr;
};

DEFINE_GAME_ACTION(WallPlaceAction, GAME_COMMAND_PLACE_WALL, WallPlaceActionResult)
{
private:
    ObjectEntryIndex _wallType{ OBJECT_ENTRY_INDEX_NULL };
    CoordsXYZ _loc;
    Direction _edge{ INVALID_DIRECTION };
    int32_t _primaryColour{ COLOUR_BLACK };
    int32_t _secondaryColour{ COLOUR_BLACK };
    int32_t _tertiaryColour{ COLOUR_BLACK };
    BannerIndex _bannerId{ BANNER_INDEX_NULL };

public:
    WallPlaceAction() = default;

    WallPlaceAction(
        ObjectEntryIndex wallType, const CoordsXYZ& loc, uint8_t edge, int32_t primaryColour, int32_t secondaryColour,
        int32_t tertiaryColour)
        : _wallType(wallType)
        , _loc(loc)
        , _edge(edge)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
        , _tertiaryColour(tertiaryColour)
    {
        rct_scenery_entry* sceneryEntry = get_wall_entry(_wallType);
        if (sceneryEntry != nullptr)
        {
            if (sceneryEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
            {
                _bannerId = create_new_banner(0);
            }
        }
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override final
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    /**
     *
     *  rct2: 0x006E5CBA
     */
    bool WallCheckObstructionWithTrack(rct_scenery_entry * wall, int32_t z0, TrackElement * trackElement, bool* wallAcrossTrack)
        const;
    /**
     *
     *  rct2: 0x006E5C1A
     */
    GameActions::Result::Ptr WallCheckObstruction(rct_scenery_entry * wall, int32_t z0, int32_t z1, bool* wallAcrossTrack)
        const;

    /**
     * Gets whether the given track type can have a wall placed on the edge of the given direction.
     * Some thin tracks for example are allowed to have walls either side of the track, but wider tracks can not.
     */
    static bool TrackIsAllowedWallEdges(uint8_t rideType, uint8_t trackType, uint8_t trackSequence, uint8_t direction)
    {
        if (!ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_NO_WALLS))
        {
            if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                if (FlatRideTrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
                {
                    return true;
                }
            }
            else
            {
                if (TrackSequenceElementAllowedWallEdges[trackType][trackSequence] & (1 << direction))
                {
                    return true;
                }
            }
        }
        return false;
    }
};