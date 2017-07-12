#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <map>

#include "GeneralSupportHeightCall.hpp"

bool GeneralSupportHeightCall::CallsMatch(SupportCall tileSupportCalls[4]) {
    SupportCall baseCall = tileSupportCalls[0];
    for (int i = 1; i < 4; i++) {
        if (tileSupportCalls[i] != baseCall) return false;
    }

    return true;
}

bool GeneralSupportHeightCall::FindMostCommonSupportCall(SupportCall calls[4], SupportCall *out) {
    std::map<SupportCall, int> map;

    for (int i = 0; i < 4; ++i) {
        if (map.count(calls[i]) == 0) {
            map[calls[i]] = 1;
        } else {
            map[calls[i]] += 1;
        }
    }

    if (map.size() == 1) {
        (*out) = calls[0];
        return true;
    }

    if (map.size() == 2) {
        for (auto &&item : map) {
            if (item.second == 3) {
                (*out) = item.first;
                return true;
            }
        }

        return false;
    }

    return false;
}

bool GeneralSupportHeightCall::AssertEquals(const SupportCall *lhs, const SupportCall *rhs) {
    if (lhs == nullptr && rhs == nullptr) return true;
    if (lhs == nullptr || rhs == nullptr) return false;

    if (lhs->height != rhs->height) return false;
    if (lhs->slope != rhs->slope) return false;

    return true;
}
