/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../interface/Window.h"

#    include <optional>
#    include <string_view>

namespace OpenRCT2::Ui::Windows
{
    class CustomListView;

    std::string GetWindowTitle(rct_window* w);
    void UpdateWindowTitle(rct_window* w, std::string_view value);
    void UpdateWidgetText(rct_window* w, rct_widgetindex widget, std::string_view string_view);
    void UpdateWidgetItems(rct_window* w, rct_widgetindex widgetIndex, const std::vector<std::string>& items);
    void UpdateWidgetColour(rct_window* w, rct_widgetindex widgetIndex, colour_t colour);
    void UpdateWidgetSelectedIndex(rct_window* w, rct_widgetindex widgetIndex, int32_t selectedIndex);
    std::vector<std::string> GetWidgetItems(rct_window* w, rct_widgetindex widgetIndex);
    colour_t GetWidgetColour(rct_window* w, rct_widgetindex widgetIndex);
    int32_t GetWidgetSelectedIndex(rct_window* w, rct_widgetindex widgetIndex);
    rct_window* FindCustomWindowByClassification(std::string_view classification);
    std::optional<rct_widgetindex> FindWidgetIndexByName(rct_window* w, std::string_view name);
    std::string GetWidgetName(rct_window* w, rct_widgetindex widgetIndex);
    void SetWidgetName(rct_window* w, rct_widgetindex widgetIndex, std::string_view name);
    CustomListView* GetCustomListView(rct_window* w, rct_widgetindex widgetIndex);
    int32_t GetWidgetMaxLength(rct_window* w, rct_widgetindex widgetIndex);
    void SetWidgetMaxLength(rct_window* w, rct_widgetindex widgetIndex, int32_t value);
    void CloseWindowsOwnedByPlugin(std::shared_ptr<Plugin> plugin);
} // namespace OpenRCT2::Ui::Windows

#endif
