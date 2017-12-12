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

#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "ScenarioSources.h"

#include "scenario.h"

namespace ScenarioSources
{
    struct ScenarioAlias
    {
        const utf8 * Original;
        const utf8 * Alternative;
    };

    struct ScenarioTitleDescriptor
    {
        const uint8  Id;
        const utf8 * Title;
        const uint8  Category;
        const bool   FirstInCategory;
    };

    #pragma region Scenario Data

    static const ScenarioAlias ScenarioAliases[] =
    {
        // UK - US differences:
        { "Katie's Dreamland",                      "Katie's World"                             },
        { "Pokey Park",                             "Dinky Park"                                },
        { "White Water Park",                       "Aqua Park"                                 },
        { "Mystic Mountain",                        "Mothball Mountain"                         },
        { "Paradise Pier",                          "Big Pier"                                  },
        { "Paradise Pier 2",                        "Big Pier 2"                                },
        { "Haunted Harbour",                        "Haunted Harbor"                            },
        { "Mythological - Cradle of Civilisation",  "Mythological - Cradle of Civilization"     },

        // RCT1 pack by RCTScenarioLover has a mistake:
        { "Geoffrey Gardens",                       "Geoffery Gardens"                          },
    };

    // RCT
    static const ScenarioTitleDescriptor ScenarioTitlesRCT1[] =
    {
        { SC_FOREST_FRONTIERS,          "Forest Frontiers",     SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_DYNAMITE_DUNES,            "Dynamite Dunes",       SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_LEAFY_LAKE,                "Leafy Lake",           SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_DIAMOND_HEIGHTS,           "Diamond Heights",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_EVERGREEN_GARDENS,         "Evergreen Gardens",    SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_BUMBLY_BEACH,              "Bumbly Beach",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_TRINITY_ISLANDS,           "Trinity Islands",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_KATIES_DREAMLAND,          "Katie's Dreamland",    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_POKEY_PARK,                "Pokey Park",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_WHITE_WATER_PARK,          "White Water Park",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MILLENNIUM_MINES,          "Millennium Mines",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_KARTS_COASTERS,            "Karts & Coasters",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MELS_WORLD,                "Mel's World",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MYSTIC_MOUNTAIN,           "Mystic Mountain",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PACIFIC_PYRAMIDS,          "Pacific Pyramids",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_CRUMBLY_WOODS,             "Crumbly Woods",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PARADISE_PIER,             "Paradise Pier",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_LIGHTNING_PEAKS,           "Lightning Peaks",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_IVORY_TOWERS,              "Ivory Towers",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_RAINBOW_VALLEY,            "Rainbow Valley",       SCENARIO_CATEGORY_EXPERT,        false },
        { SC_THUNDER_ROCK,              "Thunder Rock",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MEGA_PARK,                 "Mega Park",            SCENARIO_CATEGORY_OTHER,         false },
    };

