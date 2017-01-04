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

#include <SDL_mouse.h>
#include "../core/Guard.hpp"
#include "Cursors.h"

namespace Cursors
{
    constexpr sint32 CURSOR_WIDTH = 32;
    constexpr sint32 CURSOR_HEIGHT = 32;

    static SDL_Cursor * _loadedCursors[CURSOR_COUNT];
    static bool         _initialised = false;
    static CURSOR_ID    _currentCursor = CURSOR_UNDEFINED;

    static SDL_Cursor * Create(const CursorData * cursorInfo)
    {
        SDL_Cursor * cursor = SDL_CreateCursor(
            cursorInfo->Data,
            cursorInfo->Mask,
            CURSOR_WIDTH,
            CURSOR_HEIGHT,
            cursorInfo->HotSpot.X,
            cursorInfo->HotSpot.Y);
        return cursor;
    }

    void Initialise()
    {
        Guard::Assert(!_initialised, "Cursors have already been initialised.");
        _initialised = true;

        // Using system cursors
        _loadedCursors[CURSOR_ARROW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        _loadedCursors[CURSOR_HAND_POINT] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

        // Using custom cursors
        for (size_t i = 0; i < CURSOR_COUNT; i++)
        {
            const CursorData * cursorData = GetCursorData((CURSOR_ID)i);
            if (cursorData != nullptr)
            {
                _loadedCursors[i] = Create(cursorData);
            }
        }

        _currentCursor = CURSOR_UNDEFINED;
        SetCurrentCursor(CURSOR_ARROW);
    }

    void Dispose()
    {
        if (_initialised)
        {
            for (size_t i = 0; i < CURSOR_COUNT; i++)
            {
                SDL_FreeCursor(_loadedCursors[i]);
                _loadedCursors[i] = nullptr;
            }
            _currentCursor = CURSOR_UNDEFINED;
            _initialised = false;
        }
    }

    CURSOR_ID GetCurrentCursor()
    {
        return _currentCursor;
    }

    void SetCurrentCursor(CURSOR_ID cursorId)
    {
        if (_currentCursor != cursorId)
        {
            SDL_Cursor * cursor = _loadedCursors[cursorId];
            SDL_SetCursor(cursor);
            _currentCursor = cursorId;
        }
    }
}

extern "C"
{
    void cursors_initialise()
    {
        Cursors::Initialise();
    }

    void cursors_dispose()
    {
        Cursors::Dispose();
    }

    int cursors_getcurrentcursor()
    {
        return Cursors::GetCurrentCursor();
    }

    void cursors_setcurrentcursor(int cursorId)
    {
        Cursors::SetCurrentCursor((CURSOR_ID)cursorId);
    }
}
