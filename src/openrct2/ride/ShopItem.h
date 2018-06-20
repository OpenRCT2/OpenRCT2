/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    uint16_t cost;
    uint16_t base_value;
    uint16_t hot_value;
    uint16_t cold_value;
};

struct rct_shop_item_string_types
{
    rct_string_id price_label;      // Balloon price:
    rct_string_id singular;         // Balloon
    rct_string_id plural;           // Balloons
    rct_string_id indefinite;       // a Balloon
    rct_string_id display;          // "Diamond Heights" Balloon
};

extern uint32_t gSamePriceThroughoutParkA;
extern uint32_t gSamePriceThroughoutParkB;

extern const money8 DefaultShopItemPrice[SHOP_ITEM_COUNT];
extern const rct_shop_item_string_types ShopItemStringIds[SHOP_ITEM_COUNT];
extern const uint32_t ShopItemImage[SHOP_ITEM_COUNT];

money32 get_shop_item_cost(int32_t shopItem);
money16 get_shop_base_value(int32_t shopItem);
money16 get_shop_hot_value(int32_t shopItem);
money16 get_shop_cold_value(int32_t shopItem);
money32 shop_item_get_common_price(Ride *forRide, int32_t shopItem);
bool shop_item_is_photo(int32_t shopItem);
bool shop_item_has_common_price(int32_t shopItem);
bool shop_item_is_food_or_drink(int32_t shopItem);
bool shop_item_is_food(int32_t shopItem);
bool shop_item_is_drink(int32_t shopItem);
bool shop_item_is_souvenir(int32_t shopItem);
