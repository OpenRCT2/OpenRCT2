/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <SDL.h>
#include <functional>
#include <map>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/interface/Cursors.h>

struct SDL_Cursor;

namespace OpenRCT2::Ui
{
    struct CursorData;

    class CursorRepository
    {
    private:
        class CursorSetHolder
        {
        private:
            SDL_Cursor* _cursors[EnumValue(CursorID::Count)] = { nullptr };

        public:
            CursorSetHolder(const std::function<SDL_Cursor*(CursorID)>& getCursor)
            {
                for (size_t i = 0; i < EnumValue(CursorID::Count); i++)
                {
                    _cursors[i] = getCursor(static_cast<CursorID>(i));
                }
            }

            ~CursorSetHolder()
            {
                for (size_t i = 0; i < EnumValue(CursorID::Count); i++)
                {
                    SDL_FreeCursor(_cursors[i]);
                }
            }

            SDL_Cursor* GetScaledCursor(CursorID cursorId)
            {
                return _cursors[EnumValue(cursorId)];
            }
        };

        constexpr static int32_t kBaseCursorWidth = 32;
        constexpr static int32_t kBaseCursorHeight = 32;

        CursorID _currentCursor = CursorID::Undefined;
        uint8_t _currentCursorScale = 1;

        std::map<uint8_t, CursorSetHolder> _scaledCursors;

    public:
        ~CursorRepository();
        void LoadCursors();
        CursorID GetCurrentCursor();
        void SetCurrentCursor(CursorID cursorId);
        void SetCursorScale(uint8_t cursorScale);

    private:
        SDL_Cursor* Create(const CursorData* cursorInfo, uint8_t scale);
        void GenerateScaledCursorSetHolder(uint8_t scale);
    };
} // namespace OpenRCT2::Ui
