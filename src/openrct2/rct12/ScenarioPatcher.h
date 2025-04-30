/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

namespace OpenRCT2::RCT12
{
    void FetchAndApplyScenarioPatch(u8string_view scenarioPath);
    void ApplyScenarioPatch(u8string_view scenarioPatchFile, u8string scenarioSHA);
    /*SetDryRun should be used only for testing*/
    void SetDryRun(bool enable);
} // namespace OpenRCT2::RCT12
