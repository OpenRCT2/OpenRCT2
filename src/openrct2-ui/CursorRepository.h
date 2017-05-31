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

#include <openrct2/interface/Cursors.h>

struct SDL_Cursor;

namespace OpenRCT2
{
    namespace Ui
    {
        class CursorRepository
        {
        private:
            constexpr static sint32 CURSOR_WIDTH = 32;
            constexpr static sint32 CURSOR_HEIGHT = 32;

            SDL_Cursor *    _loadedCursors[CURSOR_COUNT] = { nullptr };
            CURSOR_ID       _currentCursor = CURSOR_UNDEFINED;

        public:
            ~CursorRepository();
            void LoadCursors();
            CURSOR_ID GetCurrentCursor();
            void SetCurrentCursor(CURSOR_ID cursorId);

        private:
            SDL_Cursor * Create(const CursorData * cursorInfo);
            static const CursorData * GetCursorData(CURSOR_ID cursorId);
        };
    }
}
