/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor kSpiralSlideRTD =
{
    .AlternateType = kRideTypeNull,
    .Category = RIDE_CATEGORY_GENTLE,
    .EnabledTrackPieces = {},
    .ExtraTrackPieces = {},
    .CoveredTrackPieces = {},
    .StartTrackPiece = TrackElemType::kFlatTrack2x2,
    .TrackPaintFunctions = TrackDrawerDescriptor(GetTrackPaintFunctionSpiralSlide),
    .Flags = kRideTypeFagsTrackHas3Colours | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION |
                     RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_NO_VEHICLES |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_ALLOW_MUSIC |
                     RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT |
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY | RIDE_TYPE_FLAG_IS_SPIRAL_SLIDE,
    .RideModes = EnumsToFlags(RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission),
    .DefaultMode = RideMode::SingleRidePerAdmission,
    .OperatingSettings = { 1, 5, 0, 0, 0, 0 },
    .Naming = { STR_RIDE_NAME_SPIRAL_SLIDE, STR_RIDE_DESCRIPTION_SPIRAL_SLIDE },
    .NameConvention = { RideComponentType::Train, RideComponentType::Building, RideComponentType::Station },
    .kEnumName = nameof(RIDE_TYPE_SPIRAL_SLIDE),
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
        { RIDE_RATING(1, 50), RIDE_RATING(1, 40), RIDE_RATING(0, 90) },
        8,
        2,
        false,
        {
            { RatingsModifierType::BonusSlideUnlimitedRides, 0, RIDE_RATING(0, 40), RIDE_RATING(0, 20), RIDE_RATING(0, 25) },
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
};
// clang-format on
