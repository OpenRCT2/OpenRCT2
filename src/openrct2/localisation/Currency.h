/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "../util/Util.h"
#include "CurrencyType.h"

enum class CurrencyAffix
{
    Prefix,
    Suffix
};

#define CURRENCY_SYMBOL_MAX_SIZE 8
#define CURRENCY_RATE_MAX_NUM_DIGITS 9

// Currency format specification - inspired by OpenTTD
struct CurrencyDescriptor
{
    char isoCode[4];
    // Rate is relative to 0.10 GBP
    int32_t rate;
    CurrencyAffix affix_unicode;
    utf8 symbol_unicode[CURRENCY_SYMBOL_MAX_SIZE];
    CurrencyAffix affix_ascii;
    char symbol_ascii[CURRENCY_SYMBOL_MAX_SIZE];
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
