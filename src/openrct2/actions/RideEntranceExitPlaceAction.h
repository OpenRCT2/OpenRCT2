/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Entrance.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideEntranceExitPlaceAction, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, GameActions::Result)
{
private:
    CoordsXY _loc;
    Direction _direction{ INVALID_DIRECTION };
    NetworkRideId_t _rideIndex{ RIDE_ID_NULL };
    StationIndex _stationNum{ STATION_INDEX_NULL };
    bool _isExit{};

public:
    RideEntranceExitPlaceAction() = default;

    RideEntranceExitPlaceAction(
        const CoordsXY& loc, Direction direction, ride_id_t rideIndex, StationIndex stationNum, bool isExit)
        : _loc(loc)
        , _direction(direction)
        , _rideIndex(rideIndex)
        , _stationNum(stationNum)
        , _isExit(isExit)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

    static GameActions::Result::Ptr TrackPlaceQuery(const CoordsXYZ& loc, const bool isExit)
    {
        auto errorTitle = isExit ? STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION
                                 : STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION;
        if (!map_check_free_elements_and_reorganise(1))
        {
            return MakeResult(GameActions::Status::NoFreeElements, errorTitle);
        }

        if (!gCheatsSandboxMode && !map_is_location_owned(loc))
        {
            return MakeResult(GameActions::Status::NotOwned, errorTitle);
        }

        int16_t baseZ = loc.z;
        int16_t clearZ = baseZ + (isExit ? RideExitHeight : RideEntranceHeight);
        auto cost = MONEY32_UNDEFINED;
        if (!map_can_construct_with_clear_at(
                { loc, baseZ, clearZ }, &map_place_non_scenery_clear_func, { 0b1111, 0 }, 0, &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MakeResult(GameActions::Status::NoClearance, errorTitle, gGameCommandErrorText, gCommonFormatArgs);
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
        {
            return MakeResult(GameActions::Status::Disallowed, errorTitle, STR_RIDE_CANT_BUILD_THIS_UNDERWATER);
        }

        if (baseZ > MaxRideEntranceOrExitHeight)
        {
            return MakeResult(GameActions::Status::Disallowed, errorTitle, STR_TOO_HIGH);
        }
        auto res = MakeResult();
        res->Position = { loc.ToTileCentre(), tile_element_height(loc) };
        res->Expenditure = ExpenditureType::RideConstruction;
        return res;
    }
};
