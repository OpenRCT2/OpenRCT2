/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioSources.h"

#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "Scenario.h"
#include "ScenarioRepository.h"

#include <iterator>

namespace ScenarioSources
{
    struct ScenarioAlias
    {
        const utf8* Original;
        const utf8* Alternative;
    };

    struct ScenarioTitleDescriptor
    {
        const uint8_t Id;
        const utf8* Title;
        const uint8_t Category;
    };

#pragma region Scenario Data

    // clang-format off
    static constexpr const ScenarioAlias ScenarioAliases[] =
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
        // RCT1 pack by Crappage uses park name to identify Utopia Park, instead of scenario list name:
        { "Utopia Park",                            "Utopia"                                    },

        // CD Projekt Polish Edition
        { "Alpine Adventures",                          "Górska przygoda"                           },
        { "Amity Airfield",                             "Lotnisko Pogięcie"                         },
        { "Botany Breakers",                            "Wyspa Odludna"                             },
        { "Bumbly Bazaar",                              "Gwarne targowisko"                         },
        { "Crazy Castle",                               "Zwariowany zamek"                          },
        { "Dusty Greens",                               "Zapiaszczone Dołki"                        },
        { "Electric Fields",                            "Pola elektryczne"                          },
        { "Extreme Heights",                            "Ekstremalne wysokości"                     },
        { "Factory Capers",                             "300% normy"                                },
        { "Fungus Woods",                               "Grzybowe Lasy"                             },
        { "Ghost Town",                                 "Miasto-widmo"                              },
        { "Gravity Gardens",                            "Ogrody Grawitacji"                         },
        { "Infernal Views",                             "Piekielny widok"                           },
        { "Lucky Lake",                                 "Jezioro Szczęściarzy"                      },
        { "Rainbow Summit",                             "Tęczowy Szczyt"                            },

        { "Six Flags Belgium",                          "\"Sześć Flag\" - Belgia"                   },
        { "Six Flags Holland",                          "\"Sześć Flag\" - Holandia"                 },
        { "Six Flags Magic Mountain",                   "Czarodziejska góra \"Sześciu Flag\""       },
        { "Six Flags Great Adventure",                  "\"Sześć Flag\" - wielka przygoda"          },
        { "Six Flags over Texas",                       "\"Sześć Flag nad Teksasem\""             },
        { "Build your own Six Flags Belgium",           "Wybuduj własny park \"Sześć Flag\" - Belgia" },
        { "Build your own Six Flags Holland",           "Wybuduj własny park \"Sześć Flag\" - Holandia" },
        { "Build your own Six Flags Magic Mountain",    "Wybuduj własny park czarodziejska góra \"Sześciu Flag\"" },
        { "Build your own Six Flags Great Adventure",   "Wybuduj własny park \"Sześć Flag\" - wielka przygoda" },
        { "Build your own Six Flags over Texas",        "Wybuduj własny park \"Sześć Flag nad Teksasem\"" },
        { "Build your own Six Flags Park",              "Wybuduj własny park \"Sześć Flag\""        },

        { "Africa - African Diamond Mine",              "Afryka - kopalnia diamentów"               },
        { "Africa - Oasis",                             "Afryka - oaza"                             },
        { "Africa - Victoria Falls",                    "Afryka - Wodospad Wiktorii"                },
        { "Antarctic - Ecological Salvage",             "Antarktyka - utylizacja"                   },
        { "Asia - Great Wall of China Tourism Enhancement", "Azja - rozwój turystyki wokół Wielkiego Muru Chińskiego" },
        { "Asia - Japanese Coastal Reclaim",            "Azja - odzysk japońskiego wybrzeża"        },
        { "Asia - Maharaja Palace",                     "Azja - pałac maharadży"                    },
        { "Australasia - Ayers Rock",                   "Australazja - Ayers Rock"                  },
        { "Australasia - Fun at the beach",             "Australazja - zabawa na plaży"             },
        { "Europe - European Cultural Festival",        "Europa - Europejski Festiwal Kulturalny"   },
        { "Europe - Renovation",                        "Europa - odbudowa"                         },
        { "N. America - Extreme Hawaiian Island",       "Ameryka Płn. - ekstremalne Hawaje"         },
        { "North America - Grand Canyon",               "Ameryka Płn. - Wielki Kanion"              },
        { "North America - Rollercoaster Heaven",       "Ameryka Płn. - raj kolejek górskich"       },
        { "South America - Inca Lost City",             "Ameryka Płd. - zaginione miasto Inków"     },
        { "South America - Rain Forest Plateau",        "Ameryka Płd. - tropikalny płaskowyż"       },
        { "South America - Rio Carnival",               "Ameryka Płd. - karnawał w Rio"             },

