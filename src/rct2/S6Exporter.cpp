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

#include "../core/Exception.hpp"
#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "S6Exporter.h"

extern "C"
{
    #include "../config.h"
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../localisation/date.h"
    #include "../localisation/localisation.h"
    #include "../management/finance.h"
    #include "../management/marketing.h"
    #include "../management/news_item.h"
    #include "../management/research.h"
    #include "../object.h"
    #include "../openrct2.h"
    #include "../peep/staff.h"
    #include "../ride/ride.h"
    #include "../ride/ride_ratings.h"
    #include "../scenario.h"
    #include "../util/sawyercoding.h"
    #include "../world/climate.h"
    #include "../world/map_animation.h"
    #include "../world/park.h"
}

S6Exporter::S6Exporter()
{
    memset(&_s6, 0, sizeof(_s6));
}

void S6Exporter::SaveGame(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to write to destination file.");
    }

    SaveGame(rw);

    SDL_RWclose(rw);
}

void S6Exporter::SaveGame(SDL_RWops *rw)
{
    Save(rw, false);
}

void S6Exporter::SaveScenario(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr)
    {
        throw IOException("Unable to write to destination file.");
    }

    SaveGame(rw);

    SDL_RWclose(rw);
}

void S6Exporter::SaveScenario(SDL_RWops *rw)
{
    Save(rw, true);
}

void S6Exporter::Save(SDL_RWops * rw, bool isScenario)
{
    _s6.header.type = isScenario ? S6_TYPE_SAVEDGAME : S6_TYPE_SCENARIO;
    _s6.header.num_packed_objects = scenario_get_num_packed_objects_to_write();
    _s6.header.version = S6_RCT2_VERSION;
    _s6.header.magic_number = S6_MAGIC_NUMBER;

    uint8 * buffer = (uint8 *)malloc(0x600000);
    if (buffer == NULL)
    {
        log_error("Unable to allocate enough space for a write buffer.");
        throw Exception("Unable to allocate memory.");
    }
    
    sawyercoding_chunk_header chunkHeader;
    int encodedLength;

    // 0: Write header chunk
    chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
    chunkHeader.length = sizeof(rct_s6_header);
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.header, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 1: Write scenario info chunk
    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
        chunkHeader.length = sizeof(rct_s6_info);
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.info, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }

    // 2: Write packed objects
    if (_s6.header.num_packed_objects > 0)
    {
        if (!scenario_write_packed_objects(rw))
        {
            free(buffer);
            throw Exception("Unable to pack objects.");
        }
    }

    // 3: Write available objects chunk
    chunkHeader.encoding = CHUNK_ENCODING_ROTATE;
    chunkHeader.length = 721 * sizeof(rct_object_entry);
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)_s6.objects, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 4: Misc fields (data, rand...) chunk
    chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
    chunkHeader.length = 16;
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.elapsed_months, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    // 5: Map elements + sprites and other fields chunk
    chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
    chunkHeader.length = 0x180000;
    encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)_s6.map_elements, chunkHeader);
    SDL_RWwrite(rw, buffer, encodedLength, 1);

    if (_s6.header.type == S6_TYPE_SCENARIO)
    {
        // 6:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x27104C;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.dword_010E63B8, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 7:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 4;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.guests_in_park, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 8:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 8;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.last_guests_in_park, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 9:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 2;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.park_rating, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 10:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 1082;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.active_research_types, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 11:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 16;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.current_expenditure, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 12:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 4;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.park_value, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);

        // 13:
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x761E8;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.completed_company_value, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }
    else
    {
        // 6: Everything else...
        chunkHeader.encoding = CHUNK_ENCODING_RLECOMPRESSED;
        chunkHeader.length = 0x2E8570;
        encodedLength = sawyercoding_write_chunk_buffer(buffer, (uint8*)&_s6.dword_010E63B8, chunkHeader);
        SDL_RWwrite(rw, buffer, encodedLength, 1);
    }

    free(buffer);
    
    // Determine number of bytes written
    size_t fileSize = (size_t)SDL_RWtell(rw);
    SDL_RWseek(rw, 0, RW_SEEK_SET);

    // Read all written bytes back into a single buffer
    buffer = (uint8 *)malloc(fileSize);
    SDL_RWread(rw, buffer, fileSize, 1);
    uint32 checksum = sawyercoding_calculate_checksum(buffer, fileSize);
    free(buffer);

    // Append the checksum
    SDL_RWseek(rw, fileSize, RW_SEEK_SET);
    SDL_RWwrite(rw, &checksum, sizeof(uint32), 1);
}

