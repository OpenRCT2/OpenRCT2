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
constexpr RideTypeDescriptor BoatHireRTD =
{
    .Category = RideCategory::water,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::boatHire,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::sBend},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, RtdFlag::trackMustBeOnWater,
                     RtdFlag::noTestMode, RtdFlag::hasLoadOptions, RtdFlag::hasVehicleColours, RtdFlag::checkForStalling,
                     RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit),
    .RideModes = EnumsToFlags(RideMode::boatHire),
    .DefaultMode = RideMode::boatHire,
    .Naming = { STR_RIDE_NAME_BOAT_HIRE, STR_RIDE_DESCRIPTION_BOAT_HIRE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::DockingPlatform, RideComponentType::DockingPlatform },
    .AvailableBreakdowns = (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 16, 0, 3, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 70, 6, 0 },
    .UpkeepCosts = { 50, 1, 0, 4, 0, 0 },
    .BuildCosts = { 27.50_GBP, 0.00_GBP, 5, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { OpenRCT2::Drawing::Colour::mossGreen, OpenRCT2::Drawing::Colour::mossGreen, OpenRCT2::Drawing::Colour::white },
        { OpenRCT2::Drawing::Colour::black, OpenRCT2::Drawing::Colour::black, OpenRCT2::Drawing::Colour::darkGreen },
        { OpenRCT2::Drawing::Colour::lightBrown, OpenRCT2::Drawing::Colour::lightBrown, OpenRCT2::Drawing::Colour::yellow },
        { OpenRCT2::Drawing::Colour::bordeauxRed, OpenRCT2::Drawing::Colour::bordeauxRed, OpenRCT2::Drawing::Colour::bordeauxRed },
        { OpenRCT2::Drawing::Colour::icyBlue, OpenRCT2::Drawing::Colour::icyBlue, OpenRCT2::Drawing::Colour::icyBlue },
        { OpenRCT2::Drawing::Colour::lightBrown, OpenRCT2::Drawing::Colour::lightBrown, OpenRCT2::Drawing::Colour::lightBrown },
        { OpenRCT2::Drawing::Colour::lightBlue, OpenRCT2::Drawing::Colour::lightBlue, OpenRCT2::Drawing::Colour::darkBlue },
        { OpenRCT2::Drawing::Colour::brightPurple, OpenRCT2::Drawing::Colour::brightPurple, OpenRCT2::Drawing::Colour::white },
        { OpenRCT2::Drawing::Colour::brightYellow, OpenRCT2::Drawing::Colour::brightYellow, OpenRCT2::Drawing::Colour::yellow },
        { OpenRCT2::Drawing::Colour::darkWater, OpenRCT2::Drawing::Colour::darkWater, OpenRCT2::Drawing::Colour::darkGreen },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_TRACK, SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "boat_hire",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(1, 90), RideRating::make(0, 80), RideRating::make(0, 90) },
        7,
        0,
        false,
        {
            { RatingsModifierType::BonusBoatHireNoCircuit, 0, RideRating::make(0, 20), 0, 0 },
            { RatingsModifierType::BonusProximity,         0, 11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
    .specialType = RtdSpecialType::boatHire,
};
} // namespace OpenRCT2
// clang-format on
