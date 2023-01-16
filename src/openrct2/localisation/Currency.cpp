/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Currency.h"

#include "../config/Config.h"
#include "../util/Util.h"
#include "StringIds.h"

// clang-format off
currency_descriptor CurrencyDescriptors[EnumValue(CurrencyType::Count)] = {
    {   "GBP",  10,     CurrencyAffix::Prefix,    "\xC2\xA3",     CurrencyAffix::Suffix,    "GBP",  STR_POUNDS          },  // British Pound
    {   "USD",  10,     CurrencyAffix::Prefix,    "$",            CurrencyAffix::Prefix,    "$",    STR_DOLLARS         },  // US Dollar
    {   "FRF",  10,     CurrencyAffix::Suffix,    "F",            CurrencyAffix::Suffix,    "F",    STR_FRANC           },  // French Franc
    {   "DEM",  10,     CurrencyAffix::Prefix,    "DM",           CurrencyAffix::Prefix,    "DM",   STR_DEUTSCHE_MARK   },  // Deutsche Mark
    {   "JPY",  1000,   CurrencyAffix::Prefix,    "\xC2\xA5",     CurrencyAffix::Suffix,    "YEN",  STR_YEN             },  // Japanese Yen
    {   "ESP",  10,     CurrencyAffix::Suffix,    "Pts",          CurrencyAffix::Suffix,    "Pts",  STR_PESETA          },  // Spanish Peseta
    {   "ITL",  1000,   CurrencyAffix::Prefix,    "L",            CurrencyAffix::Prefix,    "L",    STR_LIRA            },  // Italian Lira
    {   "NLG",  10,     CurrencyAffix::Prefix,    "\xC6\x92 ",    CurrencyAffix::Prefix,    "fl.",  STR_GUILDERS        },  // Dutch Guilder
    {   "SEK",  10,     CurrencyAffix::Suffix,    " kr",          CurrencyAffix::Suffix,    " kr",  STR_KRONA           },  // Swedish Krona
    {   "EUR",  10,     CurrencyAffix::Prefix,    "\xE2\x82\xAC", CurrencyAffix::Suffix,    "EUR",  STR_EUROS           },  // Euro
    {   "KRW",  10000,  CurrencyAffix::Prefix,    "\xE2\x82\xA9", CurrencyAffix::Prefix,    "W",    STR_WON             },  // South Korean Won
    {   "RUB",  1000,   CurrencyAffix::Suffix,    "\xE2\x82\xBD", CurrencyAffix::Prefix,    "R ",   STR_ROUBLE          },  // Russian Rouble
    {   "CZK",  100,    CurrencyAffix::Suffix,    " K\xC4\x8D",   CurrencyAffix::Suffix,    " Kc",  STR_CZECH_KORUNA    },  // Czech koruna
    {   "HKD",  100,    CurrencyAffix::Prefix,    "$",            CurrencyAffix::Prefix,    "HKD",  STR_HONG_KONG_DOLLAR},  // Hong Kong Dollar
    {   "TWD",  1000,   CurrencyAffix::Prefix,    "NT$",          CurrencyAffix::Prefix,    "NT$",  STR_NEW_TAIWAN_DOLLAR}, // New Taiwan Dollar
    {   "CNY",  100,    CurrencyAffix::Prefix,    "CN\xC2\xA5",   CurrencyAffix::Prefix,    "CNY",  STR_CHINESE_YUAN    },  // Chinese Yuan
    {   "HUF",  1000,   CurrencyAffix::Suffix,    " Ft",          CurrencyAffix::Suffix,    " Ft",  STR_HUNGARIAN_FORINT},  // Hungarian Forint
    {   "CTM",  10,     CurrencyAffix::Prefix,    "Ctm",          CurrencyAffix::Prefix,    "Ctm",  STR_CUSTOM_CURRENCY },  // Customizable currency
};
// clang-format on

void CurrencyLoadCustomCurrencyConfig()
{
    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].rate = gConfigGeneral.CustomCurrencyRate;
    CurrencyDescriptors[EnumValue(CurrencyType::Custom)].affix_unicode = gConfigGeneral.CustomCurrencyAffix;
    if (!gConfigGeneral.CustomCurrencySymbol.empty())
    {
        safe_strcpy(
            CurrencyDescriptors[EnumValue(CurrencyType::Custom)].symbol_unicode, gConfigGeneral.CustomCurrencySymbol.c_str(),
            CURRENCY_SYMBOL_MAX_SIZE);
    }
}
