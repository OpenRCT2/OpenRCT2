/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "CompetitiveState.h"
#include "../core/JsonFwd.hpp"

#include <optional>

namespace OpenRCT2::Competitive
{
    [[nodiscard]] json_t ToJson(const ScenarioIdentity& value);
    [[nodiscard]] json_t ToJson(const MatchRules& value);
    [[nodiscard]] json_t ToJson(const ParkMetrics& value);
    [[nodiscard]] json_t ToJson(const MatchState& value);

    [[nodiscard]] std::optional<ScenarioIdentity> ScenarioIdentityFromJson(const json_t& value);
    [[nodiscard]] std::optional<MatchRules> MatchRulesFromJson(const json_t& value);
    [[nodiscard]] std::optional<ParkMetrics> ParkMetricsFromJson(const json_t& value);
    [[nodiscard]] std::optional<MatchState> MatchStateFromJson(const json_t& value);
} // namespace OpenRCT2::Competitive

