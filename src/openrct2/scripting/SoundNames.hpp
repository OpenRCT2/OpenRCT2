/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"

#include <map>
#include <openrct2/audio/Audio.h>
#include <string>

using namespace OpenRCT2::Audio;

namespace OpenRCT2::Scripting
{
    // When adding to this map, also add to SoundName in openrct2.d.ts

    static EnumMap<SoundId> SoundLookupTable = {
        // Sounds
        { "lift_classic", SoundId::LiftClassic },
        { "track_friction_classic_wood", SoundId::TrackFrictionClassicWood },
        { "friction_classic", SoundId::FrictionClassic },
        { "scream_1", SoundId::Scream1 },
        { "click_1", SoundId::Click1 },
        { "click_2", SoundId::Click2 },
        { "place_item", SoundId::PlaceItem },
        { "scream_2", SoundId::Scream2 },
        { "scream_3", SoundId::Scream3 },
        { "scream_4", SoundId::Scream4 },
        { "scream_5", SoundId::Scream5 },
        { "scream_6", SoundId::Scream6 },
        { "lift_friction_wheels", SoundId::LiftFrictionWheels },
        { "purchase", SoundId::Purchase },
        { "crash", SoundId::Crash },
        { "laying_out_water", SoundId::LayingOutWater },
        { "water_1", SoundId::Water1 },
        { "water_2", SoundId::Water2 },
        { "train_whistle", SoundId::TrainWhistle },
        { "train_departing", SoundId::TrainDeparting },
        { "water_splash", SoundId::WaterSplash },
        { "go_kart_engine", SoundId::GoKartEngine },
        { "ride_launch_1", SoundId::RideLaunch1 },
        { "ride_launch_2", SoundId::RideLaunch2 },
        { "cough_1", SoundId::Cough1 },
        { "cough_2", SoundId::Cough2 },
        { "cough_3", SoundId::Cough3 },
        { "cough_4", SoundId::Cough4 },
        { "rain", SoundId::Rain },
        { "thunder_1", SoundId::Thunder1 },
        { "thunder_2", SoundId::Thunder2 },
        { "track_friction_train", SoundId::TrackFrictionTrain },
        { "track_friction_water", SoundId::TrackFrictionWater },
        { "balloon_pop", SoundId::BalloonPop },
        { "mechanic_fix", SoundId::MechanicFix },
        { "scream_7", SoundId::Scream7 },
        { "toilet_flush", SoundId::ToiletFlush },
        { "click_3", SoundId::Click3 },
        { "quack", SoundId::Quack },
        { "news_item", SoundId::NewsItem },
        { "window_open", SoundId::WindowOpen },
        { "laugh_1", SoundId::Laugh1 },
        { "laugh_2", SoundId::Laugh2 },
        { "laugh_3", SoundId::Laugh3 },
        { "applause", SoundId::Applause },
        { "haunted_house_scare", SoundId::HauntedHouseScare },
        { "haunted_house_scream_1", SoundId::HauntedHouseScream1 },
        { "haunted_house_scream_2", SoundId::HauntedHouseScream2 },
        { "block_brake_close", SoundId::BlockBrakeClose },
        { "block_brake_release", SoundId::BlockBrakeRelease },
        { "error", SoundId::Error },
        { "brake_release", SoundId::BrakeRelease },
        { "lift_arrow", SoundId::LiftArrow },
        { "lift_wood", SoundId::LiftWood },
        { "track_friction_wood", SoundId::TrackFrictionWood },
        { "lift_wild_mouse", SoundId::LiftWildMouse },
        { "lift_bm", SoundId::LiftBM },
        { "track_friction_bm", SoundId::TrackFrictionBM },
        { "scream_8", SoundId::Scream8 },
        { "tram", SoundId::Tram },
        { "door_open", SoundId::DoorOpen },
        { "door_close", SoundId::DoorClose },
        { "portcullis", SoundId::Portcullis },
        { "crowd_ambience", SoundId::CrowdAmbience },
        { "lift_rmc", SoundId::LiftRMC },
        { "track_friction_rmc", SoundId::TrackFrictionRMC },
        { "lift_flume", SoundId::LiftFlume },
        { "no_scream", SoundId::NoScream },
        { "null", SoundId::Null }
    };

    inline SoundId GetSoundByName(const std::string& input)
    {
        auto result = SoundLookupTable.find(input);
        if (result != SoundLookupTable.end())
            return result->second;
        return SoundId::Null;
    }

} // namespace OpenRCT2::Scripting
