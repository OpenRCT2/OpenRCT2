/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <fstream>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/Version.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_SCROLL,
        WIDX_OPEN_URL,
    };

    static constexpr ScreenSize kWindowSize = { 500, 400 };
    static constexpr StringId kWindowTitle = STR_CHANGELOG_TITLE;
    constexpr int32_t kMinimumWindowWidth = 300;
    constexpr int32_t kMinimumWindowHeight = 250;

    // clang-format off
    static constexpr auto _windowChangelogWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({0,  14}, {500, 382}, WidgetType::resize,      WindowColour::secondary                               ), // content panel
        makeWidget({3,  16}, {495, 366}, WidgetType::scroll,      WindowColour::secondary, SCROLL_BOTH                  ), // scroll area
        makeWidget({3, 473}, {300,  14}, WidgetType::placeholder, WindowColour::secondary, STR_NEW_RELEASE_DOWNLOAD_PAGE)  // changelog button
    );
    // clang-format on

    class ChangelogWindow final : public Window
    {
        const NewVersionInfo* _newVersionInfo;
        std::vector<std::string> _changelogLines;
        int32_t _changelogLongestLineWidth = 0;
        WindowView _personality = WindowView::changelog;

    public:
        /**
         * @brief Retrieves the changelog contents.
         */
        const std::string GetText(PathId pathId)
        {
            auto& env = GetContext()->GetPlatformEnvironment();
            auto path = env.GetFilePath(pathId);
            auto fs = std::ifstream(fs::u8path(path), std::ios::in);
            if (!fs.is_open())
            {
                throw std::runtime_error("Unable to open " + path);
            }
            fs.seekg(0, fs.end);
            auto length = fs.tellg();
            fs.seekg(0, fs.beg);
            std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);
            fs.read(buffer.get(), length);
            auto result = std::string(buffer.get(), buffer.get() + length);
            return result;
        }

        /**
         * @brief Set the Changelog Window's Personality, should be called just after creation. Returns true on success
         *
         * @param personality
         */
        bool SetPersonality(WindowView personality)
        {
            switch (personality)
            {
                case WindowView::newVersionInfo:
                    if (!GetContext()->HasNewVersionInfo())
                    {
                        return false;
                    }
                    _personality = WindowView::newVersionInfo;
                    NewVersionProcessInfo();
                    widgets[WIDX_OPEN_URL].type = WidgetType::button;
                    return true;

                case WindowView::changelog:
                    if (!ReadFile(PathId::changelog))
                    {
                        return false;
                    }
                    _personality = WindowView::changelog;
                    widgets[WIDX_TITLE].text = STR_CHANGELOG_TITLE;
                    return true;

                case WindowView::contributors:
                    if (!ReadFile(PathId::contributors))
                    {
                        return false;
                    }
                    _personality = WindowView::contributors;
                    widgets[WIDX_TITLE].text = STR_CONTRIBUTORS_WINDOW;
                    return true;

                default:
                    LOG_ERROR("Invalid personality for changelog window: %d", personality);
                    return false;
            }
        }

        void SetResizeDimensions()
        {
            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();

            WindowSetResize(
                *this, { kMinimumWindowWidth, kMinimumWindowHeight }, { (screenWidth * 4) / 5, (screenHeight * 4) / 5 });
        }

        void onOpen() override
        {
            setWidgets(_windowChangelogWidgets);

            WindowInitScrollWidgets(*this);
            SetResizeDimensions();
        }

        void onResize() override
        {
            SetResizeDimensions();

            auto downloadButtonWidth = widgets[WIDX_OPEN_URL].width() - 1;
            widgets[WIDX_OPEN_URL].left = (width - downloadButtonWidth) / 2;
            widgets[WIDX_OPEN_URL].right = widgets[WIDX_OPEN_URL].left + downloadButtonWidth;
        }

        void onPrepareDraw() override
        {
            widgets[WIDX_SCROLL].right = width - 3;
            widgets[WIDX_SCROLL].bottom = height - 22;
            widgets[WIDX_OPEN_URL].bottom = height - 5;
            widgets[WIDX_OPEN_URL].top = height - 19;
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_OPEN_URL:
                    if (_newVersionInfo != nullptr)
                    {
                        GetContext()->GetUiContext().OpenURL("https://openrct2.io/download/release");
                    }
                    else
                    {
                        LOG_ERROR("Cannot open URL: NewVersionInfo for ChangelogWindow is undefined!");
                    }
                    break;
            }
        }

        void onScrollDraw(int32_t scrollIndex, Drawing::RenderTarget& rt) override
        {
            const int32_t lineHeight = FontGetLineHeight(FontStyle::medium);

            ScreenCoordsXY screenCoords(3, 3 - lineHeight);
            for (const auto& line : _changelogLines)
            {
                screenCoords.y += lineHeight;
                if (screenCoords.y + lineHeight < rt.y || screenCoords.y >= rt.y + rt.height)
                    continue;

                DrawText(rt, screenCoords, { colours[0] }, line.c_str());
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return ScreenSize(
                _changelogLongestLineWidth + 4,
                static_cast<int32_t>(_changelogLines.size()) * FontGetLineHeight(FontStyle::medium));
        }

    private:
        /**
         * @brief Converts NewVersionInfo into changelog lines
         *
         */
        void NewVersionProcessInfo()
        {
            _newVersionInfo = GetContext()->GetNewVersionInfo();
            if (_newVersionInfo != nullptr)
            {
                char versionInfo[256];

                const char* versionInfoPtr = _newVersionInfo->name.c_str();
                FormatStringLegacy(versionInfo, 256, STR_NEW_RELEASE_VERSION_INFO, &versionInfoPtr);

                _changelogLines.emplace_back(versionInfo);
                _changelogLines.emplace_back("");

                ProcessText(_newVersionInfo->changelog);
            }
            else
            {
                LOG_ERROR("ChangelogWindow: Could not process NewVersionInfo, result was undefined");
            }
        }

        /**
         * @brief Attempts to read the changelog file, returns true on success
         *
         */
        bool ReadFile(PathId pathId)
        {
            std::string _text;
            try
            {
                _text = GetText(pathId);
            }
            catch (const std::bad_alloc&)
            {
                LOG_ERROR("Unable to allocate memory for text file");
                return false;
            }
            catch (const std::exception&)
            {
                LOG_ERROR("Unable to read text file");
                return false;
            }

            ProcessText(_text);
            return true;
        }

        /**
         * @brief Ingests a string of text and splits it into lines for the changelog and updates the longest line width for
         * scrolling purposes
         *
         * @param text
         */
        void ProcessText(const std::string& text)
        {
            std::string::size_type pos = 0;
            std::string::size_type prev = 0;
            while ((pos = text.find('\n', prev)) != std::string::npos)
            {
                _changelogLines.push_back(text.substr(prev, pos - prev));
                prev = pos + 1;
            }

            // To get the last substring (or only, if delimiter is not found)
            _changelogLines.push_back(text.substr(prev));

            _changelogLongestLineWidth = 0;
            for (const auto& line : _changelogLines)
            {
                int32_t linewidth = GfxGetStringWidth(line.c_str(), FontStyle::medium);
                _changelogLongestLineWidth = std::max(linewidth, _changelogLongestLineWidth);
            }
        }
    };

    WindowBase* ChangelogOpen(WindowView personality)
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::changelog);
        if (window == nullptr)
        {
            int32_t width = (ContextGetWidth() * 4) / 5;
            int32_t height = (ContextGetHeight() * 4) / 5;
            auto* newWindow = windowMgr->Create<ChangelogWindow>(
                WindowClass::changelog, { width, height }, { WindowFlag::centreScreen, WindowFlag::resizable });
            newWindow->SetPersonality(personality);
            return newWindow;
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
