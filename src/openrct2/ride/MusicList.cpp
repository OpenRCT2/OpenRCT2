/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../common.h"
#include "../Context.h"
#include "../core/Util.hpp"
#include "MusicList.h"


#define MAKE_TUNEID_LIST(...) std::vector<uint8_t>({__VA_ARGS__})

// 0x009AEF28
std::vector<uint8_t> gRideMusicStyleTuneIds[] =
{
    MAKE_TUNEID_LIST(TUNE_DODGEMS_BEAT),                             // MUSIC_STYLE_DODGEMS_BEAT
    MAKE_TUNEID_LIST(                                                // MUSIC_STYLE_FAIRGROUND_ORGAN
            TUNE_CHILDREN_OF_THE_REGIMENT,
            TUNE_SERENADE_OP_21,
            TUNE_IN_CONTINENTAL_MOOD,
            TUNE_WEDDING_JOURNEY,
            TUNE_TALES_FROM_THE_VIENNA_WOODS,
            TUNE_SLAVONIC_DANCE,
            TUNE_CSS_10,
            TUNE_DAS_ALPENHORN,
            TUNE_BELLA_BELLA_BIMBA,
            TUNE_THE_BLOND_SAILOR,
            TUNE_POET_AND_PEASANT_OVERTURE,
            TUNE_WALTZ_MEDLEY,
            TUNE_CSS_16),
    MAKE_TUNEID_LIST(TUNE_CAESARS_MARCH),                            // MUSIC_STYLE_ROMAN_FANFARE
    MAKE_TUNEID_LIST(TUNE_NINJAS_NOODLES),                           // MUSIC_STYLE_ORIENTAL
    MAKE_TUNEID_LIST(TUNE_INVADERS),                                 // MUSIC_STYLE_MARTIAN
    MAKE_TUNEID_LIST(TUNE_JUNGLE_JUICE),                             // MUSIC_STYLE_JUNGLE_DRUMS
    MAKE_TUNEID_LIST(TUNE_PHARAOHS_TOMB),                            // MUSIC_STYLE_EGYPTIAN
    MAKE_TUNEID_LIST(TUNE_ETERNAL_TOYBOX),                           // MUSIC_STYLE_TOYLAND
    MAKE_TUNEID_LIST(TUNE_CIRCUS_SHOW),                              // MUSIC_STYLE_CIRCUS_SHOW
    MAKE_TUNEID_LIST(TUNE_VOYAGE_TO_ANDROMEDA),                      // MUSIC_STYLE_SPACE
    MAKE_TUNEID_LIST(TUNE_VAMPIRES_LAIR),                            // MUSIC_STYLE_HORROR
    MAKE_TUNEID_LIST(TUNE_BRIMBLES_BEAT),                            // MUSIC_STYLE_TECHNO
    MAKE_TUNEID_LIST(TUNE_DRIFTING_TO_HEAVEN),                       // MUSIC_STYLE_GENTLE
    MAKE_TUNEID_LIST(TUNE_MID_SUMMERS_HEAT),                         // MUSIC_STYLE_SUMMER
    MAKE_TUNEID_LIST(TUNE_ATLANTIS),                                 // MUSIC_STYLE_WATER
    MAKE_TUNEID_LIST(TUNE_WILD_WEST_KID),                            // MUSIC_STYLE_WILD_WEST
    MAKE_TUNEID_LIST(TUNE_BLOCKBUSTER),                              // MUSIC_STYLE_JURASSIC
    MAKE_TUNEID_LIST(TUNE_AIRTIME_ROCK),                             // MUSIC_STYLE_ROCK
    MAKE_TUNEID_LIST(TUNE_SEARCHLIGHT_RAG),                          // MUSIC_STYLE_RAGTIME
    MAKE_TUNEID_LIST(TUNE_FLIGHT_OF_FANTASY),                        // MUSIC_STYLE_FANTASY
    MAKE_TUNEID_LIST(TUNE_BIG_ROCK),                                 // MUSIC_STYLE_ROCK_STYLE_2
    MAKE_TUNEID_LIST(TUNE_HYPOTHERMIA),                              // MUSIC_STYLE_ICE
    MAKE_TUNEID_LIST(TUNE_LAST_SLEIGH_RIDE),                         // MUSIC_STYLE_SNOW
    MAKE_TUNEID_LIST(TUNE_CUSTOM_1),                                 // MUSIC_STYLE_CUSTOM_MUSIC_1
    MAKE_TUNEID_LIST(TUNE_CUSTOM_2),                                 // MUSIC_STYLE_CUSTOM_MUSIC_2
    MAKE_TUNEID_LIST(TUNE_PIPES_OF_GLENCAIRN),                       // MUSIC_STYLE_MEDIEVAL
    MAKE_TUNEID_LIST(TUNE_TRAFFIC_JAM),                              // MUSIC_STYLE_URBAN
    MAKE_TUNEID_LIST(TUNE_TOCCATA),                                  // MUSIC_STYLE_ORGAN
    MAKE_TUNEID_LIST(TUNE_MANIC_MECHANIC),                           // MUSIC_STYLE_MECHANICAL
    MAKE_TUNEID_LIST(TUNE_TECHNO_TORTURE),                           // MUSIC_STYLE_MODERN
    MAKE_TUNEID_LIST(TUNE_WHAT_SHALL_WE_DO_WITH_THE_DRUNKEN_SAILOR), // MUSIC_STYLE_PIRATES
    MAKE_TUNEID_LIST(TUNE_SPACE_ROCK),                               // MUSIC_STYLE_ROCK_STYLE_3
    MAKE_TUNEID_LIST(TUNE_SWEAT_DREAMS),                             // MUSIC_STYLE_CANDY_STYLE
};

