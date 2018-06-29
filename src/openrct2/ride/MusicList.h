/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../audio/audio.h"
#include "../common.h"

enum
{
    TUNE_CHILDREN_OF_THE_REGIMENT,                  // 00
    TUNE_SERENADE_OP_21,                            // 01
    TUNE_IN_CONTINENTAL_MOOD,                       // 02
    TUNE_WEDDING_JOURNEY,                           // 03
    TUNE_TALES_FROM_THE_VIENNA_WOODS,               // 04
    TUNE_SLAVONIC_DANCE,                            // 05
    TUNE_CSS_10,                                    // 06, empty
    TUNE_DAS_ALPENHORN,                             // 07
    TUNE_BELLA_BELLA_BIMBA,                         // 08
    TUNE_THE_BLOND_SAILOR,                          // 09
    TUNE_POET_AND_PEASANT_OVERTURE,                 // 10
    TUNE_WALTZ_MEDLEY,                              // 11
    TUNE_CSS_16,                                    // 12, empty
    TUNE_DODGEMS_BEAT,                              // 13
    TUNE_RCT2_THEME_MUSIC,                          // 14
    TUNE_CAESARS_MARCH,                             // 15
    TUNE_NINJAS_NOODLES,                            // 16
    TUNE_INVADERS,                                  // 17
    TUNE_JUNGLE_JUICE,                              // 18
    TUNE_PHARAOHS_TOMB,                             // 19
    TUNE_ETERNAL_TOYBOX,                            // 20
    TUNE_CIRCUS_SHOW,                               // 21
    TUNE_VOYAGE_TO_ANDROMEDA,                       // 22
    TUNE_VAMPIRES_LAIR,                             // 23
    TUNE_BRIMBLES_BEAT,                             // 24
    TUNE_DRIFTING_TO_HEAVEN,                        // 25
    TUNE_MID_SUMMERS_HEAT,                          // 26
    TUNE_ATLANTIS,                                  // 27
    TUNE_WILD_WEST_KID,                             // 28
    TUNE_BLOCKBUSTER,                               // 29
    TUNE_AIRTIME_ROCK,                              // 30
    TUNE_SEARCHLIGHT_RAG,                           // 31
    TUNE_FLIGHT_OF_FANTASY,                         // 32
    TUNE_BIG_ROCK,                                  // 33
    TUNE_HYPOTHERMIA,                               // 34
    TUNE_LAST_SLEIGH_RIDE,                          // 35
    TUNE_CUSTOM_1,                                  // 36
    TUNE_CUSTOM_2,                                  // 37
    TUNE_PIPES_OF_GLENCAIRN,                        // 38
    TUNE_TRAFFIC_JAM,                               // 39
    TUNE_TOCCATA,                                   // 40
    TUNE_MANIC_MECHANIC,                            // 41
    TUNE_TECHNO_TORTURE,                            // 42
    TUNE_WHAT_SHALL_WE_DO_WITH_THE_DRUNKEN_SAILOR,  // 43
    TUNE_SPACE_ROCK,                                // 44
    TUNE_SWEAT_DREAMS,                              // 45, (sic)
};

extern std::vector<uint8_t> gRideMusicStyleTuneIds[];
