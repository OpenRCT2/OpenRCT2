/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Construction.h"

#include <openrct2/actions/RideCreateAction.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideTypes.h>
#include <openrct2/ride/TrackData.h>

using namespace OpenRCT2::TrackMetaData;

/**
 *
 *  rct2: 0x006B4800
 */
void RideConstructNew(RideSelection listItem)
{
    int32_t rideEntryIndex = RideGetEntryIndex(listItem.Type, listItem.EntryIndex);
    int32_t colour1 = RideGetRandomColourPresetIndex(listItem.Type);
    int32_t colour2 = RideGetUnusedPresetVehicleColour(rideEntryIndex);

    auto gameAction = RideCreateAction(listItem.Type, listItem.EntryIndex, colour1, colour2, gLastEntranceStyle);

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
    const Ride& currentRide, uint8_t buildDirection, uint8_t buildSlope, uint8_t buildBank, RideConstructionState state)
{
    auto buildDirectionIsDiagonal = TrackPieceDirectionIsDiagonal(buildDirection);
    SpecialElementsDropdownState list;

    // if it's building neither forwards nor backwards, no list is available
    if (state != RideConstructionState::Front && state != RideConstructionState::Place && state != RideConstructionState::Back)
        return list;

    for (track_type_t trackType : DropdownOrder)
    {
        const auto& ted = GetTrackElementDescriptor(trackType);
        if (!IsTrackEnabled(ted.Definition.type))
            continue;
        bool entryIsDisabled;

        // If the current build orientation (slope, bank, diagonal) matches the track element's, show the piece as enabled
        if (state == RideConstructionState::Back)
        {
            entryIsDisabled = ted.Definition.vangle_end != buildSlope || ted.Definition.bank_end != buildBank
                || TrackPieceDirectionIsDiagonal(ted.Coordinates.rotation_end) != buildDirectionIsDiagonal;
        }
        else
        {
            entryIsDisabled = ted.Definition.vangle_start != buildSlope || ted.Definition.bank_start != buildBank
                || TrackPieceDirectionIsDiagonal(ted.Coordinates.rotation_begin) != buildDirectionIsDiagonal;
        }

        // Only show up half helixes on alpine coaster
        if (currentRide.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_UP_INCLINE_REQUIRES_LIFT)
            && !gCheatsEnableAllDrawableTrackPieces
            && ((
                trackType == TrackElemType::LeftHalfBankedHelixUpSmall
                || trackType == TrackElemType::RightHalfBankedHelixUpSmall
                || trackType == TrackElemType::LeftHalfBankedHelixUpLarge
                || trackType == TrackElemType::RightHalfBankedHelixUpLarge)))
            continue;

        // Additional tower bases can only be built if the ride allows for it (elevator)
        if (trackType == TrackElemType::TowerBase
            && !currentRide.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_EXTRA_TOWER_BASES))
            entryIsDisabled = true;

        // Check if a previous element exists, to collate entries if possible
        if (!list.Elements.empty()
            && GetTrackElementDescriptor(list.Elements.back().TrackType).Description == ted.Description)
        {
            // If the current element is disabled, do not add current element.
            if (entryIsDisabled)
                continue;
            auto& lastElement = list.Elements.back();
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
        list.Elements.push_back({ trackType, entryIsDisabled });
        list.HasActiveElements |= !entryIsDisabled;
    }
    return list;
}
