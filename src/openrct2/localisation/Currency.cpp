/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Currency.h"

#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "Formatting.h"
#include "StringIds.h"

using namespace OpenRCT2;

// clang-format off
CurrencyDescriptor CurrencyDescriptors[EnumValue(CurrencyType::Count)] = {
    {   "GBP",  10,     CurrencyAffix::Prefix,    u8"£",      CurrencyAffix::Suffix,    "GBP",  STR_POUNDS          },  // British Pound
    {   "USD",  10,     CurrencyAffix::Prefix,    u8"$",      CurrencyAffix::Prefix,    "$",    STR_DOLLARS         },  // US Dollar
    {   "FRF",  10,     CurrencyAffix::Suffix,    u8"F",      CurrencyAffix::Suffix,    "F",    STR_FRANC           },  // French Franc
    {   "DEM",  10,     CurrencyAffix::Prefix,    u8"DM",     CurrencyAffix::Prefix,    "DM",   STR_DEUTSCHE_MARK   },  // Deutsche Mark
    {   "JPY",  1000,   CurrencyAffix::Prefix,    u8"¥",      CurrencyAffix::Suffix,    "YEN",  STR_YEN             },  // Japanese Yen
    {   "ESP",  10,     CurrencyAffix::Suffix,    u8"Pts",    CurrencyAffix::Suffix,    "Pts",  STR_PESETA          },  // Spanish Peseta
    {   "ITL",  1000,   CurrencyAffix::Prefix,    u8"L",      CurrencyAffix::Prefix,    "L",    STR_LIRA            },  // Italian Lira
    {   "NLG",  10,     CurrencyAffix::Prefix,    u8"ƒ ",     CurrencyAffix::Prefix,    "fl.",  STR_GUILDERS        },  // Dutch Guilder
    {   "SEK",  100,    CurrencyAffix::Suffix,    u8" kr",    CurrencyAffix::Suffix,    " kr",  STR_KRONA           },  // Swedish Krona
    {   "EUR",  10,     CurrencyAffix::Prefix,    u8"€",      CurrencyAffix::Suffix,    "EUR",  STR_EUROS           },  // Euro
    {   "KRW",  10000,  CurrencyAffix::Prefix,    u8"₩",      CurrencyAffix::Prefix,    "W",    STR_WON             },  // South Korean Won
    {   "RUB",  1000,   CurrencyAffix::Suffix,    u8"₽",      CurrencyAffix::Prefix,    "R ",   STR_ROUBLE          },  // Russian Rouble
    {   "CZK",  100,    CurrencyAffix::Suffix,    u8" Kč",    CurrencyAffix::Suffix,   " Kc",  STR_CZECH_KORUNA    },  // Czech koruna
    {   "HKD",  100,    CurrencyAffix::Prefix,    u8"$",      CurrencyAffix::Prefix,    "HKD",  STR_HONG_KONG_DOLLAR},  // Hong Kong Dollar
    {   "TWD",  1000,   CurrencyAffix::Prefix,    u8"NT$",    CurrencyAffix::Prefix,    "NT$",  STR_NEW_TAIWAN_DOLLAR}, // New Taiwan Dollar
    {   "CNY",  100,    CurrencyAffix::Prefix,    u8"CN¥",    CurrencyAffix::Prefix,    "CNY",  STR_CHINESE_YUAN    },  // Chinese Yuan
    {   "HUF",  1000,   CurrencyAffix::Suffix,    u8" Ft",    CurrencyAffix::Suffix,    " Ft",  STR_HUNGARIAN_FORINT},  // Hungarian Forint
    {   "CTM",  10,     CurrencyAffix::Prefix,    u8"Ctm",    CurrencyAffix::Prefix,    "Ctm",  STR_CUSTOM_CURRENCY },  // Customizable currency
};
// clang-format on

void CurrencyLoadCustomCurrencyConfig()
{
    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate = Config::Get().general.CustomCurrencyRate;
    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode = Config::Get().general.CustomCurrencyAffix;
    if (!Config::Get().general.CustomCurrencySymbol.empty())
    {
        String::safeUtf8Copy(
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode,
            Config::Get().general.CustomCurrencySymbol.c_str(), kCurrencySymbolMaxSize);
    }
}

