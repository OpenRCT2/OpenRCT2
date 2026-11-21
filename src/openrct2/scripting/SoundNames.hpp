/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/Audio.h"
#include "../core/EnumMap.hpp"

#include <map>
#include <string>

using namespace OpenRCT2::Audio;

namespace OpenRCT2::Scripting
{
    // When adding to this map, also add to SoundName in openrct2.d.ts

    static EnumMap<SoundId> kSoundLookupTable = {
        // Sounds
        { "lift_classic", Audio::SoundId::liftClassic },
        { "track_friction_classic_wood", Audio::SoundId::trackFrictionClassicWood },
        { "friction_classic", Audio::SoundId::frictionClassic },
        { "scream_1", Audio::SoundId::scream1 },
        { "click_1", Audio::SoundId::click1 },
        { "click_2", Audio::SoundId::click2 },
        { "place_item", Audio::SoundId::placeItem },
        { "scream_2", Audio::SoundId::scream2 },
        { "scream_3", Audio::SoundId::scream3 },
        { "scream_4", Audio::SoundId::scream4 },
        { "scream_5", Audio::SoundId::scream5 },
        { "scream_6", Audio::SoundId::scream6 },
        { "lift_friction_wheels", Audio::SoundId::liftFrictionWheels },
        { "purchase", Audio::SoundId::purchase },
        { "crash", Audio::SoundId::crash },
        { "laying_out_water", Audio::SoundId::layingOutWater },
        { "water_1", Audio::SoundId::water1 },
        { "water_2", Audio::SoundId::water2 },
        { "train_whistle", Audio::SoundId::trainWhistle },
        { "train_departing", Audio::SoundId::trainDeparting },
        { "water_splash", Audio::SoundId::waterSplash },
        { "go_kart_engine", Audio::SoundId::goKartEngine },
        { "ride_launch_1", Audio::SoundId::rideLaunch1 },
        { "ride_launch_2", Audio::SoundId::rideLaunch2 },
        { "cough_1", Audio::SoundId::cough1 },
        { "cough_2", Audio::SoundId::cough2 },
        { "cough_3", Audio::SoundId::cough3 },
        { "cough_4", Audio::SoundId::cough4 },
        { "rain", Audio::SoundId::rain },
        { "thunder_1", Audio::SoundId::thunder1 },
        { "thunder_2", Audio::SoundId::thunder2 },
        { "track_friction_train", Audio::SoundId::trackFrictionTrain },
        { "track_friction_water", Audio::SoundId::trackFrictionWater },
        { "balloon_pop", Audio::SoundId::balloonPop },
        { "mechanic_fix", Audio::SoundId::mechanicFix },
        { "scream_7", Audio::SoundId::scream7 },
        { "toilet_flush", Audio::SoundId::toiletFlush },
        { "click_3", Audio::SoundId::click3 },
        { "quack", Audio::SoundId::quack },
        { "news_item", Audio::SoundId::newsItem },
        { "window_open", Audio::SoundId::windowOpen },
        { "laugh_1", Audio::SoundId::laugh1 },
        { "laugh_2", Audio::SoundId::laugh2 },
        { "laugh_3", Audio::SoundId::laugh3 },
        { "applause", Audio::SoundId::applause },
        { "haunted_house_scare", Audio::SoundId::hauntedHouseScare },
        { "haunted_house_scream_1", Audio::SoundId::hauntedHouseScream1 },
        { "haunted_house_scream_2", Audio::SoundId::hauntedHouseScream2 },
        { "block_brake_close", Audio::SoundId::blockBrakeClose },
        { "block_brake_release", Audio::SoundId::blockBrakeRelease },
        { "error", Audio::SoundId::error },
        { "brake_release", Audio::SoundId::brakeRelease },
        { "lift_arrow", Audio::SoundId::liftArrow },
        { "lift_wood", Audio::SoundId::liftWood },
        { "track_friction_wood", Audio::SoundId::trackFrictionWood },
        { "lift_wild_mouse", Audio::SoundId::liftWildMouse },
        { "lift_bm", Audio::SoundId::liftBM },
        { "track_friction_bm", Audio::SoundId::trackFrictionBM },
        { "scream_8", Audio::SoundId::scream8 },
        { "tram", Audio::SoundId::tram },
        { "door_open", Audio::SoundId::doorOpen },
        { "door_close", Audio::SoundId::doorClose },
        { "portcullis", Audio::SoundId::portcullis },
        { "crowd_ambience", Audio::SoundId::crowdAmbience },
        { "lift_rmc", Audio::SoundId::liftRMC },
        { "track_friction_rmc", Audio::SoundId::trackFrictionRMC },
        { "lift_flume", Audio::SoundId::liftFlume },
        { "null", SoundId::null }
    };

    inline SoundId getSoundByName(const std::string& input)
    {
        auto result = kSoundLookupTable.find(input);
        if (result != kSoundLookupTable.end())
            return result->second;
        return SoundId::null;
    }

} // namespace OpenRCT2::Scripting
