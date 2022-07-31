/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <fstream>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/Version.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/util/Util.h>
#include <vector>

using namespace OpenRCT2;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CONTENT_PANEL,
    WIDX_SCROLL,
    WIDX_OPEN_URL,
};

static constexpr const int32_t WW = 500;
static constexpr const int32_t WH = 400;
static constexpr const rct_string_id WINDOW_TITLE = STR_CHANGELOG_TITLE;
constexpr int32_t MIN_WW = 300;
constexpr int32_t MIN_WH = 250;

static rct_widget _windowChangelogWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({0,  14}, {500, 382}, WindowWidgetType::Resize,      WindowColour::Secondary                               ), // content panel
    MakeWidget({3,  16}, {495, 366}, WindowWidgetType::Scroll,      WindowColour::Secondary, SCROLL_BOTH                  ), // scroll area
    MakeWidget({3, 473}, {300,  14}, WindowWidgetType::Placeholder, WindowColour::Secondary, STR_NEW_RELEASE_DOWNLOAD_PAGE), // changelog button
    WIDGETS_END,
};

// clang-format on

class ChangelogWindow final : public Window
{
    const NewVersionInfo* _newVersionInfo;
    std::vector<std::string> _changelogLines;
    int32_t _changelogLongestLineWidth = 0;
    int _personality = 0;

public:
    /**
     * @brief Retrieves the changelog contents.
     */
    const std::string GetChangelogText()
    {
        auto path = GetChangelogPath();
        auto fs = std::ifstream(fs::u8path(path), std::ios::in);
        if (!fs.is_open())
        {
            throw std::runtime_error("Unable to open " + path);
        }
        return std::string((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    }

    /**
     * @brief Set the Changelog Window's Personality, should be called just after creation. Returns true on success
     *
     * @param personality
     */
    bool SetPersonality(int personality)
    {
        switch (personality)
        {
            case WV_NEW_VERSION_INFO:
                if (!GetContext()->HasNewVersionInfo())
                {
                    return false;
                }
                _personality = WV_NEW_VERSION_INFO;
                NewVersionProcessInfo();
                widgets[WIDX_OPEN_URL].type = WindowWidgetType::Button;
                return true;

            case WV_CHANGELOG:
                if (!ReadChangelogFile())
                {
                    return false;
                }
                _personality = WV_CHANGELOG;
                return true;

            default:
                log_error("Invalid personality for changelog window: %d", personality);
                return false;
        }
    }

    void OnOpen() override
    {
        widgets = _windowChangelogWidgets;

        WindowInitScrollWidgets(*this);
        min_width = MIN_WW;
        min_height = MIN_WH;
        max_width = MIN_WW;
        max_height = MIN_WH;
    }

    void OnResize() override
    {
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();

        max_width = (screenWidth * 4) / 5;
        max_height = (screenHeight * 4) / 5;

        min_width = MIN_WW;
        min_height = MIN_WH;

        auto download_button_width = widgets[WIDX_OPEN_URL].width();
        widgets[WIDX_OPEN_URL].left = (width - download_button_width) / 2;
        widgets[WIDX_OPEN_URL].right = widgets[WIDX_OPEN_URL].left + download_button_width;

        if (width < min_width)
        {
            Invalidate();
            width = min_width;
        }
        if (height < min_height)
        {
            Invalidate();
            height = min_height;
        }
    }

    void OnPrepareDraw() override
    {
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
        widgets[WIDX_CONTENT_PANEL].right = width - 1;
        widgets[WIDX_CONTENT_PANEL].bottom = height - 1;
        widgets[WIDX_SCROLL].right = width - 3;
        widgets[WIDX_SCROLL].bottom = height - 22;
        widgets[WIDX_OPEN_URL].bottom = height - 5;
        widgets[WIDX_OPEN_URL].top = height - 19;
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_OPEN_URL:
                if (_newVersionInfo != nullptr)
                {
                    GetContext()->GetUiContext()->OpenURL(_newVersionInfo->url);
                }
                else
                {
                    log_error("Cannot open URL: NewVersionInfo for ChangelogWindow is undefined!");
                }
                break;
        }
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        const int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

        ScreenCoordsXY screenCoords(3, 3 - lineHeight);
        for (const auto& line : _changelogLines)
        {
            screenCoords.y += lineHeight;
            if (screenCoords.y + lineHeight < dpi.y || screenCoords.y >= dpi.y + dpi.height)
                continue;

            gfx_draw_string(&dpi, screenCoords, line.c_str(), { colours[0] });
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        return ScreenSize(
            _changelogLongestLineWidth + 4,
            static_cast<int32_t>(_changelogLines.size()) * font_get_line_height(FontSpriteBase::MEDIUM));
    }

    // TODO: This probably should be a utility function defined elsewhere for reusability
    /**
     * @brief Reimplementation of Window's GetCentrePositionForNewWindow for ChangelogWindow.
     *
     * @return ScreenCoordsXY
     */
    static ScreenCoordsXY GetCentrePositionForNewWindow(int32_t width, int32_t height)
    {
        auto uiContext = GetContext()->GetUiContext();
        auto screenWidth = uiContext->GetWidth();
        auto screenHeight = uiContext->GetHeight();
        return ScreenCoordsXY{ (screenWidth - width) / 2, std::max(TOP_TOOLBAR_HEIGHT + 1, (screenHeight - height) / 2) };
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
            char version_info[256];

            const char* version_info_ptr = _newVersionInfo->name.c_str();
            format_string(version_info, 256, STR_NEW_RELEASE_VERSION_INFO, &version_info_ptr);

            _changelogLines.emplace_back(version_info);
            _changelogLines.emplace_back("");

            ProcessChangelogText(_newVersionInfo->changelog);
        }
        else
        {
            log_error("ChangelogWindow: Could not process NewVersionInfo, result was undefined");
        }
    }

    /**
     * @brief Get the absolute path for the changelog file
     *
     * @return std::string
     */
    std::string GetChangelogPath()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        return env->GetFilePath(PATHID::CHANGELOG);
    }

    /**
     * @brief Attempts to read the changelog file, returns true on success
     *
     */
    bool ReadChangelogFile()
    {
        std::string _changelogText;
        try
        {
            _changelogText = GetChangelogText();
        }
        catch (const std::bad_alloc&)
        {
            log_error("Unable to allocate memory for changelog.txt");
            return false;
        }
        catch (const std::exception&)
        {
            log_error("Unable to read changelog.txt");
            return false;
        }

        ProcessChangelogText(_changelogText);
        return true;
    }

    /**
     * @brief Ingests a string of text and splits it into lines for the changelog and updates the longest line width for
     * scrolling purposes
     *
     * @param text
     */
    void ProcessChangelogText(const std::string& text)
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
            int32_t linewidth = gfx_get_string_width(line.c_str(), FontSpriteBase::MEDIUM);
            _changelogLongestLineWidth = std::max(linewidth, _changelogLongestLineWidth);
        }
    }
};

rct_window* WindowChangelogOpen(int personality)
{
    auto* window = window_bring_to_front_by_class(WC_CHANGELOG);
    if (window == nullptr)
    {
        // Create a new centred window
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        int32_t width = (screenWidth * 4) / 5;
        int32_t height = (screenHeight * 4) / 5;

        auto pos = ChangelogWindow::GetCentrePositionForNewWindow(width, height);
        auto* newWindow = WindowCreate<ChangelogWindow>(WC_CHANGELOG, pos, width, height, WF_RESIZABLE);
        newWindow->SetPersonality(personality);
        return newWindow;
    }
    return window;
}
