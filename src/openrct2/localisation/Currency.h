/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/String.hpp"
#include "../localisation/StringIdType.h"
#include "../util/Util.h"
#include "CurrencyTypes.h"

constexpr size_t kCurrencySymbolMaxSize = 8;
constexpr size_t kCurrencyRateMaxNumDigits = 9;

// Currency format specification - inspired by OpenTTD
struct CurrencyDescriptor
{
    char isoCode[4];
    // Rate is relative to 0.10 GBP
    int32_t rate;
    CurrencyAffix affix_unicode;
    utf8 symbol_unicode[kCurrencySymbolMaxSize];
    CurrencyAffix affix_ascii;
    char symbol_ascii[kCurrencySymbolMaxSize];
    StringId stringId;
};

// List of currency formats
// TODO: refactor into getter
extern CurrencyDescriptor CurrencyDescriptors[EnumValue(CurrencyType::Count)];

/**
 * Loads custom currency saved parameters into {@link CurrencyDescriptors}'
 * custom currency entry
 */
void CurrencyLoadCustomCurrencyConfig();
