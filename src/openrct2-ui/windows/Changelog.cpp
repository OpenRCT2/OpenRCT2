/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/localisation/Formatting.h>
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
static constexpr const StringId WINDOW_TITLE = STR_CHANGELOG_TITLE;
constexpr int32_t MIN_WW = 300;
constexpr int32_t MIN_WH = 250;

static Widget _windowChangelogWidgets[] = {
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
    const std::string GetText(PATHID pathId)
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto path = env->GetFilePath(pathId);
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
                if (!ReadFile(PATHID::CHANGELOG))
                {
                    return false;
                }
                _personality = WV_CHANGELOG;
                widgets[WIDX_TITLE].text = STR_CHANGELOG_TITLE;
                return true;

            case WV_CONTRIBUTORS:
                if (!ReadFile(PATHID::CONTRIBUTORS))
                {
                    return false;
                }
                _personality = WV_CONTRIBUTORS;
                widgets[WIDX_TITLE].text = STR_CONTRIBUTORS_WINDOW;
                return true;

            default:
                LOG_ERROR("Invalid personality for changelog window: %d", personality);
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
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();

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
        ResizeFrameWithPage();
        widgets[WIDX_SCROLL].right = width - 3;
        widgets[WIDX_SCROLL].bottom = height - 22;
        widgets[WIDX_OPEN_URL].bottom = height - 5;
        widgets[WIDX_OPEN_URL].top = height - 19;
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
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
                    LOG_ERROR("Cannot open URL: NewVersionInfo for ChangelogWindow is undefined!");
                }
                break;
        }
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        const int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);

        ScreenCoordsXY screenCoords(3, 3 - lineHeight);
        for (const auto& line : _changelogLines)
        {
            screenCoords.y += lineHeight;
            if (screenCoords.y + lineHeight < dpi.y || screenCoords.y >= dpi.y + dpi.height)
                continue;

            GfxDrawString(&dpi, screenCoords, line.c_str(), { colours[0] });
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        return ScreenSize(
            _changelogLongestLineWidth + 4,
            static_cast<int32_t>(_changelogLines.size()) * FontGetLineHeight(FontStyle::Medium));
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
            FormatStringLegacy(version_info, 256, STR_NEW_RELEASE_VERSION_INFO, &version_info_ptr);

            _changelogLines.emplace_back(version_info);
            _changelogLines.emplace_back("");

            ProcessText(_newVersionInfo->changelog);
        }
        else
        {
            LOG_ERROR("ChangelogWindow: Could not process NewVersionInfo, result was undefined");
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
    bool ReadFile(PATHID pathId)
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
            int32_t linewidth = GfxGetStringWidth(line.c_str(), FontStyle::Medium);
            _changelogLongestLineWidth = std::max(linewidth, _changelogLongestLineWidth);
        }
    }
};

WindowBase* WindowChangelogOpen(int personality)
{
    auto* window = WindowBringToFrontByClass(WindowClass::Changelog);
    if (window == nullptr)
    {
        // Create a new centred window
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        int32_t width = (screenWidth * 4) / 5;
        int32_t height = (screenHeight * 4) / 5;

        auto pos = ChangelogWindow::GetCentrePositionForNewWindow(width, height);
        auto* newWindow = WindowCreate<ChangelogWindow>(WindowClass::Changelog, pos, width, height, WF_RESIZABLE);
        newWindow->SetPersonality(personality);
        return newWindow;
    }
    return window;
}
