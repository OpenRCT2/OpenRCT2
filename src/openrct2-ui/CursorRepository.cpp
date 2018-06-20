/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>

#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/interface/Cursors.h>

#include "CursorRepository.h"

using namespace OpenRCT2::Ui;

CursorRepository::~CursorRepository()
{
    _scaledCursors.clear();
    _currentCursor = CURSOR_UNDEFINED;
    _currentCursorScale = 1;
}

void CursorRepository::LoadCursors()
{
    SetCursorScale(static_cast<uint8_t>(round(gConfigGeneral.window_scale)));
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
        SDL_Cursor * cursor = _scaledCursors.at(_currentCursorScale).getScaledCursor(cursorId);
        SDL_SetCursor(cursor);
        _currentCursor = cursorId;
    }
}

static bool getBit(const uint8_t * data, size_t x, size_t y, size_t width)
{
    size_t position = y * width + x;
    return (data[position / 8] & (1 << (7 - (x % 8)))) != 0;
}

static void setBit(uint8_t * data, size_t x, size_t y, size_t width)
{
    size_t position = y * width + x;
    data[position / 8] |= (1 << (7 - (position % 8)));
}

static void drawRect(uint8_t * data, size_t x, size_t y, size_t width, size_t scale)
{
    for (size_t outY = (y * scale); outY < ((1 + y) * scale); outY++)
    {
        for (size_t outX = (x * scale); outX < ((1 + x) * scale); outX++)
        {
            setBit(data, outX, outY, width * scale);
        }
    }
}

static uint8_t * scaleDataArray(const uint8_t data[], size_t width, size_t height, size_t scale)
{
    size_t length = width * height;
    auto * ret = static_cast<uint8_t *>(calloc(sizeof(uint8_t), length * scale * scale));

    for (size_t y = 0; y < height * 8; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            bool value = getBit(data, x, y, width);
            if (!value) continue;

            drawRect(ret, x, y, width, scale);
        }
    }

    return ret;
}

SDL_Cursor * CursorRepository::Create(const CursorData * cursorInfo, uint8_t scale)
{
    SDL_Cursor * cursor;

    auto integer_scale = (int) round(scale);

    auto data = scaleDataArray(cursorInfo->Data, CURSOR_BIT_WIDTH, CURSOR_HEIGHT, static_cast<size_t>(integer_scale));
    auto mask = scaleDataArray(cursorInfo->Mask, CURSOR_BIT_WIDTH, CURSOR_HEIGHT, static_cast<size_t>(integer_scale));

    cursor = SDL_CreateCursor(
            data,
            mask,
            BASE_CURSOR_WIDTH * integer_scale,
            BASE_CURSOR_HEIGHT * integer_scale,
            cursorInfo->HotSpot.X * integer_scale,
            cursorInfo->HotSpot.Y * integer_scale);

    free(data);
    free(mask);


    return cursor;
}

void CursorRepository::SetCursorScale(uint8_t cursorScale)
{
    if (cursorScale > 0.0)
    {
        _currentCursorScale = cursorScale;
        GenerateScaledCursorSetHolder(_currentCursorScale);
    }
}

void CursorRepository::GenerateScaledCursorSetHolder(uint8_t scale)
{
    if (_scaledCursors.find(scale) == _scaledCursors.end())
    {
        std::function<SDL_Cursor *(CURSOR_ID)> cursorGenerator = [this, scale](CURSOR_ID cursorId)
        {
            switch (cursorId)
            {
                // We can't scale the system cursors, but they should be appropriately scaled anyway
                case CURSOR_ARROW:
                    return SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                case CURSOR_HAND_POINT:
                    return SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
                default:
                    return this->Create(GetCursorData(cursorId), scale);
            }
        };
        _scaledCursors.emplace(scale, cursorGenerator);
    }
}
