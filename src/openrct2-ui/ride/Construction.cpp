/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/config/Config.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/tile_element/TrackElement.h>

using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2
{
    constexpr auto kSeparator = TrackElemType::none;

    /**
     * Order of special track elements dropdown. Elements with the same name string must be sequential or they show up twice.
     */
    constexpr std::array kSpecialElementsDropdownOrder = {
        TrackElemType::endStation,

        // Brakes
        TrackElemType::brakes,
        TrackElemType::diagBrakes,
        TrackElemType::down25Brakes,
        TrackElemType::diagDown25Brakes,
        TrackElemType::blockBrakes,
        TrackElemType::diagBlockBrakes,

        // Boosters
        TrackElemType::booster,
        TrackElemType::diagBooster,

        // Photo sections
        TrackElemType::onRidePhoto,

        // Rotation control
        TrackElemType::rotationControlToggle,

        // (Curved) lift (hills) pieces
        TrackElemType::leftCurvedLiftHill,
        TrackElemType::rightCurvedLiftHill,
        TrackElemType::cableLiftHill,
        TrackElemType::poweredLift,
        kSeparator,

        // Heart Line pieces
        TrackElemType::heartLineTransferUp,
        TrackElemType::heartLineTransferDown,
        TrackElemType::leftHeartLineRoll,
        TrackElemType::rightHeartLineRoll,
        kSeparator,

        // Brake for drop
        TrackElemType::brakeForDrop,
        kSeparator,

        // Tower
        TrackElemType::towerBase,
        TrackElemType::towerSection,
        kSeparator,

        // Mini Golf pieces
        TrackElemType::minigolfHoleA,
        TrackElemType::minigolfHoleB,
        TrackElemType::minigolfHoleC,
        TrackElemType::minigolfHoleD,
        TrackElemType::minigolfHoleE,
        kSeparator,

        // S-Bends
        TrackElemType::sBendLeft,
        TrackElemType::sBendRight,
        kSeparator,

        // Helixes
        TrackElemType::leftHalfBankedHelixUpSmall,
        TrackElemType::rightHalfBankedHelixUpSmall,
        TrackElemType::leftHalfBankedHelixDownSmall,
        TrackElemType::rightHalfBankedHelixDownSmall,
        TrackElemType::leftHalfBankedHelixUpLarge,
        TrackElemType::rightHalfBankedHelixUpLarge,
        TrackElemType::leftHalfBankedHelixDownLarge,
        TrackElemType::rightHalfBankedHelixDownLarge,
        TrackElemType::leftQuarterBankedHelixLargeUp,
        TrackElemType::rightQuarterBankedHelixLargeUp,
        TrackElemType::leftQuarterBankedHelixLargeDown,
        TrackElemType::rightQuarterBankedHelixLargeDown,
        TrackElemType::leftQuarterHelixLargeUp,
        TrackElemType::rightQuarterHelixLargeUp,
        TrackElemType::leftQuarterHelixLargeDown,
        TrackElemType::rightQuarterHelixLargeDown,
        kSeparator,

        // (Wooden) water splash
        TrackElemType::watersplash,
        kSeparator,

        // River Rapids
        TrackElemType::waterfall,
        TrackElemType::rapids, // Also used for Monster Trucks
        TrackElemType::whirlpool,
        kSeparator,

        // Spinning tunnel
        TrackElemType::spinningTunnel,
        kSeparator,

        // Reverser pieces
        TrackElemType::leftReverser,
        TrackElemType::rightReverser,
        TrackElemType::logFlumeReverser,
        kSeparator,

        // Reverse freefall pieces
        TrackElemType::reverseFreefallSlope,
        TrackElemType::reverseFreefallVertical,

        // Air thrust pieces
        TrackElemType::airThrustTopCap,
        TrackElemType::airThrustVerticalDown,
        TrackElemType::airThrustVerticalDownToLevel,
        kSeparator,

        // Corkscrews
        TrackElemType::leftCorkscrewUp,
        TrackElemType::leftCorkscrewDown,
        TrackElemType::rightCorkscrewUp,
        TrackElemType::rightCorkscrewDown,
        TrackElemType::leftFlyerCorkscrewUp,
        TrackElemType::leftFlyerCorkscrewDown,
        TrackElemType::rightFlyerCorkscrewUp,
        TrackElemType::rightFlyerCorkscrewDown,
        TrackElemType::leftLargeCorkscrewUp,
        TrackElemType::leftLargeCorkscrewDown,
        TrackElemType::rightLargeCorkscrewUp,
        TrackElemType::rightLargeCorkscrewDown,
        kSeparator,

        // Loops
        TrackElemType::leftVerticalLoop,
        TrackElemType::rightVerticalLoop,
        TrackElemType::halfLoopUp,
        TrackElemType::halfLoopDown,
        TrackElemType::flyerHalfLoopUninvertedUp,
        TrackElemType::flyerHalfLoopInvertedDown,
        TrackElemType::flyerHalfLoopInvertedUp,
        TrackElemType::flyerHalfLoopUninvertedDown,
        TrackElemType::leftMediumHalfLoopUp,
        TrackElemType::leftMediumHalfLoopDown,
        TrackElemType::rightMediumHalfLoopUp,
        TrackElemType::rightMediumHalfLoopDown,
        TrackElemType::leftLargeHalfLoopUp,
        TrackElemType::leftLargeHalfLoopDown,
        TrackElemType::rightLargeHalfLoopUp,
        TrackElemType::rightLargeHalfLoopDown,
        TrackElemType::leftFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::leftFlyerLargeHalfLoopInvertedDown,
        TrackElemType::leftFlyerLargeHalfLoopInvertedUp,
        TrackElemType::leftFlyerLargeHalfLoopUninvertedDown,
        TrackElemType::rightFlyerLargeHalfLoopUninvertedUp,
        TrackElemType::rightFlyerLargeHalfLoopInvertedDown,
        TrackElemType::rightFlyerLargeHalfLoopInvertedUp,
        TrackElemType::rightFlyerLargeHalfLoopUninvertedDown,
        TrackElemType::multiDimInvertedFlatToDown90QuarterLoop,
        TrackElemType::up90ToInvertedFlatQuarterLoop,
        TrackElemType::invertedFlatToDown90QuarterLoop,
        TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop,
        TrackElemType::multiDimFlatToDown90QuarterLoop,
        TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop,
        kSeparator,

        // Zero-G Rolls, Dive Loops
        TrackElemType::leftZeroGRollUp,
        TrackElemType::leftZeroGRollDown,
        TrackElemType::rightZeroGRollUp,
        TrackElemType::rightZeroGRollDown,
        TrackElemType::leftLargeZeroGRollUp,
        TrackElemType::leftLargeZeroGRollDown,
        TrackElemType::rightLargeZeroGRollUp,
        TrackElemType::rightLargeZeroGRollDown,
        TrackElemType::leftEighthDiveLoopUpToOrthogonal,
        TrackElemType::leftEighthDiveLoopDownToDiag,
        TrackElemType::rightEighthDiveLoopUpToOrthogonal,
        TrackElemType::rightEighthDiveLoopDownToDiag,
        kSeparator,

        // Barrel Rolls
        TrackElemType::leftBarrelRollUpToDown,
        TrackElemType::leftBarrelRollDownToUp,
        TrackElemType::rightBarrelRollUpToDown,
        TrackElemType::rightBarrelRollDownToUp,
        kSeparator,

        // Twists
        TrackElemType::leftTwistDownToUp,
        TrackElemType::leftTwistUpToDown,
        TrackElemType::rightTwistDownToUp,
        TrackElemType::rightTwistUpToDown,
        TrackElemType::leftFlyerTwistUp,
        TrackElemType::leftFlyerTwistDown,
        TrackElemType::rightFlyerTwistUp,
        TrackElemType::rightFlyerTwistDown,
    };

    // Update the magic number with the current number of track elements to silence
    static_assert(EnumValue(TrackElemType::count) == 350, "Reminder to add new track element to special dropdown list");

    /**
     *
     *  rct2: 0x006B4800
     */
    void RideConstructNew(RideSelection listItem)
    {
        int32_t rideEntryIndex = RideGetEntryIndex(listItem.Type, listItem.EntryIndex);
        int32_t colour1 = RideGetRandomColourPresetIndex(listItem.Type);
        int32_t colour2 = RideGetUnusedPresetVehicleColour(rideEntryIndex);

        auto gameAction = GameActions::RideCreateAction(
            listItem.Type, listItem.EntryIndex, colour1, colour2, getGameState().lastEntranceStyle,
            Config::Get().general.defaultInspectionInterval);

        gameAction.SetCallback([](const GameActions::GameAction* ga, const GameActions::Result* result) {
            if (result->error != GameActions::Status::ok)
                return;
            const auto rideIndex = result->getData<RideId>();
            auto ride = GetRide(rideIndex);
            RideConstructionStart(*ride);
        });

        GameActions::Execute(&gameAction, getGameState());
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
            if (trackType == TrackElemType::towerBase
                && !currentRide.getRideTypeDescriptor().HasFlag(RtdFlag::allowExtraTowerBases))
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
            if (trackPiece == TrackElemType::none)
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
        gRideEntranceExitPlaceDirection = kInvalidDirection;
        // determine if the mouse is hovering over a station - that's the station to add the entrance to
        auto info = GetMapCoordinatesFromPos(screenCoords, EnumsToFlags(ViewportInteractionItem::ride));
        if (info.interactionType != ViewportInteractionItem::none)
        {
            if (info.Element->GetType() == TileElementType::Track)
            {
                const auto* trackElement = info.Element->AsTrack();
                if (trackElement->GetRideIndex() == gRideEntranceExitPlaceRideIndex)
                {
                    const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
                    if (ted.sequences[0].flags.has(SequenceFlag::trackOrigin))
                    {
                        if (trackElement->GetTrackType() == TrackElemType::maze)
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

        auto stationBaseZ = ride->getStation(gRideEntranceExitPlaceStationIndex).GetBaseZ();

        auto coordsAtHeight = ScreenGetMapXYWithZ(screenCoords, stationBaseZ);
        if (!coordsAtHeight.has_value())
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        entranceExitCoords = { coordsAtHeight->ToTileStart(), stationBaseZ, kInvalidDirection };

        if (ride->type == kRideTypeNull)
        {
            entranceExitCoords.SetNull();
            return entranceExitCoords;
        }

        auto stationStart = ride->getStation(gRideEntranceExitPlaceStationIndex).Start;
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
                    if (trackElement->GetTrackType() == TrackElemType::maze)
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
                    auto connectionSides = ted.sequences[trackElement->GetSequenceIndex()].getEntranceConnectionSides();
                    if (connectionSides & (1 << direction))
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
        gRideEntranceExitPlaceDirection = kInvalidDirection;
        return entranceExitCoords;
    }

} // namespace OpenRCT2
