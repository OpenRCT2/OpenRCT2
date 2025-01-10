/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../interface/Window.h"

    #include <memory>
    #include <optional>
    #include <string_view>

namespace OpenRCT2::Ui::Windows
{
    class CustomListView;

    std::string GetWindowTitle(WindowBase* w);
    void UpdateWindowTitle(WindowBase* w, std::string_view value);
    void UpdateWindowTab(WindowBase* w, int32_t tabIndex);
    void UpdateWidgetText(WindowBase* w, WidgetIndex widget, std::string_view string_view);
    void UpdateWidgetItems(WindowBase* w, WidgetIndex widgetIndex, const std::vector<std::string>& items);
    void UpdateWidgetColour(WindowBase* w, WidgetIndex widgetIndex, colour_t colour);
    void UpdateWidgetSelectedIndex(WindowBase* w, WidgetIndex widgetIndex, int32_t selectedIndex);
    std::vector<std::string> GetWidgetItems(WindowBase* w, WidgetIndex widgetIndex);
    colour_t GetWidgetColour(WindowBase* w, WidgetIndex widgetIndex);
    int32_t GetWidgetSelectedIndex(WindowBase* w, WidgetIndex widgetIndex);
    WindowBase* FindCustomWindowByClassification(std::string_view classification);
    std::optional<WidgetIndex> FindWidgetIndexByName(WindowBase* w, std::string_view name);
    std::string GetWidgetName(WindowBase* w, WidgetIndex widgetIndex);
    void SetWidgetName(WindowBase* w, WidgetIndex widgetIndex, std::string_view name);
    std::string GetWidgetTooltip(WindowBase* w, WidgetIndex widgetIndex);
    void SetWidgetTooltip(WindowBase* w, WidgetIndex widgetIndex, std::string_view tooltip);
    CustomListView* GetCustomListView(WindowBase* w, WidgetIndex widgetIndex);
    int32_t GetWidgetMaxLength(WindowBase* w, WidgetIndex widgetIndex);
    void SetWidgetMaxLength(WindowBase* w, WidgetIndex widgetIndex, int32_t value);
    void CloseWindowsOwnedByPlugin(std::shared_ptr<Plugin> plugin);
} // namespace OpenRCT2::Ui::Windows

#endif
