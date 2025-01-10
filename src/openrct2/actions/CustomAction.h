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

    #include "GameAction.h"

class CustomAction final : public GameActionBase<GameCommand::Custom>
{
private:
    std::string _id;
    std::string _json;
    std::string _pluginName;

public:
    CustomAction() = default;
    CustomAction(const std::string& id, const std::string& json, const std::string& pluginName);

    std::string GetId() const;
    std::string GetJson() const;
    std::string GetPluginName() const;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};

#endif
