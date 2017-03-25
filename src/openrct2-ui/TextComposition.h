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
#include <openrct2/ui/UiContext.h>

union SDL_Event;

namespace OpenRCT2
{
    namespace Ui
    {
        /**
        * Represents a
        */
        class TextComposition
        {
        private:
            TextInputSession _session = { 0 };

            bool    _imeActive = false;
            sint32  _imeStart = 0;
            sint32  _imeLength = 0;
            utf8    _imeBuffer[32] = { 0 };

        public:
            bool IsActive();
            const TextInputSession * TextComposition::Start(utf8 * buffer, size_t bufferSize);
            void Stop();
            void HandleMessage(const SDL_Event * e);

        private:
            void CursorHome();
            void CursorEnd();
            void CursorLeft();
            void CursorRight();
            void Insert(const utf8 * text);
            void InsertCodepoint(codepoint_t codepoint);
            void Clear();
            void Delete();
            void RecalculateLength();
        };
    }
}
