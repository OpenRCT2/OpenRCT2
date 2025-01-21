/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
// C++ does not allow names to start with a number, hence the absence of the '3D' prefix.
constexpr RideTypeDescriptor CinemaRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::_3DCinema,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::describeAsInside, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::MouseTails3DFilm, RideMode::StormChasers3DFilm, RideMode::SpaceRaiders3DFilm),
    .DefaultMode = RideMode::MouseTails3DFilm,
    .Naming = { STR_RIDE_NAME_3D_CINEMA, STR_RIDE_DESCRIPTION_3D_CINEMA },
    .NameConvention = { RideComponentType::Building, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 128, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 20, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 70.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "3d_cinema",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { 1, 1, 1 },
        21,
        7,
        false,
        {
            { RatingsModifierType::Bonus3DCinemaMode, 0, 0, 0, 0 },
        },
    },
};
// clang-format on
