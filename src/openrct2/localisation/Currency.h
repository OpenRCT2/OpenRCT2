/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

// List of currencies
enum class CurrencyType : uint8_t
{
    Pounds,       // British Pound
    Dollars,      // US Dollar
    Franc,        // French Franc
    DeutscheMark, // Deutsche Mark
    Yen,          // Japanese Yen
    Peseta,       // Spanish Peseta
    Lira,         // Italian Lira
    Guilders,     // Dutch Gilder
    Krona,        // Swedish Krona
    Euros,        // Euro
    Won,          // South Korean Won
    Rouble,       // Russian Rouble
    CzechKoruna,  // Czech koruna
    HKD,          // Hong Kong Dollar
    TWD,          // New Taiwan Dollar
    Yuan,         // Chinese Yuan
    Forint,       // Hungarian Forint

    Custom, // Custom currency

    Count // Last item
};

enum class CurrencyAffix
{
    Prefix,
    Suffix
};

#define CURRENCY_SYMBOL_MAX_SIZE 8
#define CURRENCY_RATE_MAX_NUM_DIGITS 9

// Currency format specification - inspired by OpenTTD
struct currency_descriptor
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
extern currency_descriptor CurrencyDescriptors[static_cast<uint8_t>(CurrencyType::Count)];

/**
 * Loads custom currency saved parameters into {@link CurrencyDescriptors}'
 * custom currency entry
 */
void currency_load_custom_currency_config();
