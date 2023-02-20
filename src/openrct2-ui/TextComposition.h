/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/Context.h>
#include <openrct2/common.h>
#include <openrct2/ui/UiContext.h>

union SDL_Event;

namespace OpenRCT2::Ui
{
    /**
     * Represents a
     */
    class TextComposition
    {
    private:
        TextInputSession _session = {};

        bool _imeActive = false;
        int32_t _imeStart = 0;
        int32_t _imeLength = 0;
        utf8 _imeBuffer[32] = {};

    public:
        bool IsActive();
        TextInputSession* Start(u8string& buffer, size_t maxLength);
        void Stop();
        void HandleMessage(const SDL_Event* e);

    private:
        void CursorHome();
        void CursorEnd();
        void CursorLeft();
        void CursorRight();
        void Insert(const utf8* text);
        void InsertCodepoint(codepoint_t codepoint);
        void Clear();
        void Delete();
        void RecalculateLength();
    };
} // namespace OpenRCT2::Ui
