#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

struct Ride;

enum
{
    SHOP_ITEM_BALLOON,
    SHOP_ITEM_TOY,
    SHOP_ITEM_MAP,
    SHOP_ITEM_PHOTO,
    SHOP_ITEM_UMBRELLA,
    SHOP_ITEM_DRINK,
    SHOP_ITEM_BURGER,
    SHOP_ITEM_CHIPS,
    SHOP_ITEM_ICE_CREAM,
    SHOP_ITEM_CANDYFLOSS,
    SHOP_ITEM_EMPTY_CAN,
    SHOP_ITEM_RUBBISH,
    SHOP_ITEM_EMPTY_BURGER_BOX,
    SHOP_ITEM_PIZZA,
    SHOP_ITEM_VOUCHER,
    SHOP_ITEM_POPCORN,
    SHOP_ITEM_HOT_DOG,
    SHOP_ITEM_TENTACLE,
    SHOP_ITEM_HAT,
    SHOP_ITEM_TOFFEE_APPLE,
    SHOP_ITEM_TSHIRT,
    SHOP_ITEM_DOUGHNUT,
    SHOP_ITEM_COFFEE,
    SHOP_ITEM_EMPTY_CUP,
    SHOP_ITEM_CHICKEN,
    SHOP_ITEM_LEMONADE,
    SHOP_ITEM_EMPTY_BOX,
    SHOP_ITEM_EMPTY_BOTTLE = 27,
    SHOP_ITEM_ADMISSION = 31,
    SHOP_ITEM_PHOTO2 = 32,
    SHOP_ITEM_PHOTO3,
    SHOP_ITEM_PHOTO4,
    SHOP_ITEM_PRETZEL,
    SHOP_ITEM_CHOCOLATE,
    SHOP_ITEM_ICED_TEA,
    SHOP_ITEM_FUNNEL_CAKE,
    SHOP_ITEM_SUNGLASSES,
    SHOP_ITEM_BEEF_NOODLES,
    SHOP_ITEM_FRIED_RICE_NOODLES,
    SHOP_ITEM_WONTON_SOUP,
    SHOP_ITEM_MEATBALL_SOUP,
    SHOP_ITEM_FRUIT_JUICE,
    SHOP_ITEM_SOYBEAN_MILK,
    SHOP_ITEM_SUJEONGGWA,
    SHOP_ITEM_SUB_SANDWICH,
    SHOP_ITEM_COOKIE,
    SHOP_ITEM_EMPTY_BOWL_RED,
    SHOP_ITEM_EMPTY_DRINK_CARTON,
    SHOP_ITEM_EMPTY_JUICE_CUP,
    SHOP_ITEM_ROAST_SAUSAGE,
    SHOP_ITEM_EMPTY_BOWL_BLUE,
    SHOP_ITEM_COUNT = 56,
    SHOP_ITEM_NONE = 255
};

struct rct_shop_item_stats
{
    uint16 cost;
    uint16 base_value;
    uint16 hot_value;
    uint16 cold_value;
};

struct rct_shop_item_string_types
{
    rct_string_id price_label;      // Balloon price:
    rct_string_id singular;         // Balloon
    rct_string_id plural;           // Balloons
    rct_string_id indefinite;       // a Balloon
    rct_string_id display;          // "Diamond Heights" Balloon
};

extern uint32 gSamePriceThroughoutParkA;
extern uint32 gSamePriceThroughoutParkB;

extern const money8 DefaultShopItemPrice[SHOP_ITEM_COUNT];
extern const rct_shop_item_string_types ShopItemStringIds[SHOP_ITEM_COUNT];
extern const uint32 ShopItemImage[SHOP_ITEM_COUNT];

money32 get_shop_item_cost(sint32 shopItem);
money16 get_shop_base_value(sint32 shopItem);
money16 get_shop_hot_value(sint32 shopItem);
money16 get_shop_cold_value(sint32 shopItem);
money32 shop_item_get_common_price(Ride *forRide, sint32 shopItem);
bool shop_item_is_photo(sint32 shopItem);
bool shop_item_has_common_price(sint32 shopItem);
bool shop_item_is_food_or_drink(sint32 shopItem);
bool shop_item_is_food(sint32 shopItem);
bool shop_item_is_drink(sint32 shopItem);
bool shop_item_is_souvenir(sint32 shopItem);
