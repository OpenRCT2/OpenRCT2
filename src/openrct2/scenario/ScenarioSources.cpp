/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioSources.h"

#include "../Diagnostic.h"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "Scenario.h"
#include "ScenarioRepository.h"

#include <iterator>
#include <span>

namespace OpenRCT2::ScenarioSources
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
        u8string_view TextObjectId;
    };

#pragma region Scenario Data

    // clang-format off
    static constexpr ScenarioAlias kScenarioAliases[] =
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
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT1[] =
    {
        { SC_FOREST_FRONTIERS,  "Forest Frontiers",  SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.forest_frontiers"  },
        { SC_DYNAMITE_DUNES,    "Dynamite Dunes",    SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.dynamite_dunes"    },
        { SC_LEAFY_LAKE,        "Leafy Lake",        SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.leafy_lake"        },
        { SC_DIAMOND_HEIGHTS,   "Diamond Heights",   SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.diamond_heights"   },
        { SC_EVERGREEN_GARDENS, "Evergreen Gardens", SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.evergreen_gardens" },
        { SC_BUMBLY_BEACH,      "Bumbly Beach",      SCENARIO_CATEGORY_BEGINNER,    "rct1.scenario_text.bumbly_beach"      },
        { SC_TRINITY_ISLANDS,   "Trinity Islands",   SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.trinity_islands"   },
        { SC_KATIES_DREAMLAND,  "Katie's Dreamland", SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.katies_dreamland"  },
        { SC_POKEY_PARK,        "Pokey Park",        SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.pokey_park"        },
        { SC_WHITE_WATER_PARK,  "White Water Park",  SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.white_water_park"  },
        { SC_MILLENNIUM_MINES,  "Millennium Mines",  SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.millennium_mines"  },
        { SC_KARTS_COASTERS,    "Karts & Coasters",  SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.karts_coasters"    },
        { SC_MELS_WORLD,        "Mel's World",       SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.mels_world"        },
        { SC_MYSTIC_MOUNTAIN,   "Mystic Mountain",   SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.mystic_mountain"   },
        { SC_PACIFIC_PYRAMIDS,  "Pacific Pyramids",  SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.pacific_pyramids"  },
        { SC_CRUMBLY_WOODS,     "Crumbly Woods",     SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.crumbly_woods"     },
        { SC_PARADISE_PIER,     "Paradise Pier",     SCENARIO_CATEGORY_CHALLENGING, "rct1.scenario_text.paradise_pier"     },
        { SC_LIGHTNING_PEAKS,   "Lightning Peaks",   SCENARIO_CATEGORY_EXPERT,      "rct1.scenario_text.lightning_peaks"   },
        { SC_IVORY_TOWERS,      "Ivory Towers",      SCENARIO_CATEGORY_EXPERT,      "rct1.scenario_text.ivory_towers"      },
        { SC_RAINBOW_VALLEY,    "Rainbow Valley",    SCENARIO_CATEGORY_EXPERT,      "rct1.scenario_text.rainbow_valley"    },
        { SC_THUNDER_ROCK,      "Thunder Rock",      SCENARIO_CATEGORY_EXPERT,      "rct1.scenario_text.thunder_rock"      },
        { SC_MEGA_PARK,         "Mega Park",         SCENARIO_CATEGORY_OTHER,       "rct1.scenario_text.mega_park"         },
    };

    // RCT: Added Attractions
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT1AA[] =
    {
        { SC_WHISPERING_CLIFFS,  "Whispering Cliffs",  SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.whispering_cliffs"  },
        { SC_THREE_MONKEYS_PARK, "Three Monkeys Park", SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.three_monkeys_park" },
        { SC_CANARY_MINES,       "Canary Mines",       SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.canary_mines"       },
        { SC_BARONY_BRIDGE,      "Barony Bridge",      SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.barony_bridge"      },
        { SC_FUNTOPIA,           "Funtopia",           SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.funtopia"           },
        { SC_HAUNTED_HARBOUR,    "Haunted Harbour",    SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.haunted_harbour"    },
        { SC_FUN_FORTRESS,       "Fun Fortress",       SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.fun_fortress"       },
        { SC_FUTURE_WORLD,       "Future World",       SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.future_world"       },
        { SC_GENTLE_GLEN,        "Gentle Glen",        SCENARIO_CATEGORY_BEGINNER,    "rct1aa.scenario_text.gentle_glen"        },
        { SC_JOLLY_JUNGLE,       "Jolly Jungle",       SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.jolly_jungle"       },
        { SC_HYDRO_HILLS,        "Hydro Hills",        SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.hydro_hills"        },
        { SC_SPRIGHTLY_PARK,     "Sprightly Park",     SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.sprightly_park"     },
        { SC_MAGIC_QUARTERS,     "Magic Quarters",     SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.magic_quarters"     },
        { SC_FRUIT_FARM,         "Fruit Farm",         SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.fruit_farm"         },
        { SC_BUTTERFLY_DAM,      "Butterfly Dam",      SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.butterfly_dam"      },
        { SC_COASTER_CANYON,     "Coaster Canyon",     SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.coaster_canyon"     },
        { SC_THUNDERSTORM_PARK,  "Thunderstorm Park",  SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.thunderstorm_park"  },
        { SC_HARMONIC_HILLS,     "Harmonic Hills",     SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.harmonic_hills"     },
        { SC_ROMAN_VILLAGE,      "Roman Village",      SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.roman_village"      },
        { SC_SWAMP_COVE,         "Swamp Cove",         SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.swamp_cove"         },
        { SC_ADRENALINE_HEIGHTS, "Adrenaline Heights", SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.adrenaline_heights" },
        { SC_UTOPIA_PARK,        "Utopia Park",        SCENARIO_CATEGORY_CHALLENGING, "rct1aa.scenario_text.utopia_park"        },
        { SC_ROTTING_HEIGHTS,    "Rotting Heights",    SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.rotting_heights"    },
        { SC_FIASCO_FOREST,      "Fiasco Forest",      SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.fiasco_forest"      },
        { SC_PICKLE_PARK,        "Pickle Park",        SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.pickle_park"        },
        { SC_GIGGLE_DOWNS,       "Giggle Downs",       SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.giggle_downs"       },
        { SC_MINERAL_PARK,       "Mineral Park",       SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.mineral_park"       },
        { SC_COASTER_CRAZY,      "Coaster Crazy",      SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.coaster_crazy"      },
        { SC_URBAN_PARK,         "Urban Park",         SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.urban_park"         },
        { SC_GEOFFREY_GARDENS,   "Geoffrey Gardens",   SCENARIO_CATEGORY_EXPERT,      "rct1aa.scenario_text.geoffrey_gardens"   },
    };

    // RCT: Loopy Landscapes
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT1LL[] =
    {
        { SC_ICEBERG_ISLANDS,  "Iceberg Islands",  SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.iceberg_islands"  },
        { SC_VOLCANIA,         "Volcania",         SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.volcania"         },
        { SC_ARID_HEIGHTS,     "Arid Heights",     SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.arid_heights"     },
        { SC_RAZOR_ROCKS,      "Razor Rocks",      SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.razor_rocks"      },
        { SC_CRATER_LAKE,      "Crater Lake",      SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.crater_lake"      },
        { SC_VERTIGO_VIEWS,    "Vertigo Views",    SCENARIO_CATEGORY_BEGINNER,    "rct1ll.scenario_text.vertigo_views"    },
        { SC_PARADISE_PIER_2,  "Paradise Pier 2",  SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.paradise_pier_2"  },
        { SC_DRAGONS_COVE,     "Dragon's Cove",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.dragons_cove"     },
        { SC_GOOD_KNIGHT_PARK, "Good Knight Park", SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.good_knight_park" },
        { SC_WACKY_WARREN,     "Wacky Warren",     SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.wacky_warren"     },
        { SC_GRAND_GLACIER,    "Grand Glacier",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.grand_glacier"    },
        { SC_CRAZY_CRATERS,    "Crazy Craters",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.crazy_craters"    },
        { SC_DUSTY_DESERT,     "Dusty Desert",     SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.dusty_desert"     },
        { SC_WOODWORM_PARK,    "Woodworm Park",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.woodworm_park"    },
        { SC_ICARUS_PARK,      "Icarus Park",      SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.icarus_park"      },
        { SC_SUNNY_SWAMPS,     "Sunny Swamps",     SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.sunny_swamps"     },
        { SC_FRIGHTMARE_HILLS, "Frightmare Hills", SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.frightmare_hills" },
        { SC_THUNDER_ROCKS,    "Thunder Rocks",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.thunder_rocks"    },
        { SC_OCTAGON_PARK,     "Octagon Park",     SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.octagon_park"     },
        { SC_PLEASURE_ISLAND,  "Pleasure Island",  SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.pleasure_island"  },
        { SC_ICICLE_WORLDS,    "Icicle Worlds",    SCENARIO_CATEGORY_CHALLENGING, "rct1ll.scenario_text.icicle_worlds"    },
        { SC_SOUTHERN_SANDS,   "Southern Sands",   SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.southern_sands"   },
        { SC_TINY_TOWERS,      "Tiny Towers",      SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.tiny_towers"      },
        { SC_NEVERMORE_PARK,   "Nevermore Park",   SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.nevermore_park"   },
        { SC_PACIFICA,         "Pacifica",         SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.pacifica"         },
        { SC_URBAN_JUNGLE,     "Urban Jungle",     SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.urban_jungle"     },
        { SC_TERROR_TOWN,      "Terror Town",      SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.terror_town"      },
        { SC_MEGAWORLD_PARK,   "Megaworld Park",   SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.megaworld_park"   },
        { SC_VENUS_PONDS,      "Venus Ponds",      SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.venus_ponds"      },
        { SC_MICRO_PARK,       "Micro Park",       SCENARIO_CATEGORY_EXPERT,      "rct1ll.scenario_text.micro_park"       },
    };

    // RCT2
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2[] =
    {
        { SC_UNIDENTIFIED, "Electric Fields",   SCENARIO_CATEGORY_BEGINNER,    "rct2.scenario_text.electric_fields"   },
        { SC_UNIDENTIFIED, "Factory Capers",    SCENARIO_CATEGORY_BEGINNER,    "rct2.scenario_text.factory_capers"    },
        { SC_UNIDENTIFIED, "Crazy Castle",      SCENARIO_CATEGORY_BEGINNER,    "rct2.scenario_text.crazy_castle"      },
        { SC_UNIDENTIFIED, "Dusty Greens",      SCENARIO_CATEGORY_BEGINNER,    "rct2.scenario_text.dusty_greens"      },
        { SC_UNIDENTIFIED, "Bumbly Bazaar",     SCENARIO_CATEGORY_BEGINNER,    "rct2.scenario_text.bumbly_bazaar"     },
        { SC_UNIDENTIFIED, "Infernal Views",    SCENARIO_CATEGORY_CHALLENGING, "rct2.scenario_text.infernal_views"    },
        { SC_UNIDENTIFIED, "Lucky Lake",        SCENARIO_CATEGORY_CHALLENGING, "rct2.scenario_text.lucky_lake"        },
        { SC_UNIDENTIFIED, "Botany Breakers",   SCENARIO_CATEGORY_CHALLENGING, "rct2.scenario_text.botany_breakers"   },
        { SC_UNIDENTIFIED, "Alpine Adventures", SCENARIO_CATEGORY_CHALLENGING, "rct2.scenario_text.alpine_adventures" },
        { SC_UNIDENTIFIED, "Gravity Gardens",   SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.gravity_gardens"   },
        { SC_UNIDENTIFIED, "Extreme Heights",   SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.extreme_heights"   },
        { SC_UNIDENTIFIED, "Amity Airfield",    SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.amity_airfield"    },
        { SC_UNIDENTIFIED, "Ghost Town",        SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.ghost_town"        },
        { SC_UNIDENTIFIED, "Fungus Woods",      SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.fungus_woods"      },
        { SC_UNIDENTIFIED, "Rainbow Summit",    SCENARIO_CATEGORY_EXPERT,      "rct2.scenario_text.rainbow_summit"    },
    };

    // RCT2: Wacky Worlds
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2WW[] =
    {
        { SC_UNIDENTIFIED, "North America - Grand Canyon",                   SCENARIO_CATEGORY_BEGINNER,    "rct2ww.scenario_text.canyon_calamities"     },
        { SC_UNIDENTIFIED, "Asia - Great Wall of China Tourism Enhancement", SCENARIO_CATEGORY_BEGINNER,    "rct2ww.scenario_text.great_wall_of_china"   },
        { SC_UNIDENTIFIED, "Africa - African Diamond Mine",                  SCENARIO_CATEGORY_BEGINNER,    "rct2ww.scenario_text.mines_of_africa"       },
        { SC_UNIDENTIFIED, "Australasia - Ayers Rock",                       SCENARIO_CATEGORY_BEGINNER,    "rct2ww.scenario_text.ayers_adventure"       },
        { SC_UNIDENTIFIED, "North America - Rollercoaster Heaven",           SCENARIO_CATEGORY_BEGINNER,    "rct2ww.scenario_text.rollercoaster_heaven"  },
        { SC_UNIDENTIFIED, "Africa - Oasis",                                 SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.mirage_madness"        },
        { SC_UNIDENTIFIED, "South America - Rio Carnival",                   SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.sugarloaf_shores"      },
        { SC_UNIDENTIFIED, "Asia - Maharaja Palace",                         SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.park_maharaja"         },
        { SC_UNIDENTIFIED, "Africa - Victoria Falls",                        SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.over_the_edge"         },
        { SC_UNIDENTIFIED, "N. America - Extreme Hawaiian Island",           SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.wacky_waikiki"         },
        { SC_UNIDENTIFIED, "South America - Rain Forest Plateau",            SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.rainforest_romp"       },
        { SC_UNIDENTIFIED, "Europe - Renovation",                            SCENARIO_CATEGORY_CHALLENGING, "rct2ww.scenario_text.from_the_ashes"        },
        { SC_UNIDENTIFIED, "Antarctic - Ecological Salvage",                 SCENARIO_CATEGORY_EXPERT,      "rct2ww.scenario_text.icy_adventures"        },
        { SC_UNIDENTIFIED, "Europe - European Cultural Festival",            SCENARIO_CATEGORY_EXPERT,      "rct2ww.scenario_text.european_extravaganza" },
        { SC_UNIDENTIFIED, "Australasia - Fun at the Beach",                 SCENARIO_CATEGORY_EXPERT,      "rct2ww.scenario_text.beach_barbecue_blast"  },
        { SC_UNIDENTIFIED, "South America - Inca Lost City",                 SCENARIO_CATEGORY_EXPERT,      "rct2ww.scenario_text.lost_city_founder"     },
        { SC_UNIDENTIFIED, "Asia - Japanese Coastal Reclaim",                SCENARIO_CATEGORY_EXPERT,      "rct2ww.scenario_text.okinawa_coast"         },
    };

    // RCT2: Time Twister
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2TT[] =
    {
        { SC_UNIDENTIFIED, "Dark Age - Robin Hood",                 SCENARIO_CATEGORY_BEGINNER,    "rct2tt.scenario_text.sherwood_forest"               },
        { SC_UNIDENTIFIED, "Prehistoric - After the Asteroid",      SCENARIO_CATEGORY_BEGINNER,    "rct2tt.scenario_text.crater_carnage"                },
        { SC_UNIDENTIFIED, "Roaring Twenties - Prison Island",      SCENARIO_CATEGORY_BEGINNER,    "rct2tt.scenario_text.alcatraz"                      },
        { SC_UNIDENTIFIED, "Future - First Encounters",             SCENARIO_CATEGORY_BEGINNER,    "rct2tt.scenario_text.extraterrestrial_extravaganza" },
        { SC_UNIDENTIFIED, "Roaring Twenties - Schneider Cup",      SCENARIO_CATEGORY_CHALLENGING, "rct2tt.scenario_text.schneider_shores"              },
        { SC_UNIDENTIFIED, "Prehistoric - Stone Age",               SCENARIO_CATEGORY_CHALLENGING, "rct2tt.scenario_text.rocky_rambles"                 },
        { SC_UNIDENTIFIED, "Mythological - Cradle of Civilisation", SCENARIO_CATEGORY_CHALLENGING, "rct2tt.scenario_text.mythological_madness"          },
        { SC_UNIDENTIFIED, "Rock 'n' Roll - Rock 'n' Roll",         SCENARIO_CATEGORY_CHALLENGING, "rct2tt.scenario_text.rock_n_roll_revival"           },
        { SC_UNIDENTIFIED, "Future - Future World",                 SCENARIO_CATEGORY_CHALLENGING, "rct2tt.scenario_text.gemini_city"                   },
        { SC_UNIDENTIFIED, "Roaring Twenties - Skyscrapers",        SCENARIO_CATEGORY_EXPERT,      "rct2tt.scenario_text.metropolis"                    },
        { SC_UNIDENTIFIED, "Rock 'n' Roll - Flower Power",          SCENARIO_CATEGORY_EXPERT,      "rct2tt.scenario_text.woodstock"                     },
        { SC_UNIDENTIFIED, "Dark Age - Castle",                     SCENARIO_CATEGORY_EXPERT,      "rct2tt.scenario_text.cliffside_castle"              },
        { SC_UNIDENTIFIED, "Mythological - Animatronic Film Set",   SCENARIO_CATEGORY_EXPERT,      "rct2tt.scenario_text.animatronic_antics"            },
        { SC_UNIDENTIFIED, "Prehistoric - Jurassic Safari",         SCENARIO_CATEGORY_EXPERT,      "rct2tt.scenario_text.coastersaurus"                 },
    };

    // User Created Expansion Sets
    static constexpr ScenarioTitleDescriptor kScenarioTitlesUCES[] =
    {
        { SC_UNIDENTIFIED, "Lighthouse of Alexandria by Katatude for UCES",    SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.the_lighthouse_of_alexandria" },
        { SC_UNIDENTIFIED, "Cleveland's Luna Park",                            SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.luna_park_cleveland"          },
        { SC_UNIDENTIFIED, "Mount Vesuvius 1700 A.D. by Katatude for UCES",    SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.mount_vesuvius"               },
        { SC_UNIDENTIFIED, "The Sandbox by Katatude for UCES",                 SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.the_sandbox"                  },
        { SC_UNIDENTIFIED, "Niagara Falls & Gorge by Katatude for UCES",       SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.niagara_falls_gorge"          },
        { SC_UNIDENTIFIED, "Rocky Mountain Miners",                            SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.rocky_mountain_miners"        },
        { SC_UNIDENTIFIED, "The Time Machine by Katatude for UCES",            SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.the_time_machine"             },
        { SC_UNIDENTIFIED, "Tower of Babel",                                   SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.tower_of_babel"               },
        { SC_UNIDENTIFIED, "Transformation",                                   SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.transformation"               },
        { SC_UNIDENTIFIED, "Urbis Incognitus",                                 SCENARIO_CATEGORY_TIME_MACHINE,     "uces.scenario_text.urbis_incognitus"             },
        { SC_UNIDENTIFIED, "Beneath the Christmas Tree by Katatude for UCES",  SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.beneath_the_christmas_tree"   },
        { SC_UNIDENTIFIED, "Bigrock Blast",                                    SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.bigrock_blast"                },
        { SC_UNIDENTIFIED, "Camp Mockingbird for UCES by Katatude",            SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.camp_mockingbird"             },
        { SC_UNIDENTIFIED, "Choo Choo Town",                                   SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.choochoo_town"                },
        { SC_UNIDENTIFIED, "Dragon Islands",                                   SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.dragon_islands"               },
        { SC_UNIDENTIFIED, "Kiddy Karnival II",                                SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.kiddie_karnival_ii"           },
        { SC_UNIDENTIFIED, "Sand Dune",                                        SCENARIO_CATEGORY_KATYS_DREAMWORLD, "uces.scenario_text.sand_dune"                    },
        { SC_UNIDENTIFIED, "UCES Halloween",                                   SCENARIO_CATEGORY_OTHER,            "uces.scenario_text.cemetery_ridge"               },
    };

    // Real parks
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRealParks[] =
    {
        { SC_ALTON_TOWERS,             "Alton Towers",              SCENARIO_CATEGORY_REAL, "rct1ll.scenario_text.alton_towers"            },
        { SC_HEIDE_PARK,               "Heide-Park",                SCENARIO_CATEGORY_REAL, "rct1ll.scenario_text.heidepark"               },
        { SC_BLACKPOOL_PLEASURE_BEACH, "Blackpool Pleasure Beach",  SCENARIO_CATEGORY_REAL, "rct1ll.scenario_text.blackpool_pleasure_beach"  },
        { SC_UNIDENTIFIED,             "Six Flags Belgium",         SCENARIO_CATEGORY_REAL, "rct2.scenario_text.six_flags_belgium"         },
        { SC_UNIDENTIFIED,             "Six Flags Great Adventure", SCENARIO_CATEGORY_REAL, "rct2.scenario_text.six_flags_great_adventure" },
        { SC_UNIDENTIFIED,             "Six Flags Holland",         SCENARIO_CATEGORY_REAL, "rct2.scenario_text.six_flags_holland"         },
        { SC_UNIDENTIFIED,             "Six Flags Magic Mountain",  SCENARIO_CATEGORY_REAL, "rct2.scenario_text.six_flags_magic_mountain"  },
        { SC_UNIDENTIFIED,             "Six Flags over Texas",      SCENARIO_CATEGORY_REAL, "rct2.scenario_text.six_flags_over_texas"      },
    };

    // Other parks
    static constexpr ScenarioTitleDescriptor kScenarioTitlesExtrasParks[] =
    {
        { SC_FORT_ANACHRONISM,                          "Fort Anachronism",                           SCENARIO_CATEGORY_DLC,            "rct1dlc.scenario_text.fort_anachronism"                      },
        { SC_PCPLAYER,                                  "PC Player",                                  SCENARIO_CATEGORY_DLC,            {}                                                            },
        { SC_PCGW,                                      "PC Gaming World",                            SCENARIO_CATEGORY_DLC,            {}                                                            },
        { SC_GAMEPLAY,                                  "gameplay",                                   SCENARIO_CATEGORY_DLC,            {}                                                            },
        { SC_UNIDENTIFIED,                              "Panda World",                                SCENARIO_CATEGORY_DLC,            "rct2dlc.scenario_text.panda_world"                           },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Belgium",           SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_belgium"         },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Great Adventure",   SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_great_adventure" },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Holland",           SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_holland"         },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Magic Mountain",    SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_magic_mountain"  },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Park",              SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_park"            },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags over Texas",        SCENARIO_CATEGORY_BUILD_YOUR_OWN, "rct2.scenario_text.build_your_own_six_flags_over_texas"      },
        { SC_UNIDENTIFIED,                              "Competition Land 1",                         SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_UNIDENTIFIED,                              "Competition Land 2",                         SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_BOBSLED_COMPETITION,                       "Bobsled Roller Coaster Competition",         SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_GO_KARTS_COMPETITION,                      "Go Karts Competition",                       SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_INVERTED_ROLLER_COASTER_COMPETITION,       "Inverted Roller Coaster Competition",        SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_MINE_TRAIN_COMPETITION,                    "Mine Train Roller Coaster Competition",      SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_STAND_UP_STEEL_ROLLER_COASTER_COMPETITION, "Stand-Up Steel Roller Coaster Competition",  SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_STEEL_CORKSCREW_COMPETITION,               "Steel Corkscrew Roller Coaster Competition", SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_STEEL_MINI_ROLLER_COASTER_COMPETITION,     "Steel Mini Roller Coaster Competition",      SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_STEEL_ROLLER_COASTER_COMPETITION,          "Steel Roller Coaster Competition",           SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_STEEL_TWISTER_COMPETITION,                 "Steel Twister Roller Coaster Competition",   SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_SUSPENDED_ROLLER_COASTER_COMPETITION,      "Suspended Roller Coaster Competition",       SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_WOODEN_ROLLER_COASTER_COMPETITION,         "Wooden Roller Coaster Competition",          SCENARIO_CATEGORY_COMPETITIONS,   {}                                                            },
        { SC_UNIDENTIFIED,                              "Tycoon Park",                                SCENARIO_CATEGORY_OTHER,          "rct2.scenario_text.tycoon_park"                              },
    };

    constexpr std::span<const ScenarioTitleDescriptor> kScenarioTitlesBySource[] = {
        kScenarioTitlesRCT1,
        kScenarioTitlesRCT1AA,
        kScenarioTitlesRCT1LL,
        kScenarioTitlesRCT2,
        kScenarioTitlesRCT2WW,
        kScenarioTitlesRCT2TT,
        kScenarioTitlesUCES,
        kScenarioTitlesRealParks,
        kScenarioTitlesExtrasParks,
    };
    // clang-format on

#pragma endregion

    bool TryGetByName(const utf8* name, SourceDescriptor* outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        int32_t currentIndex = 0;
        for (size_t i = 0; i < std::size(kScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < std::size(kScenarioTitlesBySource[i]); j++)
            {
                const ScenarioTitleDescriptor* desc = &kScenarioTitlesBySource[i][j];
                if (String::iequals(name, desc->Title))
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = static_cast<uint8_t>(i);
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    outDesc->textObjectId = desc->TextObjectId;
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
        outDesc->textObjectId = {};
        return false;
    }

    bool TryGetById(uint8_t id, SourceDescriptor* outDesc)
    {
        Guard::ArgumentNotNull(outDesc, GUARD_LINE);

        int32_t currentIndex = 0;
        for (size_t i = 0; i < std::size(kScenarioTitlesBySource); i++)
        {
            for (size_t j = 0; j < std::size(kScenarioTitlesBySource[i]); j++)
            {
                const ScenarioTitleDescriptor* desc = &kScenarioTitlesBySource[i][j];
                if (id == desc->Id)
                {
                    outDesc->title = desc->Title;
                    outDesc->id = desc->Id;
                    outDesc->source = static_cast<uint8_t>(i);
                    outDesc->index = currentIndex;
                    outDesc->category = desc->Category;
                    outDesc->textObjectId = desc->TextObjectId;
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
        outDesc->textObjectId = {};
        return false;
    }

    u8string NormaliseName(u8string_view input)
    {
        u8string normalisedName;
        // Strip "RCT(1|2)?" prefix off scenario names.
        if (input.starts_with("RCT"))
        {
            LOG_VERBOSE("Stripping RCT/1/2 from name: %s", u8string(input).c_str());
            if (input.length() >= 4 && (input[3] == '1' || input[3] == '2'))
            {
                normalisedName = input.substr(4);
            }
            else
            {
                normalisedName = input.substr(3);
            }
        }
        else
        {
            normalisedName = input;
        }

        // Trim (for the sake of the above and WW / TT scenarios)
        normalisedName = String::trimStart(normalisedName);

        // American scenario titles should be converted to British name
        // Don't worry, names will be translated using language packs later
        for (const ScenarioAlias& alias : kScenarioAliases)
        {
            if (String::equals(alias.Alternative, normalisedName))
            {
                LOG_VERBOSE("Found alias: %s; will treat as: %s", normalisedName.c_str(), alias.Original);
                return u8string(alias.Original);
            }
        }

        return normalisedName;
    }
} // namespace OpenRCT2::ScenarioSources
