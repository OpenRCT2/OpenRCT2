/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor BoatHireRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::boatHire,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::sBend},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, RtdFlag::trackMustBeOnWater,
                     RtdFlag::noTestMode, RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours, RtdFlag::checkForStalling,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit),
    .RideModes = EnumsToFlags(RideMode::BoatHire),
    .DefaultMode = RideMode::BoatHire,
    .Naming = { STR_RIDE_NAME_BOAT_HIRE, STR_RIDE_DESCRIPTION_BOAT_HIRE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::DockingPlatform, RideComponentType::DockingPlatform },
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 16, 0, 3, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, 0 },
    .UpkeepCosts = { 50, 1, 0, 4, 0, 0 },
    .BuildCosts = { 27.50_GBP, 0.00_GBP, 5, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DARK_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_YELLOW },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
        { COLOUR_ICY_BLUE, COLOUR_ICY_BLUE, COLOUR_ICY_BLUE },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_DARK_BLUE },
        { COLOUR_BRIGHT_PURPLE, COLOUR_BRIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_YELLOW },
        { COLOUR_DARK_WATER, COLOUR_DARK_WATER, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_TRACK, SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "boat_hire",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 90), MakeRideRating(0, 80), MakeRideRating(0, 90) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusBoatHireNoCircuit, 0, MakeRideRating(0, 20), 0, 0 },
            { RatingsModifierType::BonusProximity,         0, 11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
// clang-format on
