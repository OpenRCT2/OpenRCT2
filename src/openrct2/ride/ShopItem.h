/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

using ShopItemIndex = uint8_t;

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

struct ShopItemStrings
{
    rct_string_id PriceLabel; // Balloon price:
    rct_string_id Singular;   // Balloon
    rct_string_id Plural;     // Balloons
    rct_string_id Indefinite; // a Balloon
    rct_string_id Display;    // "Diamond Heights" Balloon
};

struct ShopItemDescriptor
{
    uint16_t Cost;
    uint16_t BaseValue;
    uint16_t HotValue;
    uint16_t ColdValue;
    money8 DefaultPrice;
    uint32_t Image;
    ShopItemStrings Naming;
    uint16_t Flags;

    bool HasFlag(const uint16_t flag) const;
    bool IsFood() const;
    bool IsDrink() const;
    bool IsFoodOrDrink() const;
    bool IsSouvenir() const;
    bool IsPhoto() const;
};

enum
{
    SHOP_ITEM_FLAG_IS_FOOD = (1 << 0),
    SHOP_ITEM_FLAG_IS_DRINK = (1 << 1),
    SHOP_ITEM_FLAG_IS_SOUVENIR = (1 << 2),
    SHOP_ITEM_FLAG_IS_PHOTO = (1 << 3),
};

extern const ShopItemDescriptor ShopItems[SHOP_ITEM_COUNT];
extern uint64_t gSamePriceThroughoutPark;

money32 shop_item_get_common_price(Ride* forRide, const int32_t shopItem);
bool shop_item_has_common_price(const int32_t shopItem);
