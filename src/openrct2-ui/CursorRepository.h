#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <map>
#include <functional>

#include <SDL2/SDL.h>

#include <openrct2/interface/Cursors.h>

struct SDL_Cursor;

namespace OpenRCT2
{
    namespace Ui
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

            constexpr static sint32 BASE_CURSOR_WIDTH  = 32;
            constexpr static sint32 BASE_CURSOR_HEIGHT = 32;

            CURSOR_ID _currentCursor  = CURSOR_UNDEFINED;
            uint8 _currentCursorScale = 1;

            std::map<uint8, CursorSetHolder> _scaledCursors;

        public:
            ~CursorRepository();
            void LoadCursors();
            CURSOR_ID GetCurrentCursor();
            void SetCurrentCursor(CURSOR_ID cursorId);
            void SetCursorScale(uint8 cursorScale);

        private:
            SDL_Cursor * Create(const CursorData * cursorInfo, uint8 scale);
            void GenerateScaledCursorSetHolder(uint8 scale);
            static const CursorData * GetCursorData(CURSOR_ID cursorId);
        };
    }
}
