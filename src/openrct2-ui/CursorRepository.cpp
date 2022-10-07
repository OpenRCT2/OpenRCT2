/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CursorRepository.h"

#include <cmath>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/interface/Cursors.h>
#include <vector>

using namespace OpenRCT2::Ui;

CursorRepository::~CursorRepository()
{
    _scaledCursors.clear();
    _currentCursor = CursorID::Undefined;
    _currentCursorScale = 1;
}

void CursorRepository::LoadCursors()
{
    SetCursorScale(static_cast<uint8_t>(round(gConfigGeneral.window_scale)));
    SetCurrentCursor(CursorID::Arrow);
}

CursorID CursorRepository::GetCurrentCursor()
{
    return _currentCursor;
}

void CursorRepository::SetCurrentCursor(CursorID cursorId)
{
    if (_currentCursor != cursorId)
    {
        SDL_Cursor* cursor = _scaledCursors.at(_currentCursorScale).GetScaledCursor(cursorId);
        SDL_SetCursor(cursor);
        _currentCursor = cursorId;
    }
}

static bool GetBit(const uint8_t data[], size_t x, size_t y, size_t width) noexcept
{
    const size_t position = y * width + x;
    return (data[position / 8] & (1 << (7 - (x % 8)))) != 0;
}

static void SetBit(uint8_t data[], size_t x, size_t y, size_t width) noexcept
{
    size_t position = y * width + x;
    data[position / 8] |= (1 << (7 - (position % 8)));
}

static void DrawRect(uint8_t data[], size_t x, size_t y, size_t width, size_t scale) noexcept
{
    for (size_t outY = (y * scale); outY < ((1 + y) * scale); outY++)
    {
        for (size_t outX = (x * scale); outX < ((1 + x) * scale); outX++)
        {
            SetBit(data, outX, outY, width * scale);
        }
    }
}

static std::vector<uint8_t> ScaleDataArray(const uint8_t data[], size_t width, size_t height, size_t scale)
{
    const size_t length = width * height;

    std::vector<uint8_t> res;
    res.resize(length * scale * scale);

    for (size_t y = 0; y < height * 8; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            const bool value = GetBit(data, x, y, width);
            if (!value)
                continue;

            DrawRect(res.data(), x, y, width, scale);
        }
    }

    return res;
}

SDL_Cursor* CursorRepository::Create(const CursorData* cursorInfo, uint8_t scale)
{
    const auto integer_scale = static_cast<int>(round(scale));

    auto data = ScaleDataArray(cursorInfo->Data, CURSOR_BIT_WIDTH, CURSOR_HEIGHT, static_cast<size_t>(integer_scale));
    auto mask = ScaleDataArray(cursorInfo->Mask, CURSOR_BIT_WIDTH, CURSOR_HEIGHT, static_cast<size_t>(integer_scale));

    auto* cursor = SDL_CreateCursor(
        data.data(), mask.data(), BASE_CURSOR_WIDTH * integer_scale, BASE_CURSOR_HEIGHT * integer_scale,
        cursorInfo->HotSpot.X * integer_scale, cursorInfo->HotSpot.Y * integer_scale);

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
        std::function<SDL_Cursor*(CursorID)> cursorGenerator = [this, scale](CursorID cursorId) {
            switch (cursorId)
            {
                // We can't scale the system cursors, but they should be appropriately scaled anyway
                case CursorID::Arrow:
                    return SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                case CursorID::HandPoint:
                    return SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
                default:
                    return this->Create(GetCursorData(cursorId), scale);
            }
        };
        _scaledCursors.emplace(scale, cursorGenerator);
    }
}
