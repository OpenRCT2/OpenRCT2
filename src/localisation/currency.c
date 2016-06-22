#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "currency.h"
#include "string_ids.h"

currency_descriptor CurrencyDescriptors[CURRENCY_END] = {
	{	"GBP",	10,		CURRENCY_PREFIX,	"\xC2\xA3",		CURRENCY_SUFFIX,	"GBP",	STR_POUNDS			},	// British Pound
	{	"USD",	10,		CURRENCY_PREFIX,	"$",			CURRENCY_PREFIX,	"$",	STR_DOLLARS			},	// US Dollar
	{	"FRF",	10,		CURRENCY_SUFFIX,	"F",			CURRENCY_SUFFIX,	"F",	STR_FRANC			},	// French Franc
	{	"DEM",	10,		CURRENCY_PREFIX,	"DM",			CURRENCY_PREFIX,	"DM",	STR_DEUTSCHMARK		},	// Deutschmark
	{	"JPY",	1000,	CURRENCY_PREFIX,	"\xC2\xA5",		CURRENCY_SUFFIX,	"YEN",	STR_YEN				},	// Japanese Yen
	{	"ESP",	10,		CURRENCY_SUFFIX,	"Pts",			CURRENCY_SUFFIX,	"Pts",	STR_PESETA			},	// Spanish Peseta
	{	"ITL",	1000,	CURRENCY_PREFIX,	"L",			CURRENCY_PREFIX,	"L",	STR_LIRA			},	// Italian Lira
	{	"NLG",	10,		CURRENCY_PREFIX,	"\xC6\x92",		CURRENCY_PREFIX,	"fl.",	STR_GUILDERS		},	// Dutch Guilder
	{	"SEK",	10,		CURRENCY_SUFFIX,	" kr",			CURRENCY_SUFFIX,	" kr",	STR_KRONA			},	// Swedish Krona
	{	"EUR",	10,		CURRENCY_PREFIX,	"\xE2\x82\xAC",	CURRENCY_SUFFIX,	"EUR",	STR_EUROS			},	// Euro
	{	"KRW",	10000,	CURRENCY_PREFIX,	"\xE2\x82\xA9",	CURRENCY_PREFIX,	"W",	STR_WON				},	// South Korean Won
	{	"RUB",	1000,	CURRENCY_PREFIX,	"R ",			CURRENCY_PREFIX,	"R ",	STR_ROUBLE			},	// Russian Rouble
	{	"CZK",	100,	CURRENCY_SUFFIX,	" K\xC4\x8D",	CURRENCY_SUFFIX,	" Kc",	STR_CZECH_KORUNA	},	// Czech koruna
	{	"HKD",	100,	CURRENCY_PREFIX,	"$",			CURRENCY_PREFIX,	"HKD",	STR_HONG_KONG_DOLLAR},	// Hong Kong Dollar
	{	"TWD",	1000,	CURRENCY_PREFIX,	"NT$",			CURRENCY_PREFIX,	"NT$",	STR_NEW_TAIWAN_DOLLAR},	// New Taiwan Dollar
	{	"CNY",	100,	CURRENCY_PREFIX,	"CN\xC2\xA5",	CURRENCY_PREFIX,	"CNY",	STR_CHINESE_YUAN	},	// Chinese Yuan
	{	"CTM",	1,		CURRENCY_PREFIX,	"Ctm",			CURRENCY_PREFIX,	"Ctm",	STR_CUSTOM_CURRENCY	},	// Customizable currency
};
