#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

typedef struct source_desc
{
    const utf8 * title;
    uint8        id;
    uint8        source;
    sint32       index;
    uint8        category;
} source_desc;

#ifdef __cplusplus

namespace ScenarioSources
{
    bool TryGetByName(const utf8 * name, source_desc * outDesc);
    bool TryGetById(uint8 id, source_desc * outDesc);
    void NormaliseName(utf8 * buffer, size_t bufferSize, const utf8 * name);
}

#endif

#ifdef __cplusplus
extern "C"
{
#endif

    bool scenario_get_source_desc(const utf8 *name, source_desc *outDesc);
    bool scenario_get_source_desc_by_id(uint8 id, source_desc *outDesc);
    void scenario_normalise_name(utf8 *buffer, size_t bufferSize, utf8 *name);

#ifdef __cplusplus
}
#endif

// RCT1 scenario index map
enum
{
    SC_UNIDENTIFIED = 255,

    // RCT
    SC_FOREST_FRONTIERS = 0,
    SC_DYNAMITE_DUNES,
    SC_LEAFY_LAKE,
    SC_DIAMOND_HEIGHTS,
    SC_EVERGREEN_GARDENS,
    SC_BUMBLY_BEACH,
    SC_TRINITY_ISLANDS,
    SC_KATIES_DREAMLAND,
    SC_POKEY_PARK,
    SC_WHITE_WATER_PARK,
    SC_MILLENNIUM_MINES,
    SC_KARTS_COASTERS,
    SC_MELS_WORLD,
    SC_MYSTIC_MOUNTAIN,
    SC_PACIFIC_PYRAMIDS,
    SC_CRUMBLY_WOODS,
    SC_PARADISE_PIER,
    SC_LIGHTNING_PEAKS,
    SC_IVORY_TOWERS,
    SC_RAINBOW_VALLEY,
    SC_THUNDER_ROCK,
    SC_MEGA_PARK,

    // Loopy Landscapes
    SC_ICEBERG_ISLANDS,
    SC_VOLCANIA,
    SC_ARID_HEIGHTS,
    SC_RAZOR_ROCKS,
    SC_CRATER_LAKE,
    SC_VERTIGO_VIEWS,
    SC_PARADISE_PIER_2,
    SC_DRAGONS_COVE,
    SC_GOOD_KNIGHT_PARK,
    SC_WACKY_WARREN,

    // Special
    SC_ALTON_TOWERS,
    SC_FORT_ANACHRONISM,

    // Added Attractions
    SC_WHISPERING_CLIFFS = 40,
    SC_THREE_MONKEYS_PARK,
    SC_CANARY_MINES,
    SC_BARONY_BRIDGE,
    SC_FUNTOPIA,
    SC_HAUNTED_HARBOUR,
    SC_FUN_FORTRESS,
    SC_FUTURE_WORLD,
    SC_GENTLE_GLEN,
    SC_JOLLY_JUNGLE,
    SC_HYDRO_HILLS,
    SC_SPRIGHTLY_PARK,
    SC_MAGIC_QUARTERS,
    SC_FRUIT_FARM,
    SC_BUTTERFLY_DAM,
    SC_COASTER_CANYON,
    SC_THUNDERSTORM_PARK,
    SC_HARMONIC_HILLS,
    SC_ROMAN_VILLAGE,
    SC_SWAMP_COVE,
    SC_ADRENALINE_HEIGHTS,
    SC_UTOPIA,
    SC_ROTTING_HEIGHTS,
    SC_FIASCO_FOREST,
    SC_PICKLE_PARK,
    SC_GIGGLE_DOWNS,
    SC_MINERAL_PARK,
    SC_COASTER_CRAZY,
    SC_URBAN_PARK,
    SC_GEOFFREY_GARDENS,

    // Special
    SC_HEIDE_PARK,
    SC_PCPLAYER,
    SC_PCGW,
    SC_GAMEPLAY,
    SC_BLACKPOOL_PLEASURE_BEACH,

    // Loopy Landscapes
    SC_GRAND_GLACIER = 80,
    SC_CRAZY_CRATERS,
    SC_DUSTY_DESERT,
    SC_WOODWORM_PARK,
    SC_ICARUS_PARK,
    SC_SUNNY_SWAMPS,
    SC_FRIGHTMARE_HILLS,
    SC_THUNDER_ROCKS,
    SC_OCTAGON_PARK,
    SC_PLEASURE_ISLAND,
    SC_ICICLE_WORLDS,
    SC_SOUTHERN_SANDS,
    SC_TINY_TOWERS,
    SC_NEVERMORE_PARK,
    SC_PACIFICA,
    SC_URBAN_JUNGLE,
    SC_TERROR_TOWN,
    SC_MEGAWORLD_PARK,
    SC_VENUS_PONDS,
    SC_MICRO_PARK,
};