#define INIT_MUSIC_INFO(path_id, offset) { path_id, offset, 0 }

//0x009AF1C8
rct_ride_music_info gRideMusicInfoList[NUM_DEFAULT_MUSIC_TRACKS] = {
        INIT_MUSIC_INFO(PATH_ID_CSS4,  1378),
        INIT_MUSIC_INFO(PATH_ID_CSS5,  1378),
        INIT_MUSIC_INFO(PATH_ID_CSS6,  1378),
        INIT_MUSIC_INFO(PATH_ID_CSS7,  1378),
        INIT_MUSIC_INFO(PATH_ID_CSS8,  1378),
        INIT_MUSIC_INFO(PATH_ID_CSS9,  1378),
        INIT_MUSIC_INFO(0,             1378),         // Referred to the nearly empty CSS10.DAT file
        INIT_MUSIC_INFO(PATH_ID_CSS11, 1378),
        INIT_MUSIC_INFO(PATH_ID_CSS12, 1378),
        INIT_MUSIC_INFO(PATH_ID_CSS13, 1378),
        INIT_MUSIC_INFO(PATH_ID_CSS14, 1378),
        INIT_MUSIC_INFO(PATH_ID_CSS15, 1378),
        INIT_MUSIC_INFO(0,             1378),         // Referred to the nearly empty CSS16.DAT file
        INIT_MUSIC_INFO(PATH_ID_CSS3,   689),
        INIT_MUSIC_INFO(PATH_ID_CSS17, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS18, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS19, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS20, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS21, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS22, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS23, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS24, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS25, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS26, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS27, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS28, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS29, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS30, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS31, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS32, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS33, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS34, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS35, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS36, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS37, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS38, 2756),
        INIT_MUSIC_INFO(PATH_ID_CUSTOM1, 2756),
        INIT_MUSIC_INFO(PATH_ID_CUSTOM2, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS39, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS40, 1378),
        INIT_MUSIC_INFO(PATH_ID_CSS41, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS42, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS43, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS44, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS45, 2756),
        INIT_MUSIC_INFO(PATH_ID_CSS46, 2756),
};
