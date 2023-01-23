/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
namespace OpenRCT2
{
    struct SourceDescriptor
    {
        const utf8* title;
        uint8_t id;
        uint8_t source;
        int32_t index;
        uint8_t category;
    };

    namespace ScenarioSources
    {
        bool TryGetByName(const utf8* name, SourceDescriptor* outDesc);
        bool TryGetById(uint8_t id, SourceDescriptor* outDesc);
        void NormaliseName(utf8* buffer, size_t bufferSize, const utf8* name);
    } // namespace ScenarioSources

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
        SC_UTOPIA_PARK,
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

        // Competition scenarios (Build the best ... you can)
        SC_STEEL_TWISTER_COMPETITION = 110,
        SC_GO_KARTS_COMPETITION = 114,
        SC_STEEL_CORKSCREW_COMPETITION = 116,
        SC_MINE_TRAIN_COMPETITION = 117,
        SC_STEEL_ROLLER_COASTER_COMPETITION = 119,
        SC_BOBSLED_COMPETITION = 120,
        SC_INVERTED_ROLLER_COASTER_COMPETITION = 123,
        SC_SUSPENDED_ROLLER_COASTER_COMPETITION = 124,
        SC_STAND_UP_STEEL_ROLLER_COASTER_COMPETITION = 125,
        SC_WOODEN_ROLLER_COASTER_COMPETITION = 126,
        SC_STEEL_MINI_ROLLER_COASTER_COMPETITION = 127,
    };
} // namespace OpenRCT2