void S6Exporter::Export()
{
    _s6.info = *(RCT2_ADDRESS(0x0141F570, rct_s6_info));

    for (int i = 0; i < 721; i++)
    {
        rct_object_entry_extended *entry = &(RCT2_ADDRESS(0x00F3F03C, rct_object_entry_extended)[i]);

        if (gObjectList[i] == (void *)0xFFFFFFFF)
        {
            memset(&_s6.objects[i], 0xFF, sizeof(rct_object_entry));
        }
        else
        {
            _s6.objects[i] = *((rct_object_entry*)entry);
        }
    }

    _s6.elapsed_months = gDateMonthsElapsed;
    _s6.current_day = gDateMonthTicks;
    _s6.scenario_ticks = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32);
    _s6.scenario_srand_0 = gScenarioSrand0;
    _s6.scenario_srand_1 = gScenarioSrand1;

    memcpy(&_s6.elapsed_months, (void*)0x00F663A8, 16);
    memcpy(_s6.map_elements, (void*)0x00F663B8, 0x180000);
    memcpy(&_s6.dword_010E63B8, (void*)0x010E63B8, 0x2E8570);

    String::Set(_s6.scenario_filename, sizeof(_s6.scenario_filename), _scenarioFileName);

    scenario_fix_ghosts(&_s6);
    game_convert_strings_to_rct2(&_s6);
}

extern "C"
{
    // Save game state without modifying any of the state for multiplayer
    int scenario_save_network(SDL_RWops * rw)
    {
        // Set saved view
        sint16 viewX, viewY;
        uint8 viewZoom, viewRotation;
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            rct_viewport *viewport = w->viewport;

            viewX = viewport->view_width / 2 + viewport->view_x;
            viewY = viewport->view_height / 2 + viewport->view_y;
            viewZoom = viewport->zoom;
            viewRotation = get_current_rotation();
        }
        else
        {
            viewX = 0;
            viewY = 0;
            viewZoom = 0;
            viewRotation = 0;
        }

        gSavedViewX = viewX;
        gSavedViewY = viewY;
        gSavedViewZoom = viewZoom;
        gSavedViewRotation = viewRotation;
        
        bool result = false;
        auto s6exporter = new S6Exporter();
        try
        {
            s6exporter->Export();
            s6exporter->SaveGame(rw);
            result = true;
        }
        catch (Exception)
        {
        }
        delete s6exporter;

        if (!result)
        {
            return 0;
        }

        reset_loaded_objects();

        // Write other data not in normal save files
        SDL_WriteLE32(rw, gGamePaused);
        SDL_WriteLE32(rw, _guestGenerationProbability);
        SDL_WriteLE32(rw, _suggestedGuestMaximum);
        SDL_WriteU8(rw, gCheatsSandboxMode);
        SDL_WriteU8(rw, gCheatsDisableClearanceChecks);
        SDL_WriteU8(rw, gCheatsDisableSupportLimits);
        SDL_WriteU8(rw, gCheatsDisableTrainLengthLimit);
        SDL_WriteU8(rw, gCheatsShowAllOperatingModes);
        SDL_WriteU8(rw, gCheatsShowVehiclesFromOtherTrackTypes);
        SDL_WriteU8(rw, gCheatsFastLiftHill);
        SDL_WriteU8(rw, gCheatsDisableBrakesFailure);
        SDL_WriteU8(rw, gCheatsDisableAllBreakdowns);
        SDL_WriteU8(rw, gCheatsUnlockAllPrices);
        SDL_WriteU8(rw, gCheatsBuildInPauseMode);
        SDL_WriteU8(rw, gCheatsIgnoreRideIntensity);
        SDL_WriteU8(rw, gCheatsDisableVandalism);
        SDL_WriteU8(rw, gCheatsDisableLittering);
        SDL_WriteU8(rw, gCheatsNeverendingMarketing);
        SDL_WriteU8(rw, gCheatsFreezeClimate);

        gfx_invalidate_screen();
        return 1;
    }
}
