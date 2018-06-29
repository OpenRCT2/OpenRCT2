/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <map>
#include <functional>

#include <SDL2/SDL.h>

#include <openrct2/interface/Cursors.h>

struct SDL_Cursor;

namespace OpenRCT2::Ui
{
    class CursorRepository
    {
    private:
        class CursorSetHolder
        {
        private:
            SDL_Cursor * _cursors[CURSOR_COUNT] = {nullptr};
        public:
            CursorSetHolder(const std::function<SDL_Cursor *(CURSOR_ID)> & getCursor)
            {
                for (size_t i = 0; i < CURSOR_COUNT; i++)
                {
                    _cursors[i] = getCursor(static_cast<CURSOR_ID>(i));
                }
            }

            ~CursorSetHolder()
            {
                for (size_t i = 0; i < CURSOR_COUNT; i++)
                {
                    SDL_FreeCursor(_cursors[i]);
                }
            }

            SDL_Cursor * getScaledCursor(CURSOR_ID cursorId)
            {
                return _cursors[cursorId];
            }
        };

        constexpr static int32_t BASE_CURSOR_WIDTH  = 32;
        constexpr static int32_t BASE_CURSOR_HEIGHT = 32;

        CURSOR_ID _currentCursor  = CURSOR_UNDEFINED;
        uint8_t _currentCursorScale = 1;

        std::map<uint8_t, CursorSetHolder> _scaledCursors;

    public:
        ~CursorRepository();
        void LoadCursors();
        CURSOR_ID GetCurrentCursor();
        void SetCurrentCursor(CURSOR_ID cursorId);
        void SetCursorScale(uint8_t cursorScale);

    private:
        SDL_Cursor * Create(const CursorData * cursorInfo, uint8_t scale);
        void GenerateScaledCursorSetHolder(uint8_t scale);
        static const CursorData * GetCursorData(CURSOR_ID cursorId);
    };
}
