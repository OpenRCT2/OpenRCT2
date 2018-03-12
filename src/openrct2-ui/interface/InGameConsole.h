#include <openrct2/interface/Console.h>

namespace OpenRCT2 { namespace Ui
{
    class InGameConsole final : public InteractiveConsole
    {
    private:
        static constexpr sint32 CONSOLE_MAX_LINES = 300;
        static constexpr sint32 CONSOLE_HISTORY_SIZE = 64;
        static constexpr sint32 CONSOLE_INPUT_SIZE = 256;
        static constexpr sint32 CONSOLE_CARET_FLASH_THRESHOLD = 15;
        static constexpr sint32 CONSOLE_EDGE_PADDING = 4;
        static constexpr sint32 CONSOLE_CARET_WIDTH = 6;

        bool                    _isOpen;
        sint32                  _consoleLeft, _consoleTop, _consoleRight, _consoleBottom;
        sint32                  _lastMainViewportX, _lastMainViewportY;
        std::deque<std::string> _consoleLines;
        utf8                    _consoleCurrentLine[CONSOLE_INPUT_SIZE];
        sint32                  _consoleCaretTicks;
        sint32                  _consoleScrollPos = 0;
        TextInputSession *      _consoleTextInputSession;  
        utf8                    _consoleHistory[CONSOLE_HISTORY_SIZE][CONSOLE_INPUT_SIZE];
        sint32                  _consoleHistoryIndex = 0;
        sint32                  _consoleHistoryCount = 0;

    public:
        InGameConsole();
        InGameConsole(const InGameConsole& src) = delete;

        bool IsOpen() const { return _isOpen; }

        void Clear() override;
        void Open();
        void Close() override;
        void Toggle();
        void WriteLine(const std::string &s, uint32 colourFormat) override;

        void Input(CONSOLE_INPUT input);
        void RefreshCaret();
        void Scroll(sint32 linesToScroll);

        void Update();
        void Draw(rct_drawpixelinfo * dpi) const;

    private:
        void ClearInput();
        void ClearLine();
        void HistoryAdd(const utf8 * src);
        void WritePrompt();
        void ScrollToEnd();
        void Invalidate() const;
        sint32 GetNumVisibleLines() const;
    };
} }
