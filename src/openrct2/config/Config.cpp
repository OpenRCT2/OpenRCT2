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

    static bool ReadFile(const std::string &path)
    {
        try
        {
            auto reader = std::unique_ptr<IIniReader>(CreateIniReader(path));
            ReadGeneral(reader.get());
            ReadSound(reader.get());
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
