#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <memory>
#include "../core/Exception.hpp"
#include "../network/network.h"
#include "IniReader.h"

extern "C"
{
    #include "../config.h"
}

namespace Config
{
    static auto Enum_MeasurementFormat = ConfigEnum<sint8>(
    {
        ConfigEnumEntry<sint8>("IMPERIAL", MEASUREMENT_FORMAT_IMPERIAL),
        ConfigEnumEntry<sint8>("METRIC", MEASUREMENT_FORMAT_METRIC),
        ConfigEnumEntry<sint8>("SI", MEASUREMENT_FORMAT_SI),
    });

    static void ReadGeneral(IIniReader * reader)
    {
        if (reader->ReadSection("general"))
        {
            gConfigGeneral.always_show_gridlines = reader->GetBoolean("always_show_gridlines", false);
            gConfigGeneral.window_width = reader->GetSint32("window_width", -1);
            gConfigGeneral.window_height = reader->GetSint32("window_height", -1);
            gConfigGeneral.measurement_format = reader->GetEnum<sint8>("measurement_format", MEASUREMENT_FORMAT_METRIC, Enum_MeasurementFormat);
        }
    }

    static void ReadSound(IIniReader * reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigSound;
            model->master_volume = reader->GetSint32("master_volume", 100);
            model->title_music = reader->GetSint32("title_music", 2);
            model->sound_enabled = reader->GetBoolean("sound", true);
            model->sound_volume = reader->GetSint32("sound_volume", 100);
            model->ride_music_enabled = reader->GetBoolean("ride_music", true);
            model->ride_music_volume = reader->GetSint32("ride_music_volume", 100);
            model->audio_focus = reader->GetBoolean("audio_focus", false);
            model->device = reader->GetCString("audio_device", nullptr);
        }
    }

    static void ReadNetwork(IIniReader * reader)
    {
        if (reader->ReadSection("network"))
        {
            auto model = &gConfigNetwork;
            model->player_name = reader->GetCString("player_name", "Player");
            model->default_port = reader->GetSint32("default_port", NETWORK_DEFAULT_PORT);
            model->default_password = reader->GetCString("default_password", nullptr);
            model->stay_connected = reader->GetBoolean("stay_connected", true);
            model->advertise = reader->GetBoolean("advertise", true);
            model->maxplayers = reader->GetSint32("maxplayers", 16);
            model->server_name = reader->GetCString("server_name", "Server");
            model->server_description = reader->GetCString("server_description", nullptr);
            model->server_greeting = reader->GetCString("server_greeting", nullptr);
            model->master_server_url = reader->GetCString("master_server_url", nullptr);
            model->provider_name = reader->GetCString("provider_name", nullptr);
            model->provider_email = reader->GetCString("provider_email", nullptr);
            model->provider_website = reader->GetCString("provider_website", nullptr);
            model->known_keys_only = reader->GetBoolean("known_keys_only", false);
            model->log_chat = reader->GetBoolean("log_chat", false);
        }
    }

    static void ReadNotifications(IIniReader * reader)
    {
        if (reader->ReadSection("notifications"))
        {
            auto model = &gConfigNotifications;
            model->park_award = reader->GetBoolean("park_award", true);
            model->park_marketing_campaign_finished = reader->GetBoolean("park_marketing_campaign_finished", true);
            model->park_warnings = reader->GetBoolean("park_warnings", true);
            model->park_rating_warnings = reader->GetBoolean("park_rating_warnings", true);
            model->ride_broken_down = reader->GetBoolean("ride_broken_down", true);
            model->ride_crashed = reader->GetBoolean("ride_crashed", true);
            model->ride_warnings = reader->GetBoolean("ride_warnings", true);
            model->ride_researched = reader->GetBoolean("ride_researched", true);
            model->guest_warnings = reader->GetBoolean("guest_warnings", true);
            model->guest_lost = reader->GetBoolean("guest_lost", false);
            model->guest_left_park = reader->GetBoolean("guest_entered_left_park", true);
            model->guest_queuing_for_ride = reader->GetBoolean("guest_queuing_for_ride", true);
            model->guest_on_ride = reader->GetBoolean("guest_on_ride", true);
            model->guest_left_ride = reader->GetBoolean("guest_left_ride", true);
            model->guest_bought_item = reader->GetBoolean("guest_bought_item", true);
            model->guest_used_facility = reader->GetBoolean("guest_used_facility", true);
            model->guest_died = reader->GetBoolean("guest_died", true);
        }
    }

    static void ReadTwitch(IIniReader * reader)
    {
        if (reader->ReadSection("sound"))
        {
            auto model = &gConfigTwitch;
            model->channel = reader->GetCString("channel", nullptr);
            model->enable_follower_peep_names = reader->GetBoolean("follower_peep_names", true);
            model->enable_follower_peep_tracking = reader->GetBoolean("follower_peep_tracking", false);
            model->enable_chat_peep_names = reader->GetBoolean("chat_peep_names", true);
            model->enable_chat_peep_tracking = reader->GetBoolean("chat_peep_tracking", true);
            model->enable_news = reader->GetBoolean("news", false);
        }
    }

    static bool ReadFile(const std::string &path)
    {
        try
        {
            auto reader = std::unique_ptr<IIniReader>(CreateIniReader(path));
            ReadGeneral(reader.get());
            ReadSound(reader.get());
            ReadNetwork(reader.get());
            ReadNotifications(reader.get());
            ReadTwitch(reader.get());
            return true;
        }
        catch (const Exception &)
        {
            return false;
        }
    }
}

extern "C"
{
    bool config_open(const utf8 * path)
    {
        return Config::ReadFile(path);
    }
}
