/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <deque>
#include <openrct2/core/StringTypes.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/localisation/FormatCodes.h>
#include <openrct2/world/Location.hpp>
#include <vector>

namespace OpenRCT2::Ui
{
    class InGameConsole final : public InteractiveConsole
    {
    private:
        static constexpr int32_t kConsoleMaxLines = 300;
        static constexpr int32_t kConsoleHistorySize = 64;
        static constexpr int32_t kConsoleInputSize = 256;
        static constexpr int32_t kConsoleCaretFlashThreshold = 15;
        static constexpr int32_t kConsoleEdgePadding = 4;
        static constexpr int32_t kConsoleCaretWidth = 6;

        bool _isInitialised = false;
        bool _isOpen = false;
        bool _isCommandAwaitingCompletion = false;
        ScreenCoordsXY _consoleTopLeft;
        ScreenCoordsXY _consoleBottomRight;
        ScreenCoordsXY _lastMainViewport;

        std::vector<std::pair<std::string, FormatToken>> _consoleLines;
        u8string _consoleCurrentLine;

        int32_t _consoleCaretTicks;
        int32_t _consoleScrollPos = 0;
        TextInputSession* _consoleTextInputSession;

        std::deque<u8string> _consoleHistory;
        size_t _consoleHistoryIndex = 0;

        size_t _selectionStart = 0;
        int32_t _caretScreenPosX = 0;

    public:
        InGameConsole() = default;
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
        void Draw(DrawPixelInfo& dpi) const;

    private:
        void ClearInput();
        void ClearLine();
        void HistoryAdd(const u8string& src);
        void WriteInitial();
        void WritePrompt();
        void ScrollToEnd();
        void Invalidate() const;
        int32_t GetNumVisibleLines() const;
    };
} // namespace OpenRCT2::Ui
