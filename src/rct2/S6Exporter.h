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

#pragma once

#include <string>
#include <vector>

#include "../common.h"

extern "C"
{
    #include "../scenario.h"
    #include "../object_list.h"
}

int scenario_save_network(SDL_RWops* rw, const std::vector<std::string> &objects);
int scenario_write_packed_objects(SDL_RWops* rw, std::vector<std::string> &s, bool export_all);

/**
 * Class to export RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Exporter final
{
public:
    bool ExportObjects;
    bool RemoveTracklessRides;
    std::vector<std::string> ExportObjectsList;

    S6Exporter();

    void SaveGame(const utf8 * path);
    void SaveGame(SDL_RWops *rw);
    void SaveScenario(const utf8 * path);
    void SaveScenario(SDL_RWops *rw);
    void Export();
    void Export(const std::vector<std::string> &objects, bool export_all = false);

private:
    rct_s6_data _s6;

    void Save(SDL_RWops *rw, bool isScenario);
    static uint32 GetLoanHash(money32 initialCash, money32 bankLoan, uint32 maxBankLoan);
};
