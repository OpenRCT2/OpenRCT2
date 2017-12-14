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
        { SC_FOREST_FRONTIERS,          "Forest Frontiers",     "Forest Frontiers",     SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_DYNAMITE_DUNES,            "Dynamite Dunes",       "Dynamite Dunes",       SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_LEAFY_LAKE,                "Leafy Lake",           "Leafy Lake",           SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_DIAMOND_HEIGHTS,           "Diamond Heights",      "Diamond Heights",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_EVERGREEN_GARDENS,         "Evergreen Gardens",    "Evergreen Gardens",    SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_BUMBLY_BEACH,              "Bumbly Beach",         "Bumbly Beach",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_TRINITY_ISLANDS,           "Trinity Islands",      "Trinity Islands",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_KATIES_DREAMLAND,          "Katie's Dreamland",    "Katie's Dreamland",    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_POKEY_PARK,                "Pokey Park",           "Pokey Park",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_WHITE_WATER_PARK,          "White Water Park",     "White Water Park",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MILLENNIUM_MINES,          "Millennium Mines",     "Millennium Mines",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_KARTS_COASTERS,            "Karts & Coasters",     "Karts & Coasters",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MELS_WORLD,                "Mel's World",          "Mel's World",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MYSTIC_MOUNTAIN,           "Mystic Mountain",      "Mystic Mountain",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PACIFIC_PYRAMIDS,          "Pacific Pyramids",     "Pacific Pyramids",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_CRUMBLY_WOODS,             "Crumbly Woods",        "Crumbly Woods",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PARADISE_PIER,             "Paradise Pier",        "Paradise Pier",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_LIGHTNING_PEAKS,           "Lightning Peaks",      "Lightning Peaks",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_IVORY_TOWERS,              "Ivory Towers",         "Ivory Towers",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_RAINBOW_VALLEY,            "Rainbow Valley",       "Rainbow Valley",       SCENARIO_CATEGORY_EXPERT,        false },
        { SC_THUNDER_ROCK,              "Thunder Rock",         "Thunder Rock",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MEGA_PARK,                 "Mega Park",            "Mega Park",            SCENARIO_CATEGORY_OTHER,         false },
    };

    // RCT: Added Attractions
    static const ScenarioTitleDescriptor ScenarioTitlesRCT1AA[] =
    {
        { SC_WHISPERING_CLIFFS,         "Whispering Cliffs",    "Whispering Cliffs",    SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_THREE_MONKEYS_PARK,        "Three Monkeys Park",   "Three Monkeys Park",   SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_CANARY_MINES,              "Canary Mines",         "Canary Mines",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_BARONY_BRIDGE,             "Barony Bridge",        "Barony Bridge",        SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUNTOPIA,                  "Funtopia",             "Funtopia",             SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_HAUNTED_HARBOUR,           "Haunted Harbour",      "Haunted Harbour",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUN_FORTRESS,              "Fun Fortress",         "Fun Fortress",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_FUTURE_WORLD,              "Future World",         "Future World",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_GENTLE_GLEN,               "Gentle Glen",          "Gentle Glen",          SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_JOLLY_JUNGLE,              "Jolly Jungle",         "Jolly Jungle",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_HYDRO_HILLS,               "Hydro Hills",          "Hydro Hills",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SPRIGHTLY_PARK,            "Sprightly Park",       "Sprightly Park",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_MAGIC_QUARTERS,            "Magic Quarters",       "Magic Quarters",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_FRUIT_FARM,                "Fruit Farm",           "Fruit Farm",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_BUTTERFLY_DAM,             "Butterfly Dam",        "Butterfly Dam",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_COASTER_CANYON,            "Coaster Canyon",       "Coaster Canyon",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_THUNDERSTORM_PARK,         "Thunderstorm Park",    "Thunderstorm Park",    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_HARMONIC_HILLS,            "Harmonic Hills",       "Harmonic Hills",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ROMAN_VILLAGE,             "Roman Village",        "Roman Village",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SWAMP_COVE,                "Swamp Cove",           "Swamp Cove",           SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ADRENALINE_HEIGHTS,        "Adrenaline Heights",   "Adrenaline Heights",   SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UTOPIA,                    "Utopia",               "Utopia",               SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ROTTING_HEIGHTS,           "Rotting Heights",      "Rotting Heights",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_FIASCO_FOREST,             "Fiasco Forest",        "Fiasco Forest",        SCENARIO_CATEGORY_EXPERT,        false },
        { SC_PICKLE_PARK,               "Pickle Park",          "Pickle Park",          SCENARIO_CATEGORY_EXPERT,        false },
        { SC_GIGGLE_DOWNS,              "Giggle Downs",         "Giggle Downs",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MINERAL_PARK,              "Mineral Park",         "Mineral Park",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_COASTER_CRAZY,             "Coaster Crazy",        "Coaster Crazy",        SCENARIO_CATEGORY_EXPERT,        false },
        { SC_URBAN_PARK,                "Urban Park",           "Urban Park",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_GEOFFREY_GARDENS,          "Geoffrey Gardens",     "Geoffrey Gardens",     SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT: Loopy Landscapes
    static const ScenarioTitleDescriptor ScenarioTitlesRCT1LL[] =
    {
        { SC_ICEBERG_ISLANDS,           "Iceberg Islands",      "Iceberg Islands",      SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_VOLCANIA,                  "Volcania",             "Volcania",             SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_ARID_HEIGHTS,              "Arid Heights",         "Arid Heights",         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_RAZOR_ROCKS,               "Razor Rocks",          "Razor Rocks",          SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_CRATER_LAKE,               "Crater Lake",          "Crater Lake",          SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_VERTIGO_VIEWS,             "Vertigo Views",        "Vertigo Views",        SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_PARADISE_PIER_2,           "Paradise Pier 2",      "Paradise Pier 2",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_DRAGONS_COVE,              "Dragon's Cove",        "Dragon's Cove",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_GOOD_KNIGHT_PARK,          "Good Knight Park",     "Good Knight Park",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_WACKY_WARREN,              "Wacky Warren",         "Wacky Warren",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_GRAND_GLACIER,             "Grand Glacier",        "Grand Glacier",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_CRAZY_CRATERS,             "Crazy Craters",        "Crazy Craters",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_DUSTY_DESERT,              "Dusty Desert",         "Dusty Desert",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_WOODWORM_PARK,             "Woodworm Park",        "Woodworm Park",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ICARUS_PARK,               "Icarus Park",          "Icarus Park",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SUNNY_SWAMPS,              "Sunny Swamps",         "Sunny Swamps",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_FRIGHTMARE_HILLS,          "Frightmare Hills",     "Frightmare Hills",     SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_THUNDER_ROCKS,             "Thunder Rocks",        "Thunder Rocks",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_OCTAGON_PARK,              "Octagon Park",         "Octagon Park",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PLEASURE_ISLAND,           "Pleasure Island",      "Pleasure Island",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_ICICLE_WORLDS,             "Icicle Worlds",        "Icicle Worlds",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_SOUTHERN_SANDS,            "Southern Sands",       "Southern Sands",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_TINY_TOWERS,               "Tiny Towers",          "Tiny Towers",          SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_NEVERMORE_PARK,            "Nevermore Park",       "Nevermore Park",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_PACIFICA,                  "Pacifica",             "Pacifica",             SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_URBAN_JUNGLE,              "Urban Jungle",         "Urban Jungle",         SCENARIO_CATEGORY_EXPERT,        false },
        { SC_TERROR_TOWN,               "Terror Town",          "Terror Town",          SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MEGAWORLD_PARK,            "Megaworld Park",       "Megaworld Park",       SCENARIO_CATEGORY_EXPERT,        false },
        { SC_VENUS_PONDS,               "Venus Ponds",          "Venus Ponds",          SCENARIO_CATEGORY_EXPERT,        false },
        { SC_MICRO_PARK,                "Micro Park",           "Micro Park",           SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2[] =
    {
        { SC_UNIDENTIFIED,              "Crazy Castle",         "Crazy Castle",         SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Electric Fields",      "Electric Fields",      SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Factory Capers",       "Factory Capers",       SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Amity Airfield",       "Amity Airfield",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Botany Breakers",      "Botany Breakers",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Bumbly Bazaar",        "Bumbly Bazaar",        SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Dusty Greens",         "Dusty Greens",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Fungus Woods",         "Fungus Woods",         SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Gravity Gardens",      "Gravity Gardens",      SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Infernal Views",       "Infernal Views",       SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Alpine Adventures",    "Alpine Adventures",    SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Extreme Heights",      "Extreme Heights",      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Ghost Town",           "Ghost Town",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Lucky Lake",           "Lucky Lake",           SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Rainbow Summit",       "Rainbow Summit",       SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2: Wacky Worlds
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2WW[] =
    {
        { SC_UNIDENTIFIED,              "Africa - Victoria Falls",                          "Over The Edge",                    SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Asia - Great Wall of China Tourism Enhancement",   "Great Wall of China",              SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "North America - Grand Canyon",                     "Canyon Calamities",                SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "South America - Rio Carnival",                     "Sugarloaf Shores",                 SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Africa - African Diamond Mine",                    "Mines of Africa",                  SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Asia - Maharaja Palace",                           "Park Maharaja",                    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Australasia - Ayers Rock",                         "Ayers Adventure",                  SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Europe - European Cultural Festival",              "European Extravaganza",            SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "North America - Rollercoaster Heaven",             "Rollercoaster Heaven",             SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "South America - Inca Lost City",                   "Lost City Founder",                SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Africa - Oasis",                                   "Mirage Madness",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Antarctic - Ecological Salvage",                   "Icy Adventures",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Asia - Japanese Coastal Reclaim",                  "Okinawa Coast",                    SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Australasia - Fun at the Beach",                   "Beach Barbecue Blast",             SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Europe - Renovation",                              "From The Ashes",                   SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "N. America - Extreme Hawaiian Island",             "Wacky Waikiki",                    SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "South America - Rain Forest Plateau",              "Rainforest Romp",                  SCENARIO_CATEGORY_EXPERT,        false },
    };

    // RCT2: Time Twister
    static const ScenarioTitleDescriptor ScenarioTitlesRCT2TT[] =
    {
        { SC_UNIDENTIFIED,              "Dark Age - Robin Hood",                            "Sherwood Forest",                  SCENARIO_CATEGORY_BEGINNER,      true  },
        { SC_UNIDENTIFIED,              "Prehistoric - After the Asteroid",                 "Crater Carnage",                   SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Prison Island",                 "Alcatraz",                         SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Flower Power",                     "Woodstock",                        SCENARIO_CATEGORY_BEGINNER,      false },
        { SC_UNIDENTIFIED,              "Dark Age - Castle",                                "Cliffside Castle",                 SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Future - First Encounters",                        "Extraterrestrial Extravaganza",    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Mythological - Animatronic Film Set",              "Animatronic Antics",               SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Prehistoric - Jurassic Safari",                    "Coastersaurus",                    SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Schneider Cup",                 "Schneider Shores",                 SCENARIO_CATEGORY_CHALLENGING,   false },
        { SC_UNIDENTIFIED,              "Future - Future World",                            "Gemini City",                      SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Mythological - Cradle of Civilisation",            "Mythological Madness",             SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Prehistoric - Stone Age",                          "Rocky Rambles",                    SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Skyscrapers",                   "Metropolis",                       SCENARIO_CATEGORY_EXPERT,        false },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Rock 'n' Roll",                    "Rock 'n' Roll Revival",            SCENARIO_CATEGORY_EXPERT,        false },
    };

    // Real parks
    static const ScenarioTitleDescriptor ScenarioTitlesRealParks[] =
    {
        { SC_ALTON_TOWERS,              "Alton Towers",                                     "Alton Towers",                     SCENARIO_CATEGORY_REAL,    false },
        { SC_HEIDE_PARK,                "Heide-Park",                                       "Heide-Park",                       SCENARIO_CATEGORY_REAL,    false },
        { SC_BLACKPOOL_PLEASURE_BEACH,  "Blackpool Pleasure Beach",                         "Blackpool Pleasure Beach",         SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Belgium",                                "Six Flags Belgium",                SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Great Adventure",                        "Six Flags Great Adventure",        SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Holland",                                "Six Flags Holland",                SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags Magic Mountain",                         "Six Flags Magic Mountain",         SCENARIO_CATEGORY_REAL,    false },
        { SC_UNIDENTIFIED,              "Six Flags over Texas",                             "Six Flags over Texas",             SCENARIO_CATEGORY_REAL,    false },
    };

    // Other parks
    static const ScenarioTitleDescriptor ScenarioTitlesOtherParks[] =
    {
        { SC_FORT_ANACHRONISM,          "Fort Anachronism",                                 "Fort Anachronism",                 SCENARIO_CATEGORY_DLC,              false },
        { SC_PCPLAYER,                  "PC Player",                                        "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_PCGW,                      "PC Gaming World",                                  "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_GAMEPLAY,                  "gameplay",                                         "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Panda World",                                      "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Competition Land 1",                               "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Competition Land 2",                               "???",                              SCENARIO_CATEGORY_DLC,              false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Belgium",                 "Six Flags Belgium",                SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Great Adventure",         "Six Flags Great Adventure",        SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Holland",                 "Six Flags Holland",                SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Magic Mountain",          "Six Flags Magic Mountain",         SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags Park",                    "Six Flags",                        SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
        { SC_UNIDENTIFIED,              "Build your own Six Flags over Texas",              "Six Flags over Texas",             SCENARIO_CATEGORY_BUILD_YOUR_OWN,   false },
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
                    outDesc->park_name = desc->ParkName;
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
        outDesc->park_name = nullptr;
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
                    outDesc->park_name = desc->ParkName;
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
        outDesc->park_name = nullptr;
        outDesc->id = SC_UNIDENTIFIED;
        outDesc->source = SCENARIO_SOURCE_OTHER;
        outDesc->index = -1;
        outDesc->category = SCENARIO_CATEGORY_OTHER;
        outDesc->first_in_category = false;
        return false;
    }

    utf8 * GetNextScenarioPathFromGroup(uint8 source, sint32 index) {
        if (source < 0 || source >= SCENARIO_SOURCE_COUNT)
            return nullptr;

        switch (source) {
        case SCENARIO_SOURCE_RCT1:
            return ScenarioTitlesRCT[index]->path;
            break;
        case SCENARIO_SOURCE_RCT1_AA:
            return ScenarioTitlesRCT1AA[index]->path;
            break;
        case SCENARIO_SOURCE_RCT1_LL:
            return ScenarioTitlesRCT1LL[index]->path;
            break;
        case SCENARIO_SOURCE_RCT2:
            return ScenarioTitlesRCT2[index]->path;
            break;
        case SCENARIO_SOURCE_RCT2_WW:
            return ScenarioTitlesRCT2WW[index]->path;
            break;
        case SCENARIO_SOURCE_RCT2_TT:
            return ScenarioTitlesRCT2TT[index]->path;
            break;
        case SCENARIO_SOURCE_REAL:
            return ScenarioTitlesRealParks[index]->path;
            break;
        case SCENARIO_SOURCE_OTHER:
            return ScenarioTitlesOtherParks[index]->path;
            break;
        }
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