money64 StringToMoney(const char* string_to_monetise)
{
    const char* decimal_char = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
    const CurrencyDescriptor* currencyDesc = &CurrencyDescriptors[EnumValue(Config::Get().general.CurrencyFormat)];
    char processedString[128] = {};

    Guard::Assert(strlen(string_to_monetise) < sizeof(processedString));

    uint32_t numNumbers = 0;
    bool hasMinus = false;
    bool hasDecSep = false;
    const char* src_ptr = string_to_monetise;
    char* dst_ptr = processedString;

    // Process the string, keeping only numbers decimal, and minus sign(s).
    while (*src_ptr != '\0')
    {
        if (*src_ptr >= '0' && *src_ptr <= '9')
        {
            numNumbers++;
        }
        else if (*src_ptr == decimal_char[0])
        {
            if (hasDecSep)
                return kMoney64Undefined;
            hasDecSep = true;

            // Replace localised decimal separator with an English one.
            *dst_ptr++ = '.';
            src_ptr++;
            continue;
        }
        else if (*src_ptr == '-')
        {
            if (hasMinus)
                return kMoney64Undefined;
            hasMinus = true;
        }
        else
        {
            // Skip invalid characters.
            src_ptr++;
            continue;
        }

        // Copy numeric values.
        *dst_ptr++ = *src_ptr;
        src_ptr++;
    }

    // Terminate destination string.
    *dst_ptr = '\0';

    if (numNumbers == 0)
        return kMoney64Undefined;

    if (hasMinus && processedString[0] != '-')
    {
        // If there is a minus sign, it has to be at position 0 in order to be valid.
        return kMoney64Undefined;
    }

    // Due to the nature of strstr and strtok, decimals at the very beginning will be ignored, causing
    // ".1" to be interpreted as "1". To prevent this, prefix with "0" if decimal is at the beginning.
    if (processedString[0] == decimal_char[0])
    {
        for (size_t i = strlen(processedString); i >= 1; i--)
            processedString[i] = processedString[i - 1];
        processedString[0] = '0';
    }

    auto number = std::stod(processedString, nullptr);
    number /= (currencyDesc->rate / 10.0);

    return ToMoney64FromGBP(number);
}

/**
 *
 * @param amount The amount in tens of pounds, e.g. 123 = £ 12.30
 * @param buffer_to_put_value_to Output parameter.
 * @param buffer_len Length of the buffer.
 * @param forceDecimals Show decimals, even if the amount does not have them. Will be ignored if the current exchange
 *                          rate is too big to have decimals.
 */
void MoneyToString(money64 amount, char* buffer_to_put_value_to, size_t buffer_len, bool forceDecimals)
{
    if (amount == kMoney64Undefined)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
        return;
    }

    const CurrencyDescriptor& currencyDesc = CurrencyDescriptors[EnumValue(Config::Get().general.CurrencyFormat)];

    const char* sign = amount >= 0 ? "" : "-";
    const uint64_t a = std::abs(amount) * currencyDesc.rate;
    const unsigned long long whole = a / 100;
    const unsigned long long decimal = a % 100;

    bool amountIsInteger = (whole > 0) && decimal == 0;

    // If whole and decimal exist
    if ((whole > 0 && decimal > 0) || (amountIsInteger && forceDecimals && currencyDesc.rate < 100))
    {
        const char* decimalChar = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
        auto precedingZero = (decimal < 10) ? "0" : "";
        snprintf(buffer_to_put_value_to, buffer_len, "%s%llu%s%s%llu", sign, whole, decimalChar, precedingZero, decimal);
    }
    // If whole exists, but not decimal
    else if (amountIsInteger)
    {
        snprintf(buffer_to_put_value_to, buffer_len, "%s%llu", sign, whole);
    }
    // If decimal exists, but not whole
    else if (whole == 0 && decimal > 0)
    {
        const char* decimalChar = LanguageGetString(STR_LOCALE_DECIMAL_POINT);
        snprintf(buffer_to_put_value_to, buffer_len, "%s0%s%llu", sign, decimalChar, decimal);
    }
    else
    {
        snprintf(buffer_to_put_value_to, buffer_len, "0");
    }
}
