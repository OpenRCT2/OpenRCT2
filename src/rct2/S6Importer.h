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

#include "../common.h"

extern "C"
{
    #include "../scenario/scenario.h"
}

/**
 * Class to import RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Importer final
{
public:
    bool FixIssues;

    S6Importer();

    void LoadSavedGame(const utf8 * path);
    void LoadSavedGame(SDL_RWops *rw);
    void LoadScenario(const utf8 * path);
    void LoadScenario(SDL_RWops *rw);
    void Import();

private:
    const utf8 * _s6Path = nullptr;
    rct_s6_data  _s6;
    uint8        _gameVersion = 0;
};
