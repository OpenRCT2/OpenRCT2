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
#include "../../../drawing/LightFX.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor GoKartsRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::goKarts,
        .supportType = WoodenSupportType::truss,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::slopeCurve, TrackGroup::slopeCurveLarge, TrackGroup::sBend, TrackGroup::diagSlope},
        .extraTrackGroups = {TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeSteepLong, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, RtdFlag::noTestMode, RtdFlag::hasOneStation,
                     RtdFlag::noWallsAroundTrack, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::race, RideMode::continuousCircuit),
    .DefaultMode = RideMode::race,
    .OperatingSettings = { 1, 10 },
    .Naming = { STR_RIDE_NAME_GO_KARTS, STR_RIDE_DESCRIPTION_GO_KARTS },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 8, 24, 2, 1, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 120, 20, 0 },
    .UpkeepCosts = { 50, 20, 0, 8, 0, 0 },
    .BuildCosts = { 31.00_GBP, 2.00_GBP, 20, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::darkBrown },
        { Drawing::Colour::yellow, Drawing::Colour::yellow, Drawing::Colour::bordeauxRed },
        { Drawing::Colour::darkGreen, Drawing::Colour::darkGreen, Drawing::Colour::darkGreen },
        { Drawing::Colour::darkBrown, Drawing::Colour::darkBrown, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "go_karts",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(1, 42), RideRating::make(1, 73), RideRating::make(0, 40) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,            700, 32768, 0, 0 },
            { RatingsModifierType::BonusGoKartRace,        4,   RideRating::make(1, 40), RideRating::make(0, 50), 0 },
            { RatingsModifierType::BonusTurns,             0,   4458, 3476, 5718 },
            { RatingsModifierType::BonusDrops,             0,   8738, 5461, 6553 },
            { RatingsModifierType::BonusSheltered,         0,   2570, 8738, 2340 },
            { RatingsModifierType::BonusProximity,         0,   11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,   16732, 0, 0 },
            { RatingsModifierType::RequirementUnsheltered, 6,   2, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
} // namespace OpenRCT2
// clang-format on
