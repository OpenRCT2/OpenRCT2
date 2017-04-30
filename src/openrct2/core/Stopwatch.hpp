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

extern "C"
{
    #include "../common.h"
}

/**
 * Class to accurately measure elapsed time with high precision.
 */
class Stopwatch final
{
private:
    /** Number of ticks in a second. */
    static uint64 Frequency;

    uint64 _total;
    uint64 _last;
    bool   _isRunning;

    static uint64 QueryFrequency();
    static uint64 QueryCurrentTicks();

public:
    bool IsRunning() const { return _isRunning; }

    Stopwatch();

    uint64 GetElapsedTicks()        const;
    uint64 GetElapsedMilliseconds() const;

    void Reset();
    void Start();
    void Restart();
    void Stop();
};
