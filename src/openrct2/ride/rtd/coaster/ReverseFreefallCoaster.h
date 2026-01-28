/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor ReverseFreefallCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::reverseFreefallCoaster,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHillSteep, TrackGroup::reverseFreefall, TrackGroup::onridePhoto},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt
        | EnumsToFlags(RtdFlag::allowReversedTrains, RtdFlag::hasLsmBehaviourOnFlat),
    .RideModes = EnumsToFlags(RideMode::limPoweredLaunch),
    .DefaultMode = RideMode::limPoweredLaunch,
    .OperatingSettings = { 7, 30 },
    .TrackSpeedSettings = { 60, 60 },
    .BoosterSettings = { 40, 40 },
    .LegacyBoosterSettings = { 40, 40 },
    .Naming = { STR_RIDE_NAME_REVERSE_FREEFALL_COASTER, STR_RIDE_DESCRIPTION_REVERSE_FREEFALL_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 4, 7, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 44, 66, 10 },
    .UpkeepCosts = { 80, 20, 0, 0, 0, 10 },
    .BuildCosts = { 100.00_GBP, 0.00_GBP, 45, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkGreen, Drawing::Colour::yellow, Drawing::Colour::darkGreen },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::yellow },
        { Drawing::Colour::grey, Drawing::Colour::salmonPink, Drawing::Colour::grey },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "reverse_freefall_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(2, 00), RideRating::make(3, 20), RideRating::make(2, 80) },
        25,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             327, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 60), RideRating::make(0, 15), 0 },
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
} // namespace OpenRCT2
// clang-format on
