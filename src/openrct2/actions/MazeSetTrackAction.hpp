/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "GameAction.h"
// clang-format off
/** rct2: 0x00993CE9 */
static constexpr const uint8_t byte_993CE9[] = {
    0xFF, 0xE0, 0xFF,
    14, 0, 1, 2,
    6, 2, 4, 5,
    9, 10, 6, 8,
    12, 13, 14, 10,
};

/** rct2: 0x00993CFC */
static constexpr const uint8_t byte_993CFC[] = {
    5, 12, 0xFF, 0xFF,
    9, 0, 0xFF, 0xFF,
    13, 4, 0xFF, 0xFF,
    1, 8, 0xFF, 0xFF,
};

/** rct2: 0x00993D0C */
static constexpr const uint8_t byte_993D0C[] = {
    3, 0, 0xFF, 0xFF,
    0, 1, 0xFF, 0xFF,
    1, 2, 0xFF, 0xFF,
    2, 3, 0xFF, 0xFF,
};
// clang-format on

DEFINE_GAME_ACTION(MazeSetTrackAction, GAME_COMMAND_SET_MAZE_TRACK, GameActionResult)
{
private:
    CoordsXYZD _loc;
    bool _initialPlacement;
    NetworkRideId_t _rideIndex;
    uint8_t _mode;

public:
    MazeSetTrackAction()
    {
    }
    MazeSetTrackAction(CoordsXYZD location, bool initialPlacement, NetworkRideId_t rideIndex, uint8_t mode)
        : _loc(location)
        , _initialPlacement(initialPlacement)
        , _rideIndex(rideIndex)
        , _mode(mode)
    {
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_loc) << DS_TAG(_loc.direction) << DS_TAG(_initialPlacement) << DS_TAG(_rideIndex) << DS_TAG(_mode);
    }

    GameActionResult::Ptr Query() const override
    {
        auto res = std::make_unique<GameActionResult>();

        res->Position.x = _loc.x + 8;
        res->Position.y = _loc.y + 8;
        res->Position.z = _loc.z;
        res->Expenditure = ExpenditureType::RideConstruction;
        res->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
        if (!map_check_free_elements_and_reorganise(1))
        {
            res->Error = GA_ERROR::NO_FREE_ELEMENTS;
            res->ErrorMessage = STR_TILE_ELEMENT_LIMIT_REACHED;
            return res;
        }
        if ((_loc.z & 0xF) != 0)
        {
            res->Error = GA_ERROR::UNKNOWN;
            res->ErrorMessage = STR_CONSTRUCTION_ERR_UNKNOWN;
            return res;
        }

        if (!map_is_location_owned(_loc) && !gCheatsSandboxMode)
        {
            res->Error = GA_ERROR::NOT_OWNED;
            res->ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
            return res;
        }

        auto surfaceElement = map_get_surface_element_at(_loc);
        if (surfaceElement == nullptr)
        {
            res->Error = GA_ERROR::UNKNOWN;
            res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
            return res;
        }

        uint8_t baseHeight = _loc.z / 8;
        uint8_t clearanceHeight = (_loc.z + 32) / 8;

        int8_t heightDifference = baseHeight - surfaceElement->base_height;
        if (heightDifference >= 0 && !gCheatsDisableSupportLimits)
        {
            heightDifference = heightDifference >> 1;

            if (heightDifference > RideData5[RIDE_TYPE_MAZE].max_height)
            {
                res->Error = GA_ERROR::TOO_HIGH;
                res->ErrorMessage = STR_TOO_HIGH_FOR_SUPPORTS;
                return res;
            }
        }

        TileElement* tileElement = map_get_track_element_at_of_type_from_ride(_loc, TRACK_ELEM_MAZE, _rideIndex);
        if (tileElement == nullptr)
        {
            if (_mode != GC_SET_MAZE_TRACK_BUILD)
            {
                res->Error = GA_ERROR::UNKNOWN;
                res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
                return res;
            }

            if (!map_can_construct_at(floor2(_loc.x, 32), floor2(_loc.y, 32), baseHeight, clearanceHeight, { 0b1111, 0 }))
            {
                return MakeResult(GA_ERROR::NO_CLEARANCE, res->ErrorTitle, gGameCommandErrorText, gCommonFormatArgs);
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
            {
                res->Error = GA_ERROR::NO_CLEARANCE;
                res->ErrorMessage = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
                return res;
            }

            if (gMapGroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                res->Error = GA_ERROR::NO_CLEARANCE;
                res->ErrorMessage = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                return res;
            }

            auto ride = get_ride(_rideIndex);
            if (ride == nullptr || ride->type == RIDE_CRASH_TYPE_NONE)
            {
                res->Error = GA_ERROR::NO_CLEARANCE;
                res->ErrorMessage = STR_INVALID_SELECTION_OF_OBJECTS;
                return res;
            }

            money32 price = (((RideTrackCosts[ride->type].track_price * TrackPricing[TRACK_ELEM_MAZE]) >> 16));
            res->Cost = price / 2 * 10;

            return res;
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto res = std::make_unique<GameActionResult>();

        res->Position.x = _loc.x + 8;
        res->Position.y = _loc.y + 8;
        res->Position.z = _loc.z;
        res->Expenditure = ExpenditureType::RideConstruction;
        res->ErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            res->Error = GA_ERROR::INVALID_PARAMETERS;
            res->ErrorMessage = STR_NONE;
            return res;
        }

        if (!map_check_free_elements_and_reorganise(1))
        {
            res->Error = GA_ERROR::NO_FREE_ELEMENTS;
            res->ErrorMessage = STR_NONE;
            return res;
        }

        uint32_t flags = GetFlags();
        if (!(flags & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_remove_litter(_loc.x, _loc.y, _loc.z);
            wall_remove_at({ _loc.ToTileStart(), _loc.z, _loc.z + 32 });
        }

        uint8_t baseHeight = _loc.z / 8;
        uint8_t clearanceHeight = (_loc.z + 32) / 8;

        auto tileElement = map_get_track_element_at_of_type_from_ride(_loc, TRACK_ELEM_MAZE, _rideIndex);
        if (tileElement == nullptr)
        {
            money32 price = (((RideTrackCosts[ride->type].track_price * TrackPricing[TRACK_ELEM_MAZE]) >> 16));
            res->Cost = price / 2 * 10;

            uint16_t flooredX = floor2(_loc.x, 32);
            uint16_t flooredY = floor2(_loc.y, 32);

            tileElement = tile_element_insert({ _loc.x / 32, _loc.y / 32, baseHeight }, 0b1111);
            assert(tileElement != nullptr);

            tileElement->clearance_height = clearanceHeight;
            tileElement->SetType(TILE_ELEMENT_TYPE_TRACK);

            tileElement->AsTrack()->SetTrackType(TRACK_ELEM_MAZE);
            tileElement->AsTrack()->SetRideIndex(_rideIndex);
            tileElement->AsTrack()->SetMazeEntry(0xFFFF);

            if (flags & GAME_COMMAND_FLAG_GHOST)
            {
                tileElement->SetGhost(true);
            }

            map_invalidate_tile_full({ flooredX, flooredY });

            ride->maze_tiles++;
            ride->stations[0].SetBaseZ(tileElement->GetBaseZ());
            ride->stations[0].Start.x = 0;
            ride->stations[0].Start.y = 0;

            if (_initialPlacement && !(flags & GAME_COMMAND_FLAG_GHOST))
            {
                ride->overall_view.x = flooredX / 32;
                ride->overall_view.y = flooredY / 32;
            }
        }

        switch (_mode)
        {
            case GC_SET_MAZE_TRACK_BUILD:
            {
                uint8_t segmentOffset = MazeGetSegmentBit(_loc.x, _loc.y);

                tileElement->AsTrack()->MazeEntrySubtract(1 << segmentOffset);

                if (!_initialPlacement)
                {
                    segmentOffset = byte_993CE9[(_loc.direction + segmentOffset)];
                    tileElement->AsTrack()->MazeEntrySubtract(1 << segmentOffset);

                    uint8_t temp_edx = byte_993CFC[segmentOffset];
                    if (temp_edx != 0xFF)
                    {
                        uint16_t previousElementX = floor2(_loc.x, 32) - CoordsDirectionDelta[_loc.direction].x;
                        uint16_t previousElementY = floor2(_loc.y, 32) - CoordsDirectionDelta[_loc.direction].y;

                        TileElement* previousTileElement = map_get_track_element_at_of_type_from_ride(
                            { previousElementX, previousElementY, _loc.z }, TRACK_ELEM_MAZE, _rideIndex);

                        if (previousTileElement != nullptr)
                        {
                            previousTileElement->AsTrack()->MazeEntrySubtract(1 << temp_edx);
                        }
                        else
                        {
                            tileElement->AsTrack()->MazeEntryAdd(1 << segmentOffset);
                        }
                    }
                }

                break;
            }

            case GC_SET_MAZE_TRACK_MOVE:
                break;

            case GC_SET_MAZE_TRACK_FILL:
                if (!_initialPlacement)
                {
                    uint16_t previousSegmentX = _loc.x - CoordsDirectionDelta[_loc.direction].x / 2;
                    uint16_t previousSegmentY = _loc.y - CoordsDirectionDelta[_loc.direction].y / 2;

                    tileElement = map_get_track_element_at_of_type_from_ride(
                        { previousSegmentX, previousSegmentY, _loc.z }, TRACK_ELEM_MAZE, _rideIndex);

                    map_invalidate_tile_full(CoordsXY{ previousSegmentX, previousSegmentY }.ToTileStart());
                    if (tileElement == nullptr)
                    {
                        log_error("No surface found");
                        res->Error = GA_ERROR::UNKNOWN;
                        res->ErrorMessage = STR_NONE;
                        return res;
                    }

                    uint32_t segmentBit = MazeGetSegmentBit(previousSegmentX, previousSegmentY);

                    tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                    segmentBit--;
                    tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                    segmentBit = (segmentBit - 4) & 0x0F;
                    tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);
                    segmentBit = (segmentBit + 3) & 0x0F;

                    do
                    {
                        tileElement->AsTrack()->MazeEntryAdd(1 << segmentBit);

                        uint32_t direction1 = byte_993D0C[segmentBit];
                        uint16_t nextElementX = floor2(previousSegmentX, 32) + CoordsDirectionDelta[direction1].x;
                        uint16_t nextElementY = floor2(previousSegmentY, 32) + CoordsDirectionDelta[direction1].y;

                        TileElement* tmp_tileElement = map_get_track_element_at_of_type_from_ride(
                            { nextElementX, nextElementY, _loc.z }, TRACK_ELEM_MAZE, _rideIndex);

                        if (tmp_tileElement != nullptr)
                        {
                            uint8_t edx11 = byte_993CFC[segmentBit];
                            tmp_tileElement->AsTrack()->MazeEntryAdd(1 << (edx11));
                        }

                        segmentBit--;
                    } while ((segmentBit & 0x3) != 0x3);
                }
                break;
        }

        map_invalidate_tile({ _loc.ToTileStart(), tileElement->GetBaseZ(), tileElement->GetClearanceZ() });

        if ((tileElement->AsTrack()->GetMazeEntry() & 0x8888) == 0x8888)
        {
            tile_element_remove(tileElement);
            sub_6CB945(ride);
            ride->maze_tiles--;
        }

        return res;
    }

private:
    uint8_t MazeGetSegmentBit(uint16_t x, uint16_t y) const
    {
        uint8_t minorX = x & 0x1F; // 0 or 16
        uint8_t minorY = y & 0x1F; // 0 or 16

        if (minorX == 0 && minorY == 0)
        {
            return 3;
        }

        if (minorY == 16 && minorX == 16)
        {
            return 11;
        }

        if (minorY == 0)
        {
            return 15;
        }

        return 7;
    }
};
