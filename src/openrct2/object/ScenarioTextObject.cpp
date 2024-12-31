/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioTextObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"

using namespace OpenRCT2;

void ScenarioTextObject::Load()
{
}

void ScenarioTextObject::Unload()
{
}

void ScenarioTextObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "ScenarioTextObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);
}

std::string ScenarioTextObject::GetScenarioName()
{
    return GetStringTable().GetString(ObjectStringID::NAME); // SCENARIO_NAME
}

std::string ScenarioTextObject::GetParkName()
{
    return GetStringTable().GetString(ObjectStringID::PARK_NAME);
}

std::string ScenarioTextObject::GetScenarioDetails()
{
    return GetStringTable().GetString(ObjectStringID::SCENARIO_DETAILS);
}
