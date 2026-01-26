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
constexpr RideTypeDescriptor SpiralSlideRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::flatTrack2x2,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::spiralSlide,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::noTestMode, RtdFlag::noVehicles,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::allowMusic,
                     RtdFlag::hasEntranceAndExit, RtdFlag::interestingToLookAt,
                     RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::singleRidePerAdmission, RideMode::unlimitedRidesPerAdmission),
    .DefaultMode = RideMode::singleRidePerAdmission,
    .OperatingSettings = { 1, 5 },
    .Naming = { STR_RIDE_NAME_SPIRAL_SLIDE, STR_RIDE_DESCRIPTION_SPIRAL_SLIDE },
    .NameConvention = { RideComponentType::Train, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 128, 0, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 82.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::brightRed, Drawing::Colour::yellow, Drawing::Colour::darkBrown },
        { Drawing::Colour::lightBlue, Drawing::Colour::brightPink, Drawing::Colour::lightPurple },
        { Drawing::Colour::white, Drawing::Colour::bordeauxRed, Drawing::Colour::grey },
        { Drawing::Colour::beige, Drawing::Colour::saturatedRed, Drawing::Colour::grey },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "spiral_slide",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 50), RideRating::make(1, 40), RideRating::make(0, 90) },
        8,
        2,
        false,
        {
            { RatingsModifierType::BonusSlideUnlimitedRides, 0, RideRating::make(0, 40), RideRating::make(0, 20), RideRating::make(0, 25) },
            { RatingsModifierType::BonusScenery,             0, 25098, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceSpiralSlide,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = updateSpiralSlide,
    .specialType = RtdSpecialType::spiralSlide,
};
} // namespace OpenRCT2
// clang-format on
