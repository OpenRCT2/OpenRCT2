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

#include <SDL_timer.h>

#include "Stopwatch.hpp"

uint64 Stopwatch::Frequency = 0;

Stopwatch::Stopwatch()
{
    Reset();
}

uint64 Stopwatch::GetElapsedTicks() const
{
    uint64 result = _total;
    if (_isRunning)
    {
        uint64 ticks = QueryCurrentTicks();
        if (ticks != 0) {
            result += QueryCurrentTicks() - _last;
        }
    }

    return result;
}

uint64 Stopwatch::GetElapsedMilliseconds() const
{
    if (Frequency == 0)
    {
        Frequency = QueryFrequency();
        if (Frequency == 0)
        {
            return 0;
        }
    }

    return (GetElapsedTicks() * 1000) / Frequency;
}

void Stopwatch::Reset()
{
    _isRunning = false;
    _total = 0;
    _last = 0;
}

void Stopwatch::Start()
{
    if (_isRunning) return;

    uint64 ticks = QueryCurrentTicks();
    if (ticks != 0)
    {
        _last = ticks;
        _isRunning = true;
    }
}

void Stopwatch::Restart()
{
    Reset();
    Start();
}

void Stopwatch::Stop()
{
    uint64 ticks = QueryCurrentTicks();
    if (ticks != 0)
    {
        _total += QueryCurrentTicks() - _last;
    }
    _isRunning = false;
}

uint64 Stopwatch::QueryFrequency()
{
    return SDL_GetPerformanceFrequency();
}

uint64 Stopwatch::QueryCurrentTicks()
{
    return SDL_GetPerformanceCounter();
}

extern "C"
{
    #include "stopwatch.h"

    void stopwatch_create(stopwatch * stopwatch)
    {
        stopwatch->context = new Stopwatch();
    }

    void stopwatch_dispose(stopwatch * stopwatch)
    {
        delete ((Stopwatch*)stopwatch->context);
    }

    uint64 stopwatch_GetElapsedTicks(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->GetElapsedTicks();
    }

    uint64 stopwatch_GetElapsedMilliseconds(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->GetElapsedMilliseconds();
    }

    void stopwatch_Reset(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->Reset();
    }

    void stopwatch_Start(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->Start();
    }

    void stopwatch_Restart(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->Restart();
    }

    void stopwatch_Stop(stopwatch * stopwatch)
    {
        Stopwatch * ctx = (Stopwatch*)stopwatch->context;
        return ctx->Stop();
    }
}
