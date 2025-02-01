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
constexpr RideTypeDescriptor SpiralSlideRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack2x2,
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
    .RideModes = EnumsToFlags(RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission),
    .DefaultMode = RideMode::SingleRidePerAdmission,
    .OperatingSettings = { 1, 5 },
    .Naming = { STR_RIDE_NAME_SPIRAL_SLIDE, STR_RIDE_DESCRIPTION_SPIRAL_SLIDE },
    .NameConvention = { RideComponentType::Train, RideComponentType::Building, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 128, 0, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 82.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_DARK_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_BRIGHT_PINK, COLOUR_LIGHT_PURPLE },
        { COLOUR_WHITE, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_DULL_BROWN_LIGHT, COLOUR_SATURATED_RED, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "spiral_slide",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { MakeRideRating(1, 50), MakeRideRating(1, 40), MakeRideRating(0, 90) },
        8,
        2,
        false,
        {
            { RatingsModifierType::BonusSlideUnlimitedRides, 0, MakeRideRating(0, 40), MakeRideRating(0, 20), MakeRideRating(0, 25) },
            { RatingsModifierType::BonusScenery,             0, 25098, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceSpiralSlide,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = UpdateSpiralSlide,
    .specialType = RtdSpecialType::spiralSlide,
};
// clang-format on
