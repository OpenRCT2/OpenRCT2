/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "CustomMenu.h"

namespace OpenRCT2::Scripting
{
    std::vector<CustomToolbarMenuItem> CustomMenuItems;

    static void RemoveMenuItems(std::shared_ptr<Plugin> owner)
    {
        auto& menuItems = CustomMenuItems;
        for (auto it = menuItems.begin(); it != menuItems.end();)
        {
            if (it->Owner == owner)
            {
                it = menuItems.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void InitialiseCustomMenuItems(ScriptEngine& scriptEngine)
    {
        scriptEngine.SubscribeToPluginStoppedEvent([](std::shared_ptr<Plugin> plugin) -> void { RemoveMenuItems(plugin); });
    }
} // namespace OpenRCT2::Scripting

#endif
