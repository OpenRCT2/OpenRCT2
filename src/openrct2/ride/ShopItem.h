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
#include "../util/Util.h"

struct Ride;

enum class ShopItem : uint8_t
{
    SHOP_ITEM_BALLOON,           // for PEEP_ITEM was (1<<0)
    SHOP_ITEM_TOY,               // for PEEP_ITEM was (1<<1)
    SHOP_ITEM_MAP,               // for PEEP_ITEM was (1<<2)
    SHOP_ITEM_PHOTO,             // for PEEP_ITEM was (1<<3)
    SHOP_ITEM_UMBRELLA,          // for PEEP_ITEM was (1<<4)
    SHOP_ITEM_DRINK,             // for PEEP_ITEM was (1<<5)
    SHOP_ITEM_BURGER,            // for PEEP_ITEM was (1<<6)
    SHOP_ITEM_CHIPS,             // for PEEP_ITEM was (1<<7)
    SHOP_ITEM_ICE_CREAM,         // for PEEP_ITEM was (1<<8)
    SHOP_ITEM_CANDYFLOSS,        // for PEEP_ITEM was (1<<9)
    SHOP_ITEM_EMPTY_CAN,         // for PEEP_ITEM was (1<<10)
    SHOP_ITEM_RUBBISH,           // for PEEP_ITEM was (1<<11)
    SHOP_ITEM_EMPTY_BURGER_BOX,  // for PEEP_ITEM was (1<<12)
    SHOP_ITEM_PIZZA,             // for PEEP_ITEM was (1<<13)
    SHOP_ITEM_VOUCHER,           // for PEEP_ITEM was (1<<14)
    SHOP_ITEM_POPCORN,           // for PEEP_ITEM was (1<<15)
    SHOP_ITEM_HOT_DOG,           // for PEEP_ITEM was (1<<16)
    SHOP_ITEM_TENTACLE,          // for PEEP_ITEM was (1<<17)
    SHOP_ITEM_HAT,               // for PEEP_ITEM was (1<<18)
    SHOP_ITEM_TOFFEE_APPLE,      // for PEEP_ITEM was (1<<19)
    SHOP_ITEM_TSHIRT,            // for PEEP_ITEM was (1<<20)
    SHOP_ITEM_DOUGHNUT,          // for PEEP_ITEM was (1<<21)
    SHOP_ITEM_COFFEE,            // for PEEP_ITEM was (1<<22)
    SHOP_ITEM_EMPTY_CUP,         // for PEEP_ITEM was (1<<23)
    SHOP_ITEM_CHICKEN,           // for PEEP_ITEM was (1<<24)
    SHOP_ITEM_LEMONADE,          // for PEEP_ITEM was (1<<25)
    SHOP_ITEM_EMPTY_BOX,         // for PEEP_ITEM was (1<<26)
    SHOP_ITEM_EMPTY_BOTTLE = 27, // for PEEP_ITEM was (1<<27)
    SHOP_ITEM_ADMISSION = 31,
    SHOP_ITEM_PHOTO2 = 32,        // for PEEP_ITEM was (1<<0)
    SHOP_ITEM_PHOTO3,             // for PEEP_ITEM was (1<<1)
    SHOP_ITEM_PHOTO4,             // for PEEP_ITEM was (1<<2)
    SHOP_ITEM_PRETZEL,            // for PEEP_ITEM was (1<<3)
    SHOP_ITEM_CHOCOLATE,          // for PEEP_ITEM was (1<<4)
    SHOP_ITEM_ICED_TEA,           // for PEEP_ITEM was (1<<5)
    SHOP_ITEM_FUNNEL_CAKE,        // for PEEP_ITEM was (1<<6)
    SHOP_ITEM_SUNGLASSES,         // for PEEP_ITEM was (1<<7)
    SHOP_ITEM_BEEF_NOODLES,       // for PEEP_ITEM was (1<<8)
    SHOP_ITEM_FRIED_RICE_NOODLES, // for PEEP_ITEM was (1<<9)
    SHOP_ITEM_WONTON_SOUP,        // for PEEP_ITEM was (1<<10)
    SHOP_ITEM_MEATBALL_SOUP,      // for PEEP_ITEM was (1<<11)
    SHOP_ITEM_FRUIT_JUICE,        // for PEEP_ITEM was (1<<12)
    SHOP_ITEM_SOYBEAN_MILK,       // for PEEP_ITEM was (1<<13)
    SHOP_ITEM_SUJEONGGWA,         // for PEEP_ITEM was (1<<14)
    SHOP_ITEM_SUB_SANDWICH,       // for PEEP_ITEM was (1<<15)
    SHOP_ITEM_COOKIE,             // for PEEP_ITEM was (1<<16)
    SHOP_ITEM_EMPTY_BOWL_RED,     // for PEEP_ITEM was (1<<17)
    SHOP_ITEM_EMPTY_DRINK_CARTON, // for PEEP_ITEM was (1<<18)
    SHOP_ITEM_EMPTY_JUICE_CUP,    // for PEEP_ITEM was (1<<19)
    SHOP_ITEM_ROAST_SAUSAGE,      // for PEEP_ITEM was (1<<20)
    SHOP_ITEM_EMPTY_BOWL_BLUE,    // for PEEP_ITEM was (1<<21)
    SHOP_ITEM_COUNT = 56,
    SHOP_ITEM_NONE = 255
};

ShopItem& operator++(ShopItem& d, int);

ShopItem operator+(const ShopItem& lhs, const ShopItem& rhs);

ShopItem operator-(const ShopItem& lhs, const ShopItem& rhs);

using ShopItemIndex = ShopItem;

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

extern const ShopItemDescriptor ShopItems[EnumValue(ShopItem::SHOP_ITEM_COUNT)];
extern uint64_t gSamePriceThroughoutPark;

money32 shop_item_get_common_price(Ride* forRide, const ShopItem shopItem);
bool shop_item_has_common_price(const ShopItem shopItem);
