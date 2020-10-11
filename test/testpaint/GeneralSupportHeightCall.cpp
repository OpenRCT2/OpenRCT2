/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GeneralSupportHeightCall.hpp"

#include <map>

bool GeneralSupportHeightCall::CallsMatch(SupportCall tileSupportCalls[4])
{
    SupportCall baseCall = tileSupportCalls[0];
    for (int i = 1; i < 4; i++)
    {
        if (tileSupportCalls[i] != baseCall)
            return false;
    }

    return true;
}

bool GeneralSupportHeightCall::FindMostCommonSupportCall(SupportCall calls[4], SupportCall* out)
{
    std::map<SupportCall, int> map;

    for (int i = 0; i < 4; ++i)
    {
        if (map.count(calls[i]) == 0)
        {
            map[calls[i]] = 1;
        }
        else
        {
            map[calls[i]] += 1;
        }
    }

    if (map.size() == 1)
    {
        (*out) = calls[0];
        return true;
    }

    if (map.size() == 2)
    {
        for (auto&& item : map)
        {
            if (item.second == 3)
            {
                (*out) = item.first;
                return true;
            }
        }

        return false;
    }

    return false;
}

bool GeneralSupportHeightCall::AssertEquals(const SupportCall* lhs, const SupportCall* rhs)
{
    if (lhs == nullptr && rhs == nullptr)
        return true;
    if (lhs == nullptr || rhs == nullptr)
        return false;

    if (lhs->height != rhs->height)
        return false;
    if (lhs->slope != rhs->slope)
        return false;

    return true;
}
