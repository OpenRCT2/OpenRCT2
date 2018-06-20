/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef CURRENCY_H
#define CURRENCY_H

#include "../common.h"

// List of currencies
enum CURRENCY_TYPE {
    CURRENCY_POUNDS,            // British Pound
    CURRENCY_DOLLARS,           // US Dollar
    CURRENCY_FRANC,             // French Franc
    CURRENCY_DEUTSCHMARK,       // Deutsche Mark
    CURRENCY_YEN,               // Japanese Yen
    CURRENCY_PESETA,            // Spanish Peseta
    CURRENCY_LIRA,              // Italian Lira
    CURRENCY_GUILDERS,          // Dutch Gilder
    CURRENCY_KRONA,             // Swedish Krona
    CURRENCY_EUROS,             // Euro
    CURRENCY_WON,               // South Korean Won
    CURRENCY_ROUBLE,            // Russian Rouble
    CURRENCY_CZECH_KORUNA,      // Czech koruna
    CURRENCY_HKD,               // Hong Kong Dollar
    CURRENCY_TWD,               // New Taiwan Dollar
    CURRENCY_YUAN,              // Chinese Yuan

    CURRENCY_CUSTOM,            // Custom currency

    CURRENCY_END                // Last item
};

enum CURRENCY_AFFIX {
    CURRENCY_PREFIX,
    CURRENCY_SUFFIX
};

#define CURRENCY_SYMBOL_MAX_SIZE 8
#define CURRENCY_RATE_MAX_NUM_DIGITS 9

// Currency format specification - inspired by OpenTTD
struct currency_descriptor {
    char isoCode[4];
    // Rate is relative to 0.10 GBP
    int32_t rate;
    uint8_t affix_unicode;
    utf8 symbol_unicode[CURRENCY_SYMBOL_MAX_SIZE];
    uint8_t affix_ascii;
    char symbol_ascii[CURRENCY_SYMBOL_MAX_SIZE];
    rct_string_id stringId;
};

// List of currency formats
extern currency_descriptor CurrencyDescriptors[CURRENCY_END];

/**
 * Loads custom currency saved parameters into {@link CurrencyDescriptors}'
 * custom currency entry
 */
void currency_load_custom_currency_config();

#endif
