/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2
{
    // List of currencies
    enum class CurrencyType : uint8_t
    {
        pounds,          // British Pound
        dollars,         // US Dollar
        franc,           // French Franc
        deutscheMark,    // Deutsche Mark
        yen,             // Japanese Yen
        peseta,          // Spanish Peseta
        lira,            // Italian Lira
        guilders,        // Dutch Gilder
        krona,           // Swedish Krona
        euros,           // Euro
        won,             // South Korean Won
        rouble,          // Russian Rouble
        czechKoruna,     // Czech koruna
        hongKongDollar,  // Hong Kong Dollar
        newTaiwanDollar, // New Taiwan Dollar
        yuan,            // Chinese Yuan
        forint,          // Hungarian Forint
        zloty,           // Polish Złoty

        custom, // Custom currency

        count, // Last item
    };

    enum class CurrencyAffix : uint8_t
    {
        prefix,
        suffix
    };
} // namespace OpenRCT2
