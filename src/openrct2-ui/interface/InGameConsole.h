/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/core/String.hpp>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/localisation/FormatCodes.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Ui
{
    class InGameConsole final : public InteractiveConsole
    {
    private:
        static constexpr int32_t CONSOLE_MAX_LINES = 300;
        static constexpr int32_t CONSOLE_HISTORY_SIZE = 64;
        static constexpr int32_t CONSOLE_INPUT_SIZE = 256;
        static constexpr int32_t CONSOLE_CARET_FLASH_THRESHOLD = 15;
        static constexpr int32_t CONSOLE_EDGE_PADDING = 4;
        static constexpr int32_t CONSOLE_CARET_WIDTH = 6;

        bool _isOpen = false;
        ScreenCoordsXY _consoleTopLeft;
        ScreenCoordsXY _consoleBottomRight;
        ScreenCoordsXY _lastMainViewport;
        std::deque<std::string> _consoleLines;
        utf8 _consoleCurrentLine[CONSOLE_INPUT_SIZE] = {};
        int32_t _consoleCaretTicks;
        int32_t _consoleScrollPos = 0;
        TextInputSession* _consoleTextInputSession;
        utf8 _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
        int32_t _consoleHistoryIndex = 0;
        int32_t _consoleHistoryCount = 0;
        size_t _selectionStart = 0;
        int32_t _caretScreenPosX = 0;

    public:
        InGameConsole();
        InGameConsole(const InGameConsole& src) = delete;

        bool IsOpen() const
        {
            return _isOpen;
        }

        void Clear() override;
        void Open();
        void Close() override;
        void Hide() override;
        void Toggle();
        void WriteLine(const std::string& s, FormatToken colourFormat) override;

        void Input(ConsoleInput input);
        void RefreshCaret(size_t position = 0);
        void Scroll(int32_t linesToScroll);

        void Update();
        void Draw(rct_drawpixelinfo* dpi) const;

    private:
        void ClearInput();
        void ClearLine();
        void HistoryAdd(const utf8* src);
        void WritePrompt();
        void ScrollToEnd();
        void Invalidate() const;
        int32_t GetNumVisibleLines() const;
    };
} // namespace OpenRCT2::Ui
