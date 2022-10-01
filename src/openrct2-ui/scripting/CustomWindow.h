/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../interface/Window.h"

#    include <memory>
#    include <optional>
#    include <string_view>

namespace OpenRCT2::Ui::Windows
{
    class CustomListView;

    std::string GetWindowTitle(rct_window* w);
    void UpdateWindowTitle(rct_window* w, std::string_view value);
    void UpdateWindowTab(rct_window* w, int32_t tabIndex);
    void UpdateWidgetText(rct_window* w, WidgetIndex widget, std::string_view string_view);
    void UpdateWidgetItems(rct_window* w, WidgetIndex widgetIndex, const std::vector<std::string>& items);
    void UpdateWidgetColour(rct_window* w, WidgetIndex widgetIndex, colour_t colour);
    void UpdateWidgetSelectedIndex(rct_window* w, WidgetIndex widgetIndex, int32_t selectedIndex);
    std::vector<std::string> GetWidgetItems(rct_window* w, WidgetIndex widgetIndex);
    colour_t GetWidgetColour(rct_window* w, WidgetIndex widgetIndex);
    int32_t GetWidgetSelectedIndex(rct_window* w, WidgetIndex widgetIndex);
    rct_window* FindCustomWindowByClassification(std::string_view classification);
    std::optional<WidgetIndex> FindWidgetIndexByName(rct_window* w, std::string_view name);
    std::string GetWidgetName(rct_window* w, WidgetIndex widgetIndex);
    void SetWidgetName(rct_window* w, WidgetIndex widgetIndex, std::string_view name);
    CustomListView* GetCustomListView(rct_window* w, WidgetIndex widgetIndex);
    int32_t GetWidgetMaxLength(rct_window* w, WidgetIndex widgetIndex);
    void SetWidgetMaxLength(rct_window* w, WidgetIndex widgetIndex, int32_t value);
    void CloseWindowsOwnedByPlugin(std::shared_ptr<Plugin> plugin);
} // namespace OpenRCT2::Ui::Windows

#endif
