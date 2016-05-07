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
#include "S6Importer.h"

extern "C"
{
    #include "../util/sawyercoding.h"
}

S6Importer::S6Importer()
{
    memset(&_s6, 0, sizeof(_s6));
}

void S6Importer::LoadSavedGame(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr) {
        throw IOException("Unable to open SV6.");
    }

    LoadSavedGame(rw);

    SDL_RWclose(rw);

    _s6Path = path;
}

void S6Importer::LoadScenario(const utf8 * path)
{
    SDL_RWops * rw = SDL_RWFromFile(path, "rb");
    if (rw == nullptr) {
        throw IOException("Unable to open SV6.");
    }

    LoadScenario(rw);

    SDL_RWclose(rw);

    _s6Path = path;
}

void S6Importer::LoadSavedGame(SDL_RWops *rw)
{
    sawyercoding_read_chunk(rw, (uint8*)&_s6.header);

    // Read packed objects
    // TODO try to contain this more and not store objects until later
    if (_s6.header.num_packed_objects > 0) {
        int j = 0;
        for (uint16 i = 0; i < _s6.header.num_packed_objects; i++)
        {
            j += object_load_packed(rw);
        }
        if (j > 0)
        {
            object_list_load();
        }
    }

    sawyercoding_read_chunk(rw, (uint8*)&_s6.objects);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.elapsed_months);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.map_elements);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.dword_010E63B8);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.guests_in_park);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.expenditure_table);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.last_guests_in_park);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.dword_01357BD0);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_rating);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_rating_history);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.active_research_types);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.balance_history);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.current_expenditure);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.weekly_profit_history);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_value);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_value_history);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.completed_company_value);
}

void S6Importer::LoadScenario(SDL_RWops *rw)
{
    sawyercoding_read_chunk(rw, (uint8*)&_s6.header);

    // Read packed objects
    // TODO try to contain this more and not store objects until later
    if (_s6.header.num_packed_objects > 0) {
        int j = 0;
        for (uint16 i = 0; i < _s6.header.num_packed_objects; i++)
        {
            j += object_load_packed(rw);
        }
        if (j > 0)
        {
            object_list_load();
        }
    }

    sawyercoding_read_chunk(rw, (uint8*)&_s6.objects);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.elapsed_months);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.map_elements);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.dword_010E63B8);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.guests_in_park);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.last_guests_in_park);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_rating);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.active_research_types);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.current_expenditure);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.park_value);
    sawyercoding_read_chunk(rw, (uint8*)&_s6.completed_company_value);
}

void S6Importer::Import()
{

}
