/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor BoatHireRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_WATER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_CURVE_VERY_SMALL}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_boat_hire),
    SET_FIELD(Flags, {
                RideTypeFlags::HasTrackColourMain,
                RideTypeFlags::HasTrackColourSupports,
                RideTypeFlags::TrackMustBeOnWater,
                RideTypeFlags::NoTestMode,
                RideTypeFlags::HasLoadOptions,
                RideTypeFlags::HasVehicleColours,
                RideTypeFlags::CheckForStalling,
                RideTypeFlags::HasTrack,
                RideTypeFlags::SupportsMultipleTrackColour,
                RideTypeFlags::AllowMusic,
                RideTypeFlags::HasEntranceExit } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::BoatHire)),
    SET_FIELD(DefaultMode, RideMode::BoatHire),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_BOAT_HIRE, STR_RIDE_DESCRIPTION_BOAT_HIRE }),
    SET_FIELD(NameConvention, { RideComponentType::Boat, RideComponentType::DockingPlatform, RideComponentType::DockingPlatform }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_BOAT_HIRE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 255, 16, 0, 3, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_boat_hire),
    SET_FIELD(RatingsMultipliers, { 70, 6, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 4, 0, 0 }),
    SET_FIELD(BuildCosts, { 27.50_GBP, 0.00_GBP, 5, }),
    SET_FIELD(DefaultPrices, { 10, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_WATER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 40),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DARK_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_YELLOW },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_DARK_BLUE },
        { COLOUR_BRIGHT_PURPLE, COLOUR_BRIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_YELLOW },
        { COLOUR_DARK_WATER, COLOUR_DARK_WATER, COLOUR_DARK_GREEN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_TRACK, SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "boat_hire"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, LightFxAddLightsMagicVehicle_BoatHire),
};
// clang-format on
