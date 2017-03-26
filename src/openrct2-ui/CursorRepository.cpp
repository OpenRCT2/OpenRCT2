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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/interface/Cursors.h>
#include "CursorRepository.h"

using namespace OpenRCT2::Ui;

CursorRepository::~CursorRepository()
{
    for (size_t i = 0; i < CURSOR_COUNT; i++)
    {
        SDL_FreeCursor(_loadedCursors[i]);
        _loadedCursors[i] = nullptr;
    }
    _currentCursor = CURSOR_UNDEFINED;
}

void CursorRepository::LoadCursors()
{
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

CURSOR_ID CursorRepository::GetCurrentCursor()
{
    return _currentCursor;
}

void CursorRepository::SetCurrentCursor(CURSOR_ID cursorId)
{
    if (_currentCursor != cursorId)
    {
        SDL_Cursor * cursor = _loadedCursors[cursorId];
        SDL_SetCursor(cursor);
        _currentCursor = cursorId;
    }
}

SDL_Cursor * CursorRepository::Create(const CursorData * cursorInfo)
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
