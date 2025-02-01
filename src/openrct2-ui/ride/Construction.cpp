/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Construction.h"

#include "../interface/Viewport.h"

#include <openrct2/GameState.h>
#include <openrct2/actions/RideCreateAction.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/world/tile_element/TrackElement.h>

using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2
{
    constexpr auto kSeparator = TrackElemType::None;

    /**
     * Order of special track elements dropdown. Elements with the same name string must be sequential or they show up twice.
     */
    constexpr std::array kSpecialElementsDropdownOrder = {
        TrackElemType::EndStation,

        // Brakes
        TrackElemType::Brakes,
        TrackElemType::DiagBrakes,
        TrackElemType::Down25Brakes,
        TrackElemType::BlockBrakes,
        TrackElemType::DiagBlockBrakes,

        // Boosters
        TrackElemType::Booster,
        TrackElemType::DiagBooster,

        // Photo sections
        TrackElemType::OnRidePhoto,

        // Rotation control
        TrackElemType::RotationControlToggle,

        // (Curved) lift (hills) pieces
        TrackElemType::LeftCurvedLiftHill,
        TrackElemType::RightCurvedLiftHill,
        TrackElemType::CableLiftHill,
        TrackElemType::PoweredLift,
        kSeparator,

        // Heart Line pieces
        TrackElemType::HeartLineTransferUp,
        TrackElemType::HeartLineTransferDown,
        TrackElemType::LeftHeartLineRoll,
        TrackElemType::RightHeartLineRoll,
        kSeparator,

        // Brake for drop
        TrackElemType::BrakeForDrop,
        kSeparator,

        // Tower
        TrackElemType::TowerBase,
        TrackElemType::TowerSection,
        kSeparator,

        // Mini Golf pieces
        TrackElemType::MinigolfHoleA,
        TrackElemType::MinigolfHoleB,
        TrackElemType::MinigolfHoleC,
        TrackElemType::MinigolfHoleD,
        TrackElemType::MinigolfHoleE,
        kSeparator,

        // S-Bends
        TrackElemType::SBendLeft,
        TrackElemType::SBendRight,
        kSeparator,

        // Helixes
        TrackElemType::LeftHalfBankedHelixUpSmall,
        TrackElemType::RightHalfBankedHelixUpSmall,
        TrackElemType::LeftHalfBankedHelixDownSmall,
        TrackElemType::RightHalfBankedHelixDownSmall,
        TrackElemType::LeftHalfBankedHelixUpLarge,
        TrackElemType::RightHalfBankedHelixUpLarge,
        TrackElemType::LeftHalfBankedHelixDownLarge,
        TrackElemType::RightHalfBankedHelixDownLarge,
        TrackElemType::LeftQuarterBankedHelixLargeUp,
        TrackElemType::RightQuarterBankedHelixLargeUp,
        TrackElemType::LeftQuarterBankedHelixLargeDown,
        TrackElemType::RightQuarterBankedHelixLargeDown,
        TrackElemType::LeftQuarterHelixLargeUp,
        TrackElemType::RightQuarterHelixLargeUp,
        TrackElemType::LeftQuarterHelixLargeDown,
        TrackElemType::RightQuarterHelixLargeDown,
        kSeparator,

        // (Wooden) water splash
        TrackElemType::Watersplash,
        kSeparator,

        // River Rapids
        TrackElemType::Waterfall,
        TrackElemType::Rapids, // Also used for Monster Trucks
        TrackElemType::Whirlpool,
        kSeparator,

        // Spinning tunnel
        TrackElemType::SpinningTunnel,
        kSeparator,

        // Reverser pieces
        TrackElemType::LeftReverser,
        TrackElemType::RightReverser,
        TrackElemType::LogFlumeReverser,
        kSeparator,

        // Reverse freefall pieces
        TrackElemType::ReverseFreefallSlope,
        TrackElemType::ReverseFreefallVertical,

        // Air thrust pieces
        TrackElemType::AirThrustTopCap,
        TrackElemType::AirThrustVerticalDown,
        TrackElemType::AirThrustVerticalDownToLevel,
        kSeparator,

        // Corkscrews
        TrackElemType::LeftCorkscrewUp,
        TrackElemType::LeftCorkscrewDown,
        TrackElemType::RightCorkscrewUp,
        TrackElemType::RightCorkscrewDown,
        TrackElemType::LeftFlyerCorkscrewUp,
        TrackElemType::LeftFlyerCorkscrewDown,
        TrackElemType::RightFlyerCorkscrewUp,
        TrackElemType::RightFlyerCorkscrewDown,
        TrackElemType::LeftLargeCorkscrewUp,
        TrackElemType::LeftLargeCorkscrewDown,
        TrackElemType::RightLargeCorkscrewUp,
        TrackElemType::RightLargeCorkscrewDown,
        kSeparator,

        // Loops
        TrackElemType::LeftVerticalLoop,
        TrackElemType::RightVerticalLoop,
        TrackElemType::HalfLoopUp,
        TrackElemType::HalfLoopDown,
        TrackElemType::FlyerHalfLoopUninvertedUp,
        TrackElemType::FlyerHalfLoopInvertedDown,
        TrackElemType::FlyerHalfLoopInvertedUp,
        TrackElemType::FlyerHalfLoopUninvertedDown,
        TrackElemType::LeftMediumHalfLoopUp,
        TrackElemType::LeftMediumHalfLoopDown,
        TrackElemType::RightMediumHalfLoopUp,
        TrackElemType::RightMediumHalfLoopDown,
        TrackElemType::LeftLargeHalfLoopUp,
        TrackElemType::LeftLargeHalfLoopDown,
        TrackElemType::RightLargeHalfLoopUp,
        TrackElemType::RightLargeHalfLoopDown,
        TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::LeftFlyerLargeHalfLoopInvertedDown,
        TrackElemType::LeftFlyerLargeHalfLoopInvertedUp,
        TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown,
        TrackElemType::RightFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::RightFlyerLargeHalfLoopInvertedDown,
        TrackElemType::RightFlyerLargeHalfLoopInvertedUp,
        TrackElemType::RightFlyerLargeHalfLoopUninvertedDown,
        TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop,
        TrackElemType::Up90ToInvertedFlatQuarterLoop,
        TrackElemType::InvertedFlatToDown90QuarterLoop,
        TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop,
        TrackElemType::MultiDimFlatToDown90QuarterLoop,
        TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop,
        kSeparator,

        // Zero-G Rolls, Dive Loops
        TrackElemType::LeftZeroGRollUp,
        TrackElemType::LeftZeroGRollDown,
        TrackElemType::RightZeroGRollUp,
        TrackElemType::RightZeroGRollDown,
        TrackElemType::LeftLargeZeroGRollUp,
        TrackElemType::LeftLargeZeroGRollDown,
        TrackElemType::RightLargeZeroGRollUp,
        TrackElemType::RightLargeZeroGRollDown,
        TrackElemType::LeftEighthDiveLoopUpToOrthogonal,
        TrackElemType::LeftEighthDiveLoopDownToDiag,
        TrackElemType::RightEighthDiveLoopUpToOrthogonal,
        TrackElemType::RightEighthDiveLoopDownToDiag,
        kSeparator,

        // Barrel Rolls
        TrackElemType::LeftBarrelRollUpToDown,
        TrackElemType::LeftBarrelRollDownToUp,
        TrackElemType::RightBarrelRollUpToDown,
        TrackElemType::RightBarrelRollDownToUp,
        kSeparator,

        // Twists
        TrackElemType::LeftTwistDownToUp,
        TrackElemType::LeftTwistUpToDown,
        TrackElemType::RightTwistDownToUp,
        TrackElemType::RightTwistUpToDown,
        TrackElemType::LeftFlyerTwistUp,
        TrackElemType::LeftFlyerTwistDown,
        TrackElemType::RightFlyerTwistUp,
        TrackElemType::RightFlyerTwistDown,
    };

    // Update the magic number with the current number of track elements to silence
    static_assert(EnumValue(TrackElemType::Count) == 349, "Reminder to add new track element to special dropdown list");

    /**
     *
     *  rct2: 0x006B4800
     */
    void RideConstructNew(RideSelection listItem)
    {
        int32_t rideEntryIndex = RideGetEntryIndex(listItem.Type, listItem.EntryIndex);
        int32_t colour1 = RideGetRandomColourPresetIndex(listItem.Type);
        int32_t colour2 = RideGetUnusedPresetVehicleColour(rideEntryIndex);

        auto gameAction = RideCreateAction(
            listItem.Type, listItem.EntryIndex, colour1, colour2, GetGameState().LastEntranceStyle);

        gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error != GameActions::Status::Ok)
                return;
            const auto rideIndex = result->GetData<RideId>();
            auto ride = GetRide(rideIndex);
            RideConstructionStart(*ride);
        });

        GameActions::Execute(&gameAction);
    }

    SpecialElementsDropdownState BuildSpecialElementsList(
        const Ride& currentRide, uint8_t buildDirection, TrackPitch buildSlope, TrackRoll buildBank,
        RideConstructionState state)
    {
        auto buildDirectionIsDiagonal = TrackPieceDirectionIsDiagonal(buildDirection);
        SpecialElementsDropdownState list;

        // if it's building neither forwards nor backwards, no list is available
        if (state != RideConstructionState::Front && state != RideConstructionState::Place
            && state != RideConstructionState::Back)
            return list;

        auto& elements = list.Elements;

        for (TrackElemType trackType : kSpecialElementsDropdownOrder)
        {
            const auto& ted = GetTrackElementDescriptor(trackType);
            if (!IsTrackEnabled(ted.definition.group) && trackType != kSeparator)
                continue;

            // If the current build orientation (slope, bank, diagonal) matches the track element's, show the piece as enabled
            bool entryIsDisabled;
            if (state == RideConstructionState::Back)
            {
                entryIsDisabled = ted.definition.pitchEnd != buildSlope || ted.definition.rollEnd != buildBank
                    || TrackPieceDirectionIsDiagonal(ted.coordinates.rotationEnd) != buildDirectionIsDiagonal;
            }
            else
            {
                entryIsDisabled = ted.definition.pitchStart != buildSlope || ted.definition.rollStart != buildBank
                    || TrackPieceDirectionIsDiagonal(ted.coordinates.rotationBegin) != buildDirectionIsDiagonal;
            }

            // Additional tower bases can only be built if the ride allows for it (elevator)
            if (trackType == TrackElemType::TowerBase
                && !currentRide.GetRideTypeDescriptor().HasFlag(RtdFlag::allowExtraTowerBases))
                entryIsDisabled = true;

            // Check if a previous element exists, to collate entries if possible
            if (!list.Elements.empty() && GetTrackElementDescriptor(elements.back().TrackType).description == ted.description)
            {
                // If the current element is disabled, do not add current element.
                if (entryIsDisabled)
                    continue;

                auto& lastElement = elements.back();

                // Don't add two separators in a row
                if (lastElement.TrackType == kSeparator && trackType == kSeparator)
                    continue;

                // If the previous element is disabled and current element is enabled, replace the previous element
                if (lastElement.Disabled && !entryIsDisabled)
                {
                    lastElement.TrackType = trackType;
                    lastElement.Disabled = false;
                    list.HasActiveElements = true;
                    continue;
                }
                // If the previous element and current element are enabled, add both to the list
            }
            elements.push_back({ trackType, entryIsDisabled });
            list.HasActiveElements |= !entryIsDisabled && trackType != kSeparator;
        }

        if (elements.empty())
            return list;

        // If the very first element is a separator, remove it
        auto firstElement = elements.begin();
        if (firstElement->TrackType == kSeparator)
        {
            elements.erase(firstElement);
            if (elements.empty())
                return list;
        }

        // If the very last element is a separator, remove it
        auto& lastElement = elements.back();
        if (lastElement.TrackType == kSeparator)
            elements.pop_back();

        if (elements.size() < 20)
        {
            list.PreferredNumRows = elements.size();
            return list;
        }

        list.PreferredNumRows = elements.size() / 2 + 1;

        // Scan ahead of the halfway mark to see if there's a separator nearby
        for (size_t i = 0; i < 3; i++)
        {
            if (list.PreferredNumRows + i > elements.size())
                break;

            auto trackPiece = elements[list.PreferredNumRows + i].TrackType;
            if (trackPiece == TrackElemType::None)
            {
                list.PreferredNumRows += i + 1;
                break;
            }
        }

        // If the last element of the first column is a separator, remove it
        auto& middleElement = elements[list.PreferredNumRows - 1];
        if (middleElement.TrackType == kSeparator)
        {
            elements.erase(elements.begin() + list.PreferredNumRows - 1);
            list.PreferredNumRows -= 1;
        }

        return list;
    }

    /**
     *
     *  rct2: 0x006CCF70
     */
    CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords)
    {
        CoordsXYZD entranceExitCoords{};
        gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
        // determine if the mouse is hovering over a station - that's the station to add the entrance to
        auto info = GetMapCoordinatesFromPos(screenCoords, EnumsToFlags(ViewportInteractionItem::Ride));
        if (info.interactionType != ViewportInteractionItem::None)
        {
            if (info.Element->GetType() == TileElementType::Track)
            {
                const auto* trackElement = info.Element->AsTrack();
                if (trackElement->GetRideIndex() == gRideEntranceExitPlaceRideIndex)
                {
                    const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
                    if (ted.sequences[0].flags & TRACK_SEQUENCE_FLAG_ORIGIN)
                    {
                        if (trackElement->GetTrackType() == TrackElemType::Maze)
                        {
                            gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
                        }
                        else
                        {
                            gRideEntranceExitPlaceStationIndex = trackElement->GetStationIndex();
                        }
                    }
                }
            }
        }

        auto ride = GetRide(gRideEntranceExitPlaceRideIndex);
        if (ride == nullptr)
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        auto stationBaseZ = ride->GetStation(gRideEntranceExitPlaceStationIndex).GetBaseZ();

        auto coordsAtHeight = ScreenGetMapXYWithZ(screenCoords, stationBaseZ);
        if (!coordsAtHeight.has_value())
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        entranceExitCoords = { coordsAtHeight->ToTileStart(), stationBaseZ, INVALID_DIRECTION };

        if (ride->type == kRideTypeNull)
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        auto stationStart = ride->GetStation(gRideEntranceExitPlaceStationIndex).Start;
        if (stationStart.IsNull())
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        // find the quadrant the mouse is hovering over - that's the direction to start searching for a station TileElement
        Direction startDirection = 0;
        auto mapX = (coordsAtHeight->x & 0x1F) - 16;
        auto mapY = (coordsAtHeight->y & 0x1F) - 16;
        if (std::abs(mapX) < std::abs(mapY))
        {
            startDirection = mapY < 0 ? 3 : 1;
        }
        else
        {
            startDirection = mapX < 0 ? 0 : 2;
        }
        // check all 4 directions, starting with the mouse's quadrant
        for (uint8_t directionIncrement = 0; directionIncrement < 4; directionIncrement++)
        {
            entranceExitCoords.direction = (startDirection + directionIncrement) & 3;
            // search for TrackElement one tile over, shifted in the search direction
            auto nextLocation = entranceExitCoords;
            nextLocation += CoordsDirectionDelta[entranceExitCoords.direction];
            if (MapIsLocationValid(nextLocation))
            {
                // iterate over every element in the tile until we find what we want
                auto* tileElement = MapGetFirstElementAt(nextLocation);
                if (tileElement == nullptr)
                    continue;
                do
                {
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;
                    if (tileElement->GetBaseZ() != stationBaseZ)
                        continue;
                    auto* trackElement = tileElement->AsTrack();
                    if (trackElement->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                        continue;
                    if (trackElement->GetTrackType() == TrackElemType::Maze)
                    {
                        // if it's a maze, it can place the entrance and exit immediately
                        entranceExitCoords.direction = DirectionReverse(entranceExitCoords.direction);
                        gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                        return entranceExitCoords;
                    }
                    // if it's not a maze, the sequence properties for the TrackElement must be found to determine if an
                    // entrance can be placed on that side

                    gRideEntranceExitPlaceStationIndex = trackElement->GetStationIndex();

                    // get the ride entrance's side relative to the TrackElement
                    Direction direction = (DirectionReverse(entranceExitCoords.direction) - tileElement->GetDirection()) & 3;
                    const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
                    if (ted.sequences[trackElement->GetSequenceIndex()].flags & (1 << direction))
                    {
                        // if that side of the TrackElement supports stations, the ride entrance is valid and faces away from
                        // the station
                        entranceExitCoords.direction = DirectionReverse(entranceExitCoords.direction);
                        gRideEntranceExitPlaceDirection = entranceExitCoords.direction;
                        return entranceExitCoords;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
        }
        gRideEntranceExitPlaceDirection = INVALID_DIRECTION;
        return entranceExitCoords;
    }

} // namespace OpenRCT2
