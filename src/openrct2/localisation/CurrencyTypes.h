/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

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