        { "Dark Age - Castle",                          "Mroczne Wieki - zamek"                     },
        { "Dark Age - Robin Hood",                      "Mroczne Wieki - Robin Hood"                },
        { "Future - First Encounters",                  "Przyszłość - pierwsze spotkania"           },
        { "Future - Future World",                      "Przyszłość - świat przyszłości"            },
        { "Mythological - Animatronic Film Set",        "Mitologia - mechaniczny plan filmowy"      },
        { "Mythological - Cradle of Civilisation",      "Mitologia - kołyska cywilizacji"           },
        { "Prehistoric - After the Asteroid",           "Prehistoria - po asteroidzie"              },
        { "Prehistoric - Jurassic Safari",              "Prehistoria - jurajskie safari"            },
        { "Prehistoric - Stone Age",                    "Prehistoria - epoka kamienna"              },
        { "Roaring Twenties - Prison Island",           "Ryczące Dwudziestki - wyspa więzienna"     },
        { "Roaring Twenties - Schneider Cup",           "Ryczące Dwudziestki - puchar Schneidera"   },
        { "Roaring Twenties - Skyscrapers",             "Ryczące Dwudziestki - drapacze chmur"      },
        { "Rock 'n' Roll - Flower Power",               "Rock 'n' Roll - dzieci kwiaty"             },
        // Skipping Rock 'n' Roll - Rock 'n' Roll as that name matches exactly.

    };

    // RCT
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT1[] =
    {
        { SC_FOREST_FRONTIERS,          "Forest Frontiers",     SCENARIO_CATEGORY_BEGINNER      },
        { SC_DYNAMITE_DUNES,            "Dynamite Dunes",       SCENARIO_CATEGORY_BEGINNER      },
        { SC_LEAFY_LAKE,                "Leafy Lake",           SCENARIO_CATEGORY_BEGINNER      },
        { SC_DIAMOND_HEIGHTS,           "Diamond Heights",      SCENARIO_CATEGORY_BEGINNER      },
        { SC_EVERGREEN_GARDENS,         "Evergreen Gardens",    SCENARIO_CATEGORY_BEGINNER      },
        { SC_BUMBLY_BEACH,              "Bumbly Beach",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_TRINITY_ISLANDS,           "Trinity Islands",      SCENARIO_CATEGORY_CHALLENGING   },
        { SC_KATIES_DREAMLAND,          "Katie's Dreamland",    SCENARIO_CATEGORY_CHALLENGING   },
        { SC_POKEY_PARK,                "Pokey Park",           SCENARIO_CATEGORY_CHALLENGING   },
        { SC_WHITE_WATER_PARK,          "White Water Park",     SCENARIO_CATEGORY_CHALLENGING   },
        { SC_MILLENNIUM_MINES,          "Millennium Mines",     SCENARIO_CATEGORY_CHALLENGING   },
        { SC_KARTS_COASTERS,            "Karts & Coasters",     SCENARIO_CATEGORY_CHALLENGING   },
        { SC_MELS_WORLD,                "Mel's World",          SCENARIO_CATEGORY_CHALLENGING   },
        { SC_MYSTIC_MOUNTAIN,           "Mystic Mountain",      SCENARIO_CATEGORY_CHALLENGING   },
        { SC_PACIFIC_PYRAMIDS,          "Pacific Pyramids",     SCENARIO_CATEGORY_CHALLENGING   },
        { SC_CRUMBLY_WOODS,             "Crumbly Woods",        SCENARIO_CATEGORY_CHALLENGING   },
        { SC_PARADISE_PIER,             "Paradise Pier",        SCENARIO_CATEGORY_CHALLENGING   },
        { SC_LIGHTNING_PEAKS,           "Lightning Peaks",      SCENARIO_CATEGORY_EXPERT        },
        { SC_IVORY_TOWERS,              "Ivory Towers",         SCENARIO_CATEGORY_EXPERT        },
        { SC_RAINBOW_VALLEY,            "Rainbow Valley",       SCENARIO_CATEGORY_EXPERT        },
        { SC_THUNDER_ROCK,              "Thunder Rock",         SCENARIO_CATEGORY_EXPERT        },
        { SC_MEGA_PARK,                 "Mega Park",            SCENARIO_CATEGORY_OTHER         },
    };

    // RCT: Added Attractions
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT1AA[] =
    {
        { SC_WHISPERING_CLIFFS,         "Whispering Cliffs",    SCENARIO_CATEGORY_BEGINNER      },
        { SC_THREE_MONKEYS_PARK,        "Three Monkeys Park",   SCENARIO_CATEGORY_BEGINNER      },
        { SC_CANARY_MINES,              "Canary Mines",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_BARONY_BRIDGE,             "Barony Bridge",        SCENARIO_CATEGORY_BEGINNER      },
        { SC_FUNTOPIA,                  "Funtopia",             SCENARIO_CATEGORY_BEGINNER      },
        { SC_HAUNTED_HARBOUR,           "Haunted Harbour",      SCENARIO_CATEGORY_BEGINNER      },
        { SC_FUN_FORTRESS,              "Fun Fortress",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_FUTURE_WORLD,              "Future World",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_GENTLE_GLEN,               "Gentle Glen",          SCENARIO_CATEGORY_BEGINNER      },
        { SC_JOLLY_JUNGLE,              "Jolly Jungle",         SCENARIO_CATEGORY_CHALLENGING   },
        { SC_HYDRO_HILLS,               "Hydro Hills",          SCENARIO_CATEGORY_CHALLENGING   },
        { SC_SPRIGHTLY_PARK,            "Sprightly Park",       SCENARIO_CATEGORY_CHALLENGING   },
        { SC_MAGIC_QUARTERS,            "Magic Quarters",       SCENARIO_CATEGORY_CHALLENGING   },
        { SC_FRUIT_FARM,                "Fruit Farm",           SCENARIO_CATEGORY_CHALLENGING   },
        { SC_BUTTERFLY_DAM,             "Butterfly Dam",        SCENARIO_CATEGORY_CHALLENGING   },
        { SC_COASTER_CANYON,            "Coaster Canyon",       SCENARIO_CATEGORY_CHALLENGING   },
        { SC_THUNDERSTORM_PARK,         "Thunderstorm Park",    SCENARIO_CATEGORY_CHALLENGING   },
        { SC_HARMONIC_HILLS,            "Harmonic Hills",       SCENARIO_CATEGORY_CHALLENGING   },
        { SC_ROMAN_VILLAGE,             "Roman Village",        SCENARIO_CATEGORY_CHALLENGING   },
        { SC_SWAMP_COVE,                "Swamp Cove",           SCENARIO_CATEGORY_CHALLENGING   },
        { SC_ADRENALINE_HEIGHTS,        "Adrenaline Heights",   SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UTOPIA_PARK,               "Utopia Park",          SCENARIO_CATEGORY_CHALLENGING   },
        { SC_ROTTING_HEIGHTS,           "Rotting Heights",      SCENARIO_CATEGORY_EXPERT        },
        { SC_FIASCO_FOREST,             "Fiasco Forest",        SCENARIO_CATEGORY_EXPERT        },
        { SC_PICKLE_PARK,               "Pickle Park",          SCENARIO_CATEGORY_EXPERT        },
        { SC_GIGGLE_DOWNS,              "Giggle Downs",         SCENARIO_CATEGORY_EXPERT        },
        { SC_MINERAL_PARK,              "Mineral Park",         SCENARIO_CATEGORY_EXPERT        },
        { SC_COASTER_CRAZY,             "Coaster Crazy",        SCENARIO_CATEGORY_EXPERT        },
        { SC_URBAN_PARK,                "Urban Park",           SCENARIO_CATEGORY_EXPERT        },
        { SC_GEOFFREY_GARDENS,          "Geoffrey Gardens",     SCENARIO_CATEGORY_EXPERT        },
    };

    // RCT: Loopy Landscapes
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT1LL[] =
    {
        {   SC_ICEBERG_ISLANDS,         "Iceberg Islands",      SCENARIO_CATEGORY_BEGINNER      },
        {   SC_VOLCANIA,                "Volcania",             SCENARIO_CATEGORY_BEGINNER      },
        {   SC_ARID_HEIGHTS,            "Arid Heights",         SCENARIO_CATEGORY_BEGINNER      },
        {   SC_RAZOR_ROCKS,             "Razor Rocks",          SCENARIO_CATEGORY_BEGINNER      },
        {   SC_CRATER_LAKE,             "Crater Lake",          SCENARIO_CATEGORY_BEGINNER      },
        {   SC_VERTIGO_VIEWS,           "Vertigo Views",        SCENARIO_CATEGORY_BEGINNER      },
        {   SC_PARADISE_PIER_2,         "Paradise Pier 2",      SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_DRAGONS_COVE,            "Dragon's Cove",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_GOOD_KNIGHT_PARK,        "Good Knight Park",     SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_WACKY_WARREN,            "Wacky Warren",         SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_GRAND_GLACIER,           "Grand Glacier",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_CRAZY_CRATERS,           "Crazy Craters",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_DUSTY_DESERT,            "Dusty Desert",         SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_WOODWORM_PARK,           "Woodworm Park",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_ICARUS_PARK,             "Icarus Park",          SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_SUNNY_SWAMPS,            "Sunny Swamps",         SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_FRIGHTMARE_HILLS,        "Frightmare Hills",     SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_THUNDER_ROCKS,           "Thunder Rocks",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_OCTAGON_PARK,            "Octagon Park",         SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_PLEASURE_ISLAND,         "Pleasure Island",      SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_ICICLE_WORLDS,           "Icicle Worlds",        SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_SOUTHERN_SANDS,          "Southern Sands",       SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_TINY_TOWERS,             "Tiny Towers",          SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_NEVERMORE_PARK,          "Nevermore Park",       SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_PACIFICA,                "Pacifica",             SCENARIO_CATEGORY_CHALLENGING   },
        {   SC_URBAN_JUNGLE,            "Urban Jungle",         SCENARIO_CATEGORY_EXPERT        },
        {   SC_TERROR_TOWN,             "Terror Town",          SCENARIO_CATEGORY_EXPERT        },
        {   SC_MEGAWORLD_PARK,          "Megaworld Park",       SCENARIO_CATEGORY_EXPERT        },
        {   SC_VENUS_PONDS,             "Venus Ponds",          SCENARIO_CATEGORY_EXPERT        },
        {   SC_MICRO_PARK,              "Micro Park",           SCENARIO_CATEGORY_EXPERT        },
    };

    // RCT2
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT2[] =
    {
        { SC_UNIDENTIFIED,              "Electric Fields",      SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Factory Capers",       SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Crazy Castle",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Dusty Greens",         SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Bumbly Bazaar",        SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Infernal Views",       SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Lucky Lake",           SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Botany Breakers",      SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Alpine Adventures",    SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Gravity Gardens",      SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Extreme Heights",      SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Amity Airfield",       SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Ghost Town",           SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Fungus Woods",         SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Rainbow Summit",       SCENARIO_CATEGORY_EXPERT        },
    };

    // RCT2: Wacky Worlds
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT2WW[] =
    {
        { SC_UNIDENTIFIED,              "Africa - Victoria Falls",                          SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Asia - Great Wall of China Tourism Enhancement",   SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "North America - Grand Canyon",                     SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "South America - Rio Carnival",                     SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Africa - African Diamond Mine",                    SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Asia - Maharaja Palace",                           SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Australasia - Ayers Rock",                         SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Europe - European Cultural Festival",              SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "North America - Rollercoaster Heaven",             SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "South America - Inca Lost City",                   SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Africa - Oasis",                                   SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Antarctic - Ecological Salvage",                   SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Asia - Japanese Coastal Reclaim",                  SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Australasia - Fun at the Beach",                   SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Europe - Renovation",                              SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "N. America - Extreme Hawaiian Island",             SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "South America - Rain Forest Plateau",              SCENARIO_CATEGORY_EXPERT        },
    };

    // RCT2: Time Twister
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRCT2TT[] =
    {
        { SC_UNIDENTIFIED,              "Dark Age - Robin Hood",                            SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Prehistoric - After the Asteroid",                 SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Prison Island",                 SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Flower Power",                     SCENARIO_CATEGORY_BEGINNER      },
        { SC_UNIDENTIFIED,              "Dark Age - Castle",                                SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Future - First Encounters",                        SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Mythological - Animatronic Film Set",              SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Prehistoric - Jurassic Safari",                    SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Schneider Cup",                 SCENARIO_CATEGORY_CHALLENGING   },
        { SC_UNIDENTIFIED,              "Future - Future World",                            SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Mythological - Cradle of Civilisation",            SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Prehistoric - Stone Age",                          SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Roaring Twenties - Skyscrapers",                   SCENARIO_CATEGORY_EXPERT        },
        { SC_UNIDENTIFIED,              "Rock 'n' Roll - Rock 'n' Roll",                    SCENARIO_CATEGORY_EXPERT        },
    };

    // User Created Expansion Sets
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesUCES[] =
    {
        { SC_UNIDENTIFIED,              "Lighthouse of Alexandria by Katatude for UCES",    SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Cleveland's Luna Park",                            SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Mount Vesuvius 1700 A.D. by Katatude for UCES",    SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "The Sandbox by Katatude for UCES",                 SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Niagara Falls & Gorge by Katatude for UCES",       SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Rocky Mountain Miners",                            SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "The Time Machine by Katatude for UCES",            SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Tower of Babel",                                   SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Transformation",                                   SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Urbis Incognitus",                                 SCENARIO_CATEGORY_TIME_MACHINE     },
        { SC_UNIDENTIFIED,              "Beneath the Christmas Tree by Katatude for UCES",  SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Bigrock Blast",                                    SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Camp Mockingbird for UCES by Katatude",            SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Choo Choo Town",                                   SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Dragon Islands",                                   SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Kiddy Karnival II",                                SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "Sand Dune",                                        SCENARIO_CATEGORY_KATYS_DREAMWORLD },
        { SC_UNIDENTIFIED,              "UCES Halloween",                                   SCENARIO_CATEGORY_OTHER            },
    };

    // Real parks
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesRealParks[] =
    {
        { SC_ALTON_TOWERS,              "Alton Towers",                                     SCENARIO_CATEGORY_REAL  },
        { SC_HEIDE_PARK,                "Heide-Park",                                       SCENARIO_CATEGORY_REAL  },
        { SC_BLACKPOOL_PLEASURE_BEACH,  "Blackpool Pleasure Beach",                         SCENARIO_CATEGORY_REAL  },
        { SC_UNIDENTIFIED,              "Six Flags Belgium",                                SCENARIO_CATEGORY_REAL  },
        { SC_UNIDENTIFIED,              "Six Flags Great Adventure",                        SCENARIO_CATEGORY_REAL  },
        { SC_UNIDENTIFIED,              "Six Flags Holland",                                SCENARIO_CATEGORY_REAL  },
        { SC_UNIDENTIFIED,              "Six Flags Magic Mountain",                         SCENARIO_CATEGORY_REAL  },
        { SC_UNIDENTIFIED,              "Six Flags over Texas",                             SCENARIO_CATEGORY_REAL  },
    };

    // Other parks
    static constexpr const ScenarioTitleDescriptor ScenarioTitlesExtrasParks[] =
    {
        { SC_FORT_ANACHRONISM,                          "Fort Anachronism",                                 SCENARIO_CATEGORY_DLC            },
        { SC_PCPLAYER,                                  "PC Player",                                        SCENARIO_CATEGORY_DLC            },
        { SC_PCGW,                                      "PC Gaming World",                                  SCENARIO_CATEGORY_DLC            },
        { SC_GAMEPLAY,                                  "gameplay",                                         SCENARIO_CATEGORY_DLC            },
        { SC_UNIDENTIFIED,                              "Panda World",                                      SCENARIO_CATEGORY_DLC            },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Belgium",                 SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Great Adventure",         SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Holland",                 SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Magic Mountain",          SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Park",                    SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags over Texas",              SCENARIO_CATEGORY_BUILD_YOUR_OWN },
        { SC_UNIDENTIFIED,                              "Competition Land 1",                               SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_UNIDENTIFIED,                              "Competition Land 2",                               SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_BOBSLED_COMPETITION,                       "Bobsled Roller Coaster Competition",               SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_GO_KARTS_COMPETITION,                      "Go Karts Competition",                             SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_INVERTED_ROLLER_COASTER_COMPETITION,       "Inverted Roller Coaster Competition",              SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_MINE_TRAIN_COMPETITION,                    "Mine Train Roller Coaster Competition",            SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_STAND_UP_STEEL_ROLLER_COASTER_COMPETITION, "Stand-Up Steel Roller Coaster Competition",        SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_STEEL_CORKSCREW_COMPETITION,               "Steel Corkscrew Roller Coaster Competition",       SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_STEEL_MINI_ROLLER_COASTER_COMPETITION,     "Steel Mini Roller Coaster Competition",            SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_STEEL_ROLLER_COASTER_COMPETITION,          "Steel Roller Coaster Competition",                 SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_STEEL_TWISTER_COMPETITION,                 "Steel Twister Roller Coaster Competition",         SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_SUSPENDED_ROLLER_COASTER_COMPETITION,      "Suspended Roller Coaster Competition",             SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_WOODEN_ROLLER_COASTER_COMPETITION,         "Wooden Roller Coaster Competition",                SCENARIO_CATEGORY_COMPETITIONS   },
        { SC_UNIDENTIFIED,                              "Tycoon Park",                                      SCENARIO_CATEGORY_OTHER          },
    };

    #define DEFINE_SCENARIO_TITLE_DESC_GROUP(x) { std::size(x), x }
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
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesUCES),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesRealParks),
        DEFINE_SCENARIO_TITLE_DESC_GROUP(ScenarioTitlesExtrasParks),
    };
    // clang-format on

