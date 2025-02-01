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
constexpr RideTypeDescriptor ReverseFreefallCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::reverseFreefallCoaster,
        .supportType = WoodenSupportType::Truss,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHillSteep, TrackGroup::reverseFreefall, TrackGroup::onridePhoto},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt
        | EnumsToFlags(RtdFlag::allowReversedTrains, RtdFlag::hasLsmBehaviourOnFlat),
    .RideModes = EnumsToFlags(RideMode::LimPoweredLaunch),
    .DefaultMode = RideMode::LimPoweredLaunch,
    .OperatingSettings = { 7, 30 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 40, 40 },
    .LegacyBoosterSettings = { 40, 40 },
    .Naming = { STR_RIDE_NAME_REVERSE_FREEFALL_COASTER, STR_RIDE_DESCRIPTION_REVERSE_FREEFALL_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 4, 7, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 44, 66, 10 },
    .UpkeepCosts = { 80, 20, 0, 0, 0, 10 },
    .BuildCosts = { 100.00_GBP, 0.00_GBP, 45, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_YELLOW, COLOUR_DARK_GREEN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_YELLOW },
        { COLOUR_GREY, COLOUR_SALMON_PINK, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "reverse_freefall_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 00), MakeRideRating(3, 20), MakeRideRating(2, 80) },
        25,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             327, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 60), MakeRideRating(0, 15), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                436906, 436906, 320398 },
            { RatingsModifierType::BonusGForces,          0,                24576, 41704, 59578 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 28398, 11702 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 10, 25 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 34,               2, 2, 2 },
        },
    },
};
// clang-format on
