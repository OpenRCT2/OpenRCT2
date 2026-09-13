/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "ScenarioCategory.h"
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
        const Scenario::Category Category;
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
        { SC_FOREST_FRONTIERS,  "Forest Frontiers",  Scenario::Category::beginner,    "rct1.scenario_meta.forest_frontiers"  },
        { SC_DYNAMITE_DUNES,    "Dynamite Dunes",    Scenario::Category::beginner,    "rct1.scenario_meta.dynamite_dunes"    },
        { SC_LEAFY_LAKE,        "Leafy Lake",        Scenario::Category::beginner,    "rct1.scenario_meta.leafy_lake"        },
        { SC_DIAMOND_HEIGHTS,   "Diamond Heights",   Scenario::Category::beginner,    "rct1.scenario_meta.diamond_heights"   },
        { SC_EVERGREEN_GARDENS, "Evergreen Gardens", Scenario::Category::beginner,    "rct1.scenario_meta.evergreen_gardens" },
        { SC_BUMBLY_BEACH,      "Bumbly Beach",      Scenario::Category::beginner,    "rct1.scenario_meta.bumbly_beach"      },
        { SC_TRINITY_ISLANDS,   "Trinity Islands",   Scenario::Category::challenging, "rct1.scenario_meta.trinity_islands"   },
        { SC_KATIES_DREAMLAND,  "Katie's Dreamland", Scenario::Category::challenging, "rct1.scenario_meta.katies_dreamland"  },
        { SC_POKEY_PARK,        "Pokey Park",        Scenario::Category::challenging, "rct1.scenario_meta.pokey_park"        },
        { SC_WHITE_WATER_PARK,  "White Water Park",  Scenario::Category::challenging, "rct1.scenario_meta.white_water_park"  },
        { SC_MILLENNIUM_MINES,  "Millennium Mines",  Scenario::Category::challenging, "rct1.scenario_meta.millennium_mines"  },
        { SC_KARTS_COASTERS,    "Karts & Coasters",  Scenario::Category::challenging, "rct1.scenario_meta.karts_coasters"    },
        { SC_MELS_WORLD,        "Mel's World",       Scenario::Category::challenging, "rct1.scenario_meta.mels_world"        },
        { SC_MYSTIC_MOUNTAIN,   "Mystic Mountain",   Scenario::Category::challenging, "rct1.scenario_meta.mystic_mountain"   },
        { SC_PACIFIC_PYRAMIDS,  "Pacific Pyramids",  Scenario::Category::challenging, "rct1.scenario_meta.pacific_pyramids"  },
        { SC_CRUMBLY_WOODS,     "Crumbly Woods",     Scenario::Category::challenging, "rct1.scenario_meta.crumbly_woods"     },
        { SC_PARADISE_PIER,     "Paradise Pier",     Scenario::Category::challenging, "rct1.scenario_meta.paradise_pier"     },
        { SC_LIGHTNING_PEAKS,   "Lightning Peaks",   Scenario::Category::expert,      "rct1.scenario_meta.lightning_peaks"   },
        { SC_IVORY_TOWERS,      "Ivory Towers",      Scenario::Category::expert,      "rct1.scenario_meta.ivory_towers"      },
        { SC_RAINBOW_VALLEY,    "Rainbow Valley",    Scenario::Category::expert,      "rct1.scenario_meta.rainbow_valley"    },
        { SC_THUNDER_ROCK,      "Thunder Rock",      Scenario::Category::expert,      "rct1.scenario_meta.thunder_rock"      },
        { SC_MEGA_PARK,         "Mega Park",         Scenario::Category::other,       "rct1.scenario_meta.mega_park"         },
    };

    // RCT: Added Attractions
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT1AA[] =
    {
        { SC_WHISPERING_CLIFFS,  "Whispering Cliffs",  Scenario::Category::beginner,    "rct1aa.scenario_meta.whispering_cliffs"  },
        { SC_THREE_MONKEYS_PARK, "Three Monkeys Park", Scenario::Category::beginner,    "rct1aa.scenario_meta.three_monkeys_park" },
        { SC_CANARY_MINES,       "Canary Mines",       Scenario::Category::beginner,    "rct1aa.scenario_meta.canary_mines"       },
        { SC_BARONY_BRIDGE,      "Barony Bridge",      Scenario::Category::beginner,    "rct1aa.scenario_meta.barony_bridge"      },
        { SC_FUNTOPIA,           "Funtopia",           Scenario::Category::beginner,    "rct1aa.scenario_meta.funtopia"           },
        { SC_HAUNTED_HARBOUR,    "Haunted Harbour",    Scenario::Category::beginner,    "rct1aa.scenario_meta.haunted_harbour"    },
        { SC_FUN_FORTRESS,       "Fun Fortress",       Scenario::Category::beginner,    "rct1aa.scenario_meta.fun_fortress"       },
        { SC_FUTURE_WORLD,       "Future World",       Scenario::Category::beginner,    "rct1aa.scenario_meta.future_world"       },
        { SC_GENTLE_GLEN,        "Gentle Glen",        Scenario::Category::beginner,    "rct1aa.scenario_meta.gentle_glen"        },
        { SC_JOLLY_JUNGLE,       "Jolly Jungle",       Scenario::Category::challenging, "rct1aa.scenario_meta.jolly_jungle"       },
        { SC_HYDRO_HILLS,        "Hydro Hills",        Scenario::Category::challenging, "rct1aa.scenario_meta.hydro_hills"        },
        { SC_SPRIGHTLY_PARK,     "Sprightly Park",     Scenario::Category::challenging, "rct1aa.scenario_meta.sprightly_park"     },
        { SC_MAGIC_QUARTERS,     "Magic Quarters",     Scenario::Category::challenging, "rct1aa.scenario_meta.magic_quarters"     },
        { SC_FRUIT_FARM,         "Fruit Farm",         Scenario::Category::challenging, "rct1aa.scenario_meta.fruit_farm"         },
        { SC_BUTTERFLY_DAM,      "Butterfly Dam",      Scenario::Category::challenging, "rct1aa.scenario_meta.butterfly_dam"      },
        { SC_COASTER_CANYON,     "Coaster Canyon",     Scenario::Category::challenging, "rct1aa.scenario_meta.coaster_canyon"     },
        { SC_THUNDERSTORM_PARK,  "Thunderstorm Park",  Scenario::Category::challenging, "rct1aa.scenario_meta.thunderstorm_park"  },
        { SC_HARMONIC_HILLS,     "Harmonic Hills",     Scenario::Category::challenging, "rct1aa.scenario_meta.harmonic_hills"     },
        { SC_ROMAN_VILLAGE,      "Roman Village",      Scenario::Category::challenging, "rct1aa.scenario_meta.roman_village"      },
        { SC_SWAMP_COVE,         "Swamp Cove",         Scenario::Category::challenging, "rct1aa.scenario_meta.swamp_cove"         },
        { SC_ADRENALINE_HEIGHTS, "Adrenaline Heights", Scenario::Category::challenging, "rct1aa.scenario_meta.adrenaline_heights" },
        { SC_UTOPIA_PARK,        "Utopia Park",        Scenario::Category::challenging, "rct1aa.scenario_meta.utopia_park"        },
        { SC_ROTTING_HEIGHTS,    "Rotting Heights",    Scenario::Category::expert,      "rct1aa.scenario_meta.rotting_heights"    },
        { SC_FIASCO_FOREST,      "Fiasco Forest",      Scenario::Category::expert,      "rct1aa.scenario_meta.fiasco_forest"      },
        { SC_PICKLE_PARK,        "Pickle Park",        Scenario::Category::expert,      "rct1aa.scenario_meta.pickle_park"        },
        { SC_GIGGLE_DOWNS,       "Giggle Downs",       Scenario::Category::expert,      "rct1aa.scenario_meta.giggle_downs"       },
        { SC_MINERAL_PARK,       "Mineral Park",       Scenario::Category::expert,      "rct1aa.scenario_meta.mineral_park"       },
        { SC_COASTER_CRAZY,      "Coaster Crazy",      Scenario::Category::expert,      "rct1aa.scenario_meta.coaster_crazy"      },
        { SC_URBAN_PARK,         "Urban Park",         Scenario::Category::expert,      "rct1aa.scenario_meta.urban_park"         },
        { SC_GEOFFREY_GARDENS,   "Geoffrey Gardens",   Scenario::Category::expert,      "rct1aa.scenario_meta.geoffrey_gardens"   },
    };

    // RCT: Loopy Landscapes
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT1LL[] =
    {
        { SC_ICEBERG_ISLANDS,  "Iceberg Islands",  Scenario::Category::beginner,    "rct1ll.scenario_meta.iceberg_islands"  },
        { SC_VOLCANIA,         "Volcania",         Scenario::Category::beginner,    "rct1ll.scenario_meta.volcania"         },
        { SC_ARID_HEIGHTS,     "Arid Heights",     Scenario::Category::beginner,    "rct1ll.scenario_meta.arid_heights"     },
        { SC_RAZOR_ROCKS,      "Razor Rocks",      Scenario::Category::beginner,    "rct1ll.scenario_meta.razor_rocks"      },
        { SC_CRATER_LAKE,      "Crater Lake",      Scenario::Category::beginner,    "rct1ll.scenario_meta.crater_lake"      },
        { SC_VERTIGO_VIEWS,    "Vertigo Views",    Scenario::Category::beginner,    "rct1ll.scenario_meta.vertigo_views"    },
        { SC_PARADISE_PIER_2,  "Paradise Pier 2",  Scenario::Category::challenging, "rct1ll.scenario_meta.paradise_pier_2"  },
        { SC_DRAGONS_COVE,     "Dragon's Cove",    Scenario::Category::challenging, "rct1ll.scenario_meta.dragons_cove"     },
        { SC_GOOD_KNIGHT_PARK, "Good Knight Park", Scenario::Category::challenging, "rct1ll.scenario_meta.good_knight_park" },
        { SC_WACKY_WARREN,     "Wacky Warren",     Scenario::Category::challenging, "rct1ll.scenario_meta.wacky_warren"     },
        { SC_GRAND_GLACIER,    "Grand Glacier",    Scenario::Category::challenging, "rct1ll.scenario_meta.grand_glacier"    },
        { SC_CRAZY_CRATERS,    "Crazy Craters",    Scenario::Category::challenging, "rct1ll.scenario_meta.crazy_craters"    },
        { SC_DUSTY_DESERT,     "Dusty Desert",     Scenario::Category::challenging, "rct1ll.scenario_meta.dusty_desert"     },
        { SC_WOODWORM_PARK,    "Woodworm Park",    Scenario::Category::challenging, "rct1ll.scenario_meta.woodworm_park"    },
        { SC_ICARUS_PARK,      "Icarus Park",      Scenario::Category::challenging, "rct1ll.scenario_meta.icarus_park"      },
        { SC_SUNNY_SWAMPS,     "Sunny Swamps",     Scenario::Category::challenging, "rct1ll.scenario_meta.sunny_swamps"     },
        { SC_FRIGHTMARE_HILLS, "Frightmare Hills", Scenario::Category::challenging, "rct1ll.scenario_meta.frightmare_hills" },
        { SC_THUNDER_ROCKS,    "Thunder Rocks",    Scenario::Category::challenging, "rct1ll.scenario_meta.thunder_rocks"    },
        { SC_OCTAGON_PARK,     "Octagon Park",     Scenario::Category::challenging, "rct1ll.scenario_meta.octagon_park"     },
        { SC_PLEASURE_ISLAND,  "Pleasure Island",  Scenario::Category::challenging, "rct1ll.scenario_meta.pleasure_island"  },
        { SC_ICICLE_WORLDS,    "Icicle Worlds",    Scenario::Category::challenging, "rct1ll.scenario_meta.icicle_worlds"    },
        { SC_SOUTHERN_SANDS,   "Southern Sands",   Scenario::Category::expert,      "rct1ll.scenario_meta.southern_sands"   },
        { SC_TINY_TOWERS,      "Tiny Towers",      Scenario::Category::expert,      "rct1ll.scenario_meta.tiny_towers"      },
        { SC_NEVERMORE_PARK,   "Nevermore Park",   Scenario::Category::expert,      "rct1ll.scenario_meta.nevermore_park"   },
        { SC_PACIFICA,         "Pacifica",         Scenario::Category::expert,      "rct1ll.scenario_meta.pacifica"         },
        { SC_URBAN_JUNGLE,     "Urban Jungle",     Scenario::Category::expert,      "rct1ll.scenario_meta.urban_jungle"     },
        { SC_TERROR_TOWN,      "Terror Town",      Scenario::Category::expert,      "rct1ll.scenario_meta.terror_town"      },
        { SC_MEGAWORLD_PARK,   "Megaworld Park",   Scenario::Category::expert,      "rct1ll.scenario_meta.megaworld_park"   },
        { SC_VENUS_PONDS,      "Venus Ponds",      Scenario::Category::expert,      "rct1ll.scenario_meta.venus_ponds"      },
        { SC_MICRO_PARK,       "Micro Park",       Scenario::Category::expert,      "rct1ll.scenario_meta.micro_park"       },
    };

    // RCT2
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2[] =
    {
        { SC_UNIDENTIFIED, "Electric Fields",   Scenario::Category::beginner,    "rct2.scenario_meta.electric_fields"   },
        { SC_UNIDENTIFIED, "Factory Capers",    Scenario::Category::beginner,    "rct2.scenario_meta.factory_capers"    },
        { SC_UNIDENTIFIED, "Crazy Castle",      Scenario::Category::beginner,    "rct2.scenario_meta.crazy_castle"      },
        { SC_UNIDENTIFIED, "Dusty Greens",      Scenario::Category::beginner,    "rct2.scenario_meta.dusty_greens"      },
        { SC_UNIDENTIFIED, "Bumbly Bazaar",     Scenario::Category::beginner,    "rct2.scenario_meta.bumbly_bazaar"     },
        { SC_UNIDENTIFIED, "Infernal Views",    Scenario::Category::challenging, "rct2.scenario_meta.infernal_views"    },
        { SC_UNIDENTIFIED, "Lucky Lake",        Scenario::Category::challenging, "rct2.scenario_meta.lucky_lake"        },
        { SC_UNIDENTIFIED, "Botany Breakers",   Scenario::Category::challenging, "rct2.scenario_meta.botany_breakers"   },
        { SC_UNIDENTIFIED, "Alpine Adventures", Scenario::Category::challenging, "rct2.scenario_meta.alpine_adventures" },
        { SC_UNIDENTIFIED, "Gravity Gardens",   Scenario::Category::expert,      "rct2.scenario_meta.gravity_gardens"   },
        { SC_UNIDENTIFIED, "Extreme Heights",   Scenario::Category::expert,      "rct2.scenario_meta.extreme_heights"   },
        { SC_UNIDENTIFIED, "Amity Airfield",    Scenario::Category::expert,      "rct2.scenario_meta.amity_airfield"    },
        { SC_UNIDENTIFIED, "Ghost Town",        Scenario::Category::expert,      "rct2.scenario_meta.ghost_town"        },
        { SC_UNIDENTIFIED, "Fungus Woods",      Scenario::Category::expert,      "rct2.scenario_meta.fungus_woods"      },
        { SC_UNIDENTIFIED, "Rainbow Summit",    Scenario::Category::expert,      "rct2.scenario_meta.rainbow_summit"    },
    };

    // RCT2: Wacky Worlds
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2WW[] =
    {
        { SC_UNIDENTIFIED, "North America - Grand Canyon",                   Scenario::Category::beginner,    "rct2ww.scenario_meta.canyon_calamities"     },
        { SC_UNIDENTIFIED, "Asia - Great Wall of China Tourism Enhancement", Scenario::Category::beginner,    "rct2ww.scenario_meta.great_wall_of_china"   },
        { SC_UNIDENTIFIED, "Africa - African Diamond Mine",                  Scenario::Category::beginner,    "rct2ww.scenario_meta.mines_of_africa"       },
        { SC_UNIDENTIFIED, "Australasia - Ayers Rock",                       Scenario::Category::beginner,    "rct2ww.scenario_meta.ayers_adventure"       },
        { SC_UNIDENTIFIED, "North America - Rollercoaster Heaven",           Scenario::Category::beginner,    "rct2ww.scenario_meta.rollercoaster_heaven"  },
        { SC_UNIDENTIFIED, "Africa - Oasis",                                 Scenario::Category::challenging, "rct2ww.scenario_meta.mirage_madness"        },
        { SC_UNIDENTIFIED, "South America - Rio Carnival",                   Scenario::Category::challenging, "rct2ww.scenario_meta.sugarloaf_shores"      },
        { SC_UNIDENTIFIED, "Asia - Maharaja Palace",                         Scenario::Category::challenging, "rct2ww.scenario_meta.park_maharaja"         },
        { SC_UNIDENTIFIED, "Africa - Victoria Falls",                        Scenario::Category::challenging, "rct2ww.scenario_meta.over_the_edge"         },
        { SC_UNIDENTIFIED, "N. America - Extreme Hawaiian Island",           Scenario::Category::challenging, "rct2ww.scenario_meta.wacky_waikiki"         },
        { SC_UNIDENTIFIED, "South America - Rain Forest Plateau",            Scenario::Category::challenging, "rct2ww.scenario_meta.rainforest_romp"       },
        { SC_UNIDENTIFIED, "Europe - Renovation",                            Scenario::Category::challenging, "rct2ww.scenario_meta.from_the_ashes"        },
        { SC_UNIDENTIFIED, "Antarctic - Ecological Salvage",                 Scenario::Category::expert,      "rct2ww.scenario_meta.icy_adventures"        },
        { SC_UNIDENTIFIED, "Europe - European Cultural Festival",            Scenario::Category::expert,      "rct2ww.scenario_meta.european_extravaganza" },
        { SC_UNIDENTIFIED, "Australasia - Fun at the Beach",                 Scenario::Category::expert,      "rct2ww.scenario_meta.beach_barbecue_blast"  },
        { SC_UNIDENTIFIED, "South America - Inca Lost City",                 Scenario::Category::expert,      "rct2ww.scenario_meta.lost_city_founder"     },
        { SC_UNIDENTIFIED, "Asia - Japanese Coastal Reclaim",                Scenario::Category::expert,      "rct2ww.scenario_meta.okinawa_coast"         },
    };

    // RCT2: Time Twister
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRCT2TT[] =
    {
        { SC_UNIDENTIFIED, "Dark Age - Robin Hood",                 Scenario::Category::beginner,    "rct2tt.scenario_meta.sherwood_forest"               },
        { SC_UNIDENTIFIED, "Prehistoric - After the Asteroid",      Scenario::Category::beginner,    "rct2tt.scenario_meta.crater_carnage"                },
        { SC_UNIDENTIFIED, "Roaring Twenties - Prison Island",      Scenario::Category::beginner,    "rct2tt.scenario_meta.alcatraz"                      },
        { SC_UNIDENTIFIED, "Future - First Encounters",             Scenario::Category::beginner,    "rct2tt.scenario_meta.extraterrestrial_extravaganza" },
        { SC_UNIDENTIFIED, "Roaring Twenties - Schneider Cup",      Scenario::Category::challenging, "rct2tt.scenario_meta.schneider_shores"              },
        { SC_UNIDENTIFIED, "Prehistoric - Stone Age",               Scenario::Category::challenging, "rct2tt.scenario_meta.rocky_rambles"                 },
        { SC_UNIDENTIFIED, "Mythological - Cradle of Civilisation", Scenario::Category::challenging, "rct2tt.scenario_meta.mythological_madness"          },
        { SC_UNIDENTIFIED, "Rock 'n' Roll - Rock 'n' Roll",         Scenario::Category::challenging, "rct2tt.scenario_meta.rock_n_roll_revival"           },
        { SC_UNIDENTIFIED, "Future - Future World",                 Scenario::Category::challenging, "rct2tt.scenario_meta.gemini_city"                   },
        { SC_UNIDENTIFIED, "Roaring Twenties - Skyscrapers",        Scenario::Category::expert,      "rct2tt.scenario_meta.metropolis"                    },
        { SC_UNIDENTIFIED, "Rock 'n' Roll - Flower Power",          Scenario::Category::expert,      "rct2tt.scenario_meta.woodstock"                     },
        { SC_UNIDENTIFIED, "Dark Age - Castle",                     Scenario::Category::expert,      "rct2tt.scenario_meta.cliffside_castle"              },
        { SC_UNIDENTIFIED, "Mythological - Animatronic Film Set",   Scenario::Category::expert,      "rct2tt.scenario_meta.animatronic_antics"            },
        { SC_UNIDENTIFIED, "Prehistoric - Jurassic Safari",         Scenario::Category::expert,      "rct2tt.scenario_meta.coastersaurus"                 },
    };

    // User Created Expansion Sets
    static constexpr ScenarioTitleDescriptor kScenarioTitlesUCES[] =
    {
        { SC_UNIDENTIFIED, "Lighthouse of Alexandria by Katatude for UCES",    Scenario::Category::timeMachine,     "uces.scenario_meta.the_lighthouse_of_alexandria" },
        { SC_UNIDENTIFIED, "Cleveland's Luna Park",                            Scenario::Category::timeMachine,     "uces.scenario_meta.luna_park_cleveland"          },
        { SC_UNIDENTIFIED, "Mount Vesuvius 1700 A.D. by Katatude for UCES",    Scenario::Category::timeMachine,     "uces.scenario_meta.mount_vesuvius"               },
        { SC_UNIDENTIFIED, "The Sandbox by Katatude for UCES",                 Scenario::Category::timeMachine,     "uces.scenario_meta.the_sandbox"                  },
        { SC_UNIDENTIFIED, "Niagara Falls & Gorge by Katatude for UCES",       Scenario::Category::timeMachine,     "uces.scenario_meta.niagara_falls_gorge"          },
        { SC_UNIDENTIFIED, "Rocky Mountain Miners",                            Scenario::Category::timeMachine,     "uces.scenario_meta.rocky_mountain_miners"        },
        { SC_UNIDENTIFIED, "The Time Machine by Katatude for UCES",            Scenario::Category::timeMachine,     "uces.scenario_meta.the_time_machine"             },
        { SC_UNIDENTIFIED, "Tower of Babel",                                   Scenario::Category::timeMachine,     "uces.scenario_meta.tower_of_babel"               },
        { SC_UNIDENTIFIED, "Transformation",                                   Scenario::Category::timeMachine,     "uces.scenario_meta.transformation"               },
        { SC_UNIDENTIFIED, "Urbis Incognitus",                                 Scenario::Category::timeMachine,     "uces.scenario_meta.urbis_incognitus"             },
        { SC_UNIDENTIFIED, "Beneath the Christmas Tree by Katatude for UCES",  Scenario::Category::katysDreamworld, "uces.scenario_meta.beneath_the_christmas_tree"   },
        { SC_UNIDENTIFIED, "Bigrock Blast",                                    Scenario::Category::katysDreamworld, "uces.scenario_meta.bigrock_blast"                },
        { SC_UNIDENTIFIED, "Camp Mockingbird for UCES by Katatude",            Scenario::Category::katysDreamworld, "uces.scenario_meta.camp_mockingbird"             },
        { SC_UNIDENTIFIED, "Choo Choo Town",                                   Scenario::Category::katysDreamworld, "uces.scenario_meta.choochoo_town"                },
        { SC_UNIDENTIFIED, "Dragon Islands",                                   Scenario::Category::katysDreamworld, "uces.scenario_meta.dragon_islands"               },
        { SC_UNIDENTIFIED, "Kiddy Karnival II",                                Scenario::Category::katysDreamworld, "uces.scenario_meta.kiddie_karnival_ii"           },
        { SC_UNIDENTIFIED, "Sand Dune",                                        Scenario::Category::katysDreamworld, "uces.scenario_meta.sand_dune"                    },
        { SC_UNIDENTIFIED, "UCES Halloween",                                   Scenario::Category::other,            "uces.scenario_meta.cemetery_ridge"               },
    };

    // Real parks
    static constexpr ScenarioTitleDescriptor kScenarioTitlesRealParks[] =
    {
        { SC_ALTON_TOWERS,             "Alton Towers",              Scenario::Category::real, "rct1ll.scenario_meta.alton_towers"            },
        { SC_HEIDE_PARK,               "Heide-Park",                Scenario::Category::real, "rct1ll.scenario_meta.heidepark"               },
        { SC_BLACKPOOL_PLEASURE_BEACH, "Blackpool Pleasure Beach",  Scenario::Category::real, "rct1ll.scenario_meta.blackpool_pleasure_beach"  },
        { SC_UNIDENTIFIED,             "Six Flags Belgium",         Scenario::Category::real, "rct2.scenario_meta.six_flags_belgium"         },
        { SC_UNIDENTIFIED,             "Six Flags Great Adventure", Scenario::Category::real, "rct2.scenario_meta.six_flags_great_adventure" },
        { SC_UNIDENTIFIED,             "Six Flags Holland",         Scenario::Category::real, "rct2.scenario_meta.six_flags_holland"         },
        { SC_UNIDENTIFIED,             "Six Flags Magic Mountain",  Scenario::Category::real, "rct2.scenario_meta.six_flags_magic_mountain"  },
        { SC_UNIDENTIFIED,             "Six Flags over Texas",      Scenario::Category::real, "rct2.scenario_meta.six_flags_over_texas"      },
    };

    // Other parks
    static constexpr ScenarioTitleDescriptor kScenarioTitlesExtrasParks[] =
    {
        { SC_FORT_ANACHRONISM,                          "Fort Anachronism",                           Scenario::Category::dlc,            "rct1dlc.scenario_meta.fort_anachronism"                           },
        { SC_TAIWAN_PARK,                               "Taiwan Park",                                Scenario::Category::dlc,            "rct1tw.scenario_meta.taiwan_park"                                 },
        { SC_PCPLAYER,                                  "PC Player",                                  Scenario::Category::dlc,            "rct1dlc.scenario_meta.pc_player"                                  },
        { SC_PCGW,                                      "PC Gaming World",                            Scenario::Category::dlc,            "rct1dlc.scenario_meta.pc_gaming_world"                            },
        { SC_GAMEPLAY,                                  "gameplay",                                   Scenario::Category::dlc,            "rct1dlc.scenario_meta.gameplay"                                   },
        { SC_UNIDENTIFIED,                              "Panda World",                                Scenario::Category::dlc,            "rct2dlc.scenario_meta.panda_world"                                },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Belgium",           Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_belgium"              },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Great Adventure",   Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_great_adventure"      },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Holland",           Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_holland"              },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Magic Mountain",    Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_magic_mountain"       },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags Park",              Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_park"                 },
        { SC_UNIDENTIFIED,                              "Build your own Six Flags over Texas",        Scenario::Category::buildYourOwn,   "rct2.scenario_meta.build_your_own_six_flags_over_texas"           },
        { SC_UNIDENTIFIED,                              "Competition Land 1",                         Scenario::Category::competitions,   "rct1dlc.scenario_meta.competition_land_1"                         },
        { SC_UNIDENTIFIED,                              "Competition Land 2",                         Scenario::Category::competitions,   "rct1dlc.scenario_meta.competition_land_2"                         },
        { SC_BOBSLED_COMPETITION,                       "Bobsled Roller Coaster Competition",         Scenario::Category::competitions,   "rct1dlc.scenario_meta.bobsled_roller_coaster_competition"         },
        { SC_GO_KARTS_COMPETITION,                      "Go Karts Competition",                       Scenario::Category::competitions,   "rct1dlc.scenario_meta.go_karts_competition"                       },
        { SC_INVERTED_ROLLER_COASTER_COMPETITION,       "Inverted Roller Coaster Competition",        Scenario::Category::competitions,   "rct1dlc.scenario_meta.inverted_roller_coaster_competition"        },
        { SC_MINE_TRAIN_COMPETITION,                    "Mine Train Roller Coaster Competition",      Scenario::Category::competitions,   "rct1dlc.scenario_meta.mine_train_roller_coaster_competition"      },
        { SC_STAND_UP_STEEL_ROLLER_COASTER_COMPETITION, "Stand-Up Steel Roller Coaster Competition",  Scenario::Category::competitions,   "rct1dlc.scenario_meta.stand_up_steel_roller_coaster_competition"  },
        { SC_STEEL_CORKSCREW_COMPETITION,               "Steel Corkscrew Roller Coaster Competition", Scenario::Category::competitions,   "rct1dlc.scenario_meta.steel_corkscrew_roller_coaster_competition" },
        { SC_STEEL_MINI_ROLLER_COASTER_COMPETITION,     "Steel Mini-Roller Coaster Competition",      Scenario::Category::competitions,   "rct1dlc.scenario_meta.steel_mini_roller_coaster_competition"      },
        { SC_STEEL_ROLLER_COASTER_COMPETITION,          "Steel Roller Coaster Competition",           Scenario::Category::competitions,   "rct1dlc.scenario_meta.steel_roller_coaster_competition"           },
        { SC_STEEL_TWISTER_COMPETITION,                 "Steel Twister Roller Coaster Competition",   Scenario::Category::competitions,   "rct1dlc.scenario_meta.steel_twister_roller_coaster_competition"   },
        { SC_SUSPENDED_ROLLER_COASTER_COMPETITION,      "Suspended Roller Coaster Competition",       Scenario::Category::competitions,   "rct1dlc.scenario_meta.suspended_roller_coaster_competition"       },
        { SC_WOODEN_ROLLER_COASTER_COMPETITION,         "Wooden Roller Coaster Competition",          Scenario::Category::competitions,   "rct1dlc.scenario_meta.wooden_roller_coaster_competition"          },
        { SC_UNIDENTIFIED,                              "Tycoon Park",                                Scenario::Category::other,          "rct2.scenario_meta.tycoon_park"                                   },
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

    bool TryGetByName(u8string_view name, SourceDescriptor* outDesc)
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
        outDesc->category = Scenario::Category::other;
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
        outDesc->category = Scenario::Category::other;
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