#pragma endregion

    bool TryGetByName(const utf8* name, source_desc* outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        int32_t currentIndex = 0;
        for (size_t i = 0; i < std::size(ScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < ScenarioTitlesBySource[i].count; j++)
            {
                const ScenarioTitleDescriptor* desc = &ScenarioTitlesBySource[i].titles[j];
                if (String::Equals(name, desc->Title, true))
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = static_cast<uint8_t>(i);
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    return true;
                }
                currentIndex++;
            }
        }

        outDesc->title = nullptr;
        outDesc->id = SC_UNIDENTIFIED;
        outDesc->source = static_cast<uint8_t>(ScenarioSource::Other);
        outDesc->index = -1;
        outDesc->category = SCENARIO_CATEGORY_OTHER;
        return false;
    }

    bool TryGetById(uint8_t id, source_desc* outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        int32_t currentIndex = 0;
        for (size_t i = 0; i < std::size(ScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < ScenarioTitlesBySource[i].count; j++)
            {
                const ScenarioTitleDescriptor* desc = &ScenarioTitlesBySource[i].titles[j];
                if (id == desc->Id)
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = static_cast<uint8_t>(i);
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    return true;
                }
                currentIndex++;
            }
        }

        outDesc->title = "";
        outDesc->id = SC_UNIDENTIFIED;
        outDesc->source = static_cast<uint8_t>(ScenarioSource::Other);
        outDesc->index = -1;
        outDesc->category = SCENARIO_CATEGORY_OTHER;
        return false;
    }

    void NormaliseName(utf8* buffer, size_t bufferSize, const utf8* name)
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
        for (const ScenarioAlias& alias : ScenarioAliases)
        {
            if (String::Equals(alias.Alternative, name))
            {
                log_verbose("Found alias: %s; will treat as: %s", name, alias.Original);
                String::Set(buffer, bufferSize, alias.Original);
            }
        }
    }
} // namespace ScenarioSources
