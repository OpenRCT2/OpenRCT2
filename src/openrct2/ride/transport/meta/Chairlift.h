/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor ChairliftRTD =
{
    .Category = RIDE_CATEGORY_TRANSPORT,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionChairlift,
        .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS | RIDE_TYPE_FLAG_TRANSPORT_RIDE | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER |
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT | RIDE_TYPE_FLAG_IS_SUSPENDED,
    .RideModes = EnumsToFlags(RideMode::StationToStation),
    .DefaultMode = RideMode::StationToStation,
    .OperatingSettings = { 1, 4 },
    .Naming = { STR_RIDE_NAME_CHAIRLIFT, STR_RIDE_DESCRIPTION_CHAIRLIFT },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .EnumName = nameof(RIDE_TYPE_CHAIRLIFT),
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 40, 32, 28, 2, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 10, 0 },
    .UpkeepCosts = { 60, 20, 0, 4, 3, 10 },
    .BuildCosts = { 32.50_GBP, 0.50_GBP, 30, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = MUSIC_OBJECT_SUMMER,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_OLIVE_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_WHITE },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_TRACK, SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "chairlift",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(1, 60), RIDE_RATING(0, 40), RIDE_RATING(0, 50) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,            6000,     764, 0, 0 },
            { RatingsModifierType::BonusTrainLength,       0,        187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,          0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,      0,        291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,          150,      26214, 0, 0 },
            { RatingsModifierType::BonusTurns,             0,        7430, 3476, 4574 },
            { RatingsModifierType::BonusSheltered,         0,        -19275, 21845, 23405 },
            { RatingsModifierType::BonusProximity,         0,        11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0,        25098, 0, 0 },
            { RatingsModifierType::RequirementLength,      0x960000, 2, 2, 2 },
            { RatingsModifierType::RequirementStations,    1,        0, 2, 1 },
            { RatingsModifierType::RequirementUnsheltered, 4,        4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_ChairLift,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = UpdateChairlift,
};
// clang-format on
