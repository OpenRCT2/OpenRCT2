/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor ReverseFreefallCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, { TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL_STEEP, TRACK_REVERSE_FREEFALL, TRACK_ON_RIDE_PHOTO}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionReverseFreefallRC),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::LimPoweredLaunch)),
    SET_FIELD(DefaultMode, RideMode::LimPoweredLaunch),
    SET_FIELD(OperatingSettings, { 7, 30, 30, 40, 40, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_REVERSE_FREEFALL_COASTER, STR_RIDE_DESCRIPTION_REVERSE_FREEFALL_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_REVERSE_FREEFALL_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 255, 32, 4, 7, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsMultipliers, { 44, 66, 10 }),
    SET_FIELD(UpkeepCosts, { 80, 20, 0, 0, 0, 10 }),
    SET_FIELD(BuildCosts, { 100.00_GBP, 0.00_GBP, 45, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 70),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_YELLOW, COLOUR_DARK_GREEN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_YELLOW },
        { COLOUR_GREY, COLOUR_SALMON_PINK, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "reverse_freefall_rc"),
    SET_FIELD(RatingsData,
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 00), RIDE_RATING(3, 20), RIDE_RATING(2, 80) },
        25,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             327, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 60), RIDE_RATING(0, 15), 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                436906, 436906, 320398 },
            { RatingsModifierType::BonusGForces,          0,                24576, 41704, 59578 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 28398, 11702 },
            { RatingsModifierType::BonusReversedTrains,   0,                2, 10, 25 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                11155, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 34,               2, 2, 2 },
        },
    }),
};
// clang-format on
