/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Marketing.h"
#include "StringIdType.h"

#include <string>

std::string FormatStringIDLegacy(StringId format, const void* args);
void FormatStringToUpper(char* dest, size_t size, StringId format, const void* args);

constexpr size_t kUserStringMaxLength = 32;

constexpr size_t CommonTextBufferSize = 512;

extern thread_local char gCommonStringFormatBuffer[CommonTextBufferSize];

extern const StringId ObjectiveNames[12];
extern const StringId ResearchFundingLevelNames[4];
extern const StringId MarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3];
extern const StringId RideInspectionIntervalNames[];
extern const StringId PeepThoughts[174];
