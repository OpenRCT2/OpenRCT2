/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepNamesObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"

using namespace OpenRCT2;

void PeepNamesObject::Load()
{
}

void PeepNamesObject::Unload()
{
}

void PeepNamesObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "PeepNamesObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);

    Guard::Assert(root["given_names"].is_array(), "PeepNamesObject::ReadJson expects given_names to be an array");
    _givenNames = root["given_names"].get<std::vector<std::string>>();
    std::sort(_givenNames.begin(), _givenNames.end());

    Guard::Assert(root["surnames"].is_array(), "PeepNamesObject::ReadJson expects surnames to be an array");
    _surnames = root["surnames"].get<std::vector<std::string>>();
    std::sort(_surnames.begin(), _surnames.end());
}

std::string PeepNamesObject::GetGivenNameAt(size_t index) const
{
    return _givenNames[index % _givenNames.size()];
}

std::string PeepNamesObject::GetSurnameAt(size_t index) const
{
    return _surnames[index % _surnames.size()];
}