    // RCT: Added Attractions
    static const ScenarioTitleDescriptor ScenarioTitlesRCT1AA[] =
    {
        { SC_WHISPERING_CLIFFS,         "Whispering Cliffs",    SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_THREE_MONKEYS_PARK,        "Three Monkeys Park",   SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_CANARY_MINES,              "Canary Mines",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_BARONY_BRIDGE,             "Barony Bridge",        SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUNTOPIA,                  "Funtopia",             SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_HAUNTED_HARBOUR,           "Haunted Harbour",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUN_FORTRESS,              "Fun Fortress",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUTURE_WORLD,              "Future World",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_GENTLE_GLEN,               "Gentle Glen",          SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_JOLLY_JUNGLE,              "Jolly Jungle",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_HYDRO_HILLS,               "Hydro Hills",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SPRIGHTLY_PARK,            "Sprightly Park",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MAGIC_QUARTERS,            "Magic Quarters",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_FRUIT_FARM,                "Fruit Farm",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_BUTTERFLY_DAM,             "Butterfly Dam",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_COASTER_CANYON,            "Coaster Canyon",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_THUNDERSTORM_PARK,         "Thunderstorm Park",    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_HARMONIC_HILLS,            "Harmonic Hills",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ROMAN_VILLAGE,             "Roman Village",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SWAMP_COVE,                "Swamp Cove",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ADRENALINE_HEIGHTS,        "Adrenaline Heights",   SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UTOPIA,                    "Utopia",               SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ROTTING_HEIGHTS,           "Rotting Heights",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_FIASCO_FOREST,             "Fiasco Forest",        SCENARIO_CATEGORY_EXPERT,        false },
        { SC_PICKLE_PARK,               "Pickle Park",          SCENARIO_CATEGORY_EXPERT,        false },
        { SC_GIGGLE_DOWNS,              "Giggle Downs",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MINERAL_PARK,              "Mineral Park",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_COASTER_CRAZY,             "Coaster Crazy",        SCENARIO_CATEGORY_EXPERT,        false },
        { SC_URBAN_PARK,                "Urban Park",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_GEOFFREY_GARDENS,          "Geoffrey Gardens",     SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT: Loopy Landscapes
    static const ScenarioTitleDescriptor ScenarioTitlesRCT1LL[] =
    {
        {   SC_ICEBERG_ISLANDS,         "Iceberg Islands",      SCENARIO_CATEGORY_BEGINNER,      true  },
        {   SC_VOLCANIA,                "Volcania",             SCENARIO_CATEGORY_BEGINNER,      false },
        {   SC_ARID_HEIGHTS,            "Arid Heights",         SCENARIO_CATEGORY_BEGINNER,      false },
        {   SC_RAZOR_ROCKS,             "Razor Rocks",          SCENARIO_CATEGORY_BEGINNER,      false },
        {   SC_CRATER_LAKE,             "Crater Lake",          SCENARIO_CATEGORY_BEGINNER,      false },
        {   SC_VERTIGO_VIEWS,           "Vertigo Views",        SCENARIO_CATEGORY_BEGINNER,      false },
        {   SC_PARADISE_PIER_2,         "Paradise Pier 2",      SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_DRAGONS_COVE,            "Dragon's Cove",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_GOOD_KNIGHT_PARK,        "Good Knight Park",     SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_WACKY_WARREN,            "Wacky Warren",         SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_GRAND_GLACIER,           "Grand Glacier",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_CRAZY_CRATERS,           "Crazy Craters",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_DUSTY_DESERT,            "Dusty Desert",         SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_WOODWORM_PARK,           "Woodworm Park",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_ICARUS_PARK,             "Icarus Park",          SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_SUNNY_SWAMPS,            "Sunny Swamps",         SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_FRIGHTMARE_HILLS,        "Frightmare Hills",     SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_THUNDER_ROCKS,           "Thunder Rocks",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_OCTAGON_PARK,            "Octagon Park",         SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_PLEASURE_ISLAND,         "Pleasure Island",      SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_ICICLE_WORLDS,           "Icicle Worlds",        SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_SOUTHERN_SANDS,          "Southern Sands",       SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_TINY_TOWERS,             "Tiny Towers",          SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_NEVERMORE_PARK,          "Nevermore Park",       SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_PACIFICA,                "Pacifica",             SCENARIO_CATEGORY_CHALLENGING,   false },
        {   SC_URBAN_JUNGLE,            "Urban Jungle",         SCENARIO_CATEGORY_EXPERT,        false },
        {   SC_TERROR_TOWN,             "Terror Town",          SCENARIO_CATEGORY_EXPERT,        false },
        {   SC_MEGAWORLD_PARK,          "Megaworld Park",       SCENARIO_CATEGORY_EXPERT,        false },
        {   SC_VENUS_PONDS,             "Venus Ponds",          SCENARIO_CATEGORY_EXPERT,        false },
        {   SC_MICRO_PARK,              "Micro Park",           SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2[] =
    {
        { SC_UNIDENTIFIED,              "Crazy Castle",         SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Electric Fields",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Factory Capers",       SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Amity Airfield",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Botany Breakers",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Bumbly Bazaar",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Dusty Greens",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Fungus Woods",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Gravity Gardens",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Infernal Views",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Alpine Adventures",    SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Extreme Heights",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Ghost Town",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Lucky Lake",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Rainbow Summit",       SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2: Wacky Worlds
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2WW[] =
    {
        { SC_UNIDENTIFIED,              "Africa - Victoria Falls",                          SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Asia - Great Wall of China Tourism Enhancement",   SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "North America - Grand Canyon",                     SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "South America - Rio Carnival",                     SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Africa - African Diamond Mine",                    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Asia - Maharaja Palace",                           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Australasia - Ayers Rock",                         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Europe - European Cultural Festival",              SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "North America - Rollercoaster Heaven",             SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "South America - Inca Lost City",                   SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Africa - Oasis",                                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Antarctic - Ecological Salvage",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Asia - Japanese Coastal Reclaim",                  SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Australasia - Fun at the Beach",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Europe - Renovation",                              SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "N. America - Extreme Hawaiian Island",             SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "South America - Rain Forest Plateau",              SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2: Time Twister
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2TT[] =
    {
        { SC_UNIDENTIFIED,              "Dark Age - Robin Hood",                            SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Prehistoric - After the Asteroid",                 SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Prison Island",                 SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Flower Power",                     SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Dark Age - Castle",                                SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Future - First Encounters",                        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Mythological - Animatronic Film Set",              SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Prehistoric - Jurassic Safari",                    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Schneider Cup",                 SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Future - Future World",                            SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Mythological - Cradle of Civilisation",            SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Prehistoric - Stone Age",                          SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Skyscrapers",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Rock 'n' Roll",                    SCENARIO_CATEGORY_EXPERT,        false },
    };

    // Real parks
    static const ScenarioTitleDescriptor ScenarioTitlesRealParks[] =
    {
        { SC_ALTON_TOWERS,              "Alton Towers",                                     SCENARIO_CATEGORY_REAL,    false },
        { SC_HEIDE_PARK,                "Heide-Park",                                       SCENARIO_CATEGORY_REAL,    false },
        { SC_BLACKPOOL_PLEASURE_BEACH,  "Blackpool Pleasure Beach",                         SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Belgium",                                SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Great Adventure",                        SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Holland",                                SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Magic Mountain",                         SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags over Texas",                             SCENARIO_CATEGORY_REAL,    false },
    };

    // Other parks
    static const ScenarioTitleDescriptor ScenarioTitlesOtherParks[] =
    {
        { SC_FORT_ANACHRONISM,          "Fort Anachronism",                                 SCENARIO_CATEGORY_DLC,              false },
        { SC_PCPLAYER,                  "PC Player",                                        SCENARIO_CATEGORY_DLC,              false },
        { SC_PCGW,                      "PC Gaming World",                                  SCENARIO_CATEGORY_DLC,              false },
        { SC_GAMEPLAY,                  "gameplay",                                         SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Panda World",                                      SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Competition Land 1",                               SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Competition Land 2",                               SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Belgium",                 SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Great Adventure",         SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Holland",                 SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Magic Mountain",          SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Park",                    SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags over Texas",              SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
    };

    #define DEFINE_SCENARIO_TITLE_DESC_GROUP(x) { Util::CountOf(x), x }
    const struct {
        size_t count;
        const ScenarioTitleDescriptor * const titles;
    } ScenarioTitlesBySource[] = {
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT1),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT1AA),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT1LL),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT2),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT2WW),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRCT2TT),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRealParks),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesOtherParks),
    };

    #pragma endregion

    bool TryGetByName(const utf8 * name, source_desc * outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        sint32 currentIndex = 0;
        for (size_t i = 0; i < Util::CountOf(ScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < ScenarioTitlesBySource[i].count; j++)
            {
                const ScenarioTitleDescriptor *desc = &ScenarioTitlesBySource[i].titles[j];
                if (String::Equals(name, desc->Title, true))
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = (uint8)i;
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    outDesc->first_in_category = desc->FirstInCategory;
                    return true;
                }
                currentIndex++;
            }
        }

        outDesc->title = nullptr;
        outDesc->id = SC_UNIDENTIFIED;
        outDesc->source = SCENARIO_SOURCE_OTHER;
        outDesc->index = -1;
        outDesc->category = SCENARIO_CATEGORY_OTHER;
        outDesc->first_in_category = false;
        return false;
    }

    bool TryGetById(uint8 id, source_desc * outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        sint32 currentIndex = 0;
        for (size_t i = 0; i < Util::CountOf(ScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < ScenarioTitlesBySource[i].count; j++)
            {
                const ScenarioTitleDescriptor * desc = &ScenarioTitlesBySource[i].titles[j];
                if (id == desc->Id)
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = (uint8)i;
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    return true;
                }
                currentIndex++;
            }
        }

        outDesc->title = nullptr;
        outDesc->id = SC_UNIDENTIFIED;
        outDesc->source = SCENARIO_SOURCE_OTHER;
        outDesc->index = -1;
        outDesc->category = SCENARIO_CATEGORY_OTHER;
        return false;
    }

    void NormaliseName(utf8 * buffer, size_t bufferSize, const utf8 * name)
    {
        size_t nameLength = String::LengthOf(name);

        // Strip "RCT(1|2)?" prefix off scenario names.
        if (nameLength >= 3 && (name[0] == 'R' && name[1] == 'C' && name[2] == 'T'))
        {
            if (nameLength >= 4 && (name[3] == '1' || name[3] == '2'))
            {
                log_verbose("Stripping RCT/1/2 from name: %s", name);
                String::Set(buffer, bufferSize, name + 4);
            }
            else
            {
                String::Set(buffer, bufferSize, name + 3);
            }
        }

        // Trim (for the sake of the above and WW / TT scenarios
        String::TrimStart(buffer, bufferSize, name);

        // American scenario titles should be converted to British name
        // Don't worry, names will be translated using language packs later
        for (const ScenarioAlias &alias : ScenarioAliases)
        {
            if (String::Equals(alias.Alternative, name))
            {
                log_verbose("Found alias: %s; will treat as: %s", name, alias.Original);
                String::Set(buffer, bufferSize, alias.Original);
            }
        }
    }
}

extern "C"
{
    bool scenario_get_source_desc(const utf8 * name, source_desc * outDesc)
    {
        return ScenarioSources::TryGetByName(name, outDesc);
    }

    bool scenario_get_source_desc_by_id(uint8 id, source_desc * outDesc)
    {
        return ScenarioSources::TryGetById(id, outDesc);
    }

    void scenario_normalise_name(utf8 * buffer, size_t bufferSize, utf8 * name)
    {
        ScenarioSources::NormaliseName(buffer, bufferSize, name);
    }
}
