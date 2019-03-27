/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FunctionCall.hpp"

#include <openrct2/common.h>

namespace PaintIntercept
{
    void InitHooks();

    void ClearCalls();
    int GetCalls(function_call* buffer);

    void SetSimulateWoodenSupports(bool enabled);
}; // namespace PaintIntercept
