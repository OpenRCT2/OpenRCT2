/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor FerrisWheelRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack1x4C,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::ferrisWheel,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack,
                     RtdFlag::isFlatRide, RtdFlag::hasVehicleColours, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::slightlyInterestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::ForwardRotation, RideMode::BackwardRotation),
    .DefaultMode = RideMode::ForwardRotation,
    .OperatingSettings = { 1, 3 },
    .Naming = { STR_RIDE_NAME_FERRIS_WHEEL, STR_RIDE_DESCRIPTION_FERRIS_WHEEL },
    .NameConvention = { RideComponentType::Wheel, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 176, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 60, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 112.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_FERRIS_WHEEL_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "ferris_wheel",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(0, 60), MakeRideRating(0, 25), MakeRideRating(0, 30) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusRotations,        0,                25, 25, 25 },
            { RatingsModifierType::BonusScenery,          0,                41831, 0, 0 },
        },
    },
};
// clang-format on
