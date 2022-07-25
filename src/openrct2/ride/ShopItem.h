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
#include "../entity/Litter.h"
#include "../util/Util.h"

struct Ride;
enum class PeepThoughtType : uint8_t;

enum class ShopItem : uint8_t
{
    Balloon,
    Toy,
    Map,
    Photo,
    Umbrella,
    Drink,
    Burger,
    Chips,
    IceCream,
    Candyfloss,
    EmptyCan,
    Rubbish,
    EmptyBurgerBox,
    Pizza,
    Voucher,
    Popcorn,
    HotDog,
    Tentacle,
    Hat,
    ToffeeApple,
    TShirt,
    Doughnut,
    Coffee,
    EmptyCup,
    Chicken,
    Lemonade,
    EmptyBox,
    EmptyBottle = 27,
    Admission = 31,
    Photo2 = 32,
    Photo3,
    Photo4,
    Pretzel,
    Chocolate,
    IcedTea,
    FunnelCake,
    Sunglasses,
    BeefNoodles,
    FriedRiceNoodles,
    WontonSoup,
    MeatballSoup,
    FruitJuice,
    SoybeanMilk,
    Sujeonggwa,
    SubSandwich,
    Cookie,
    EmptyBowlRed,
    EmptyDrinkCarton,
    EmptyJuiceCup,
    RoastSausage,
    EmptyBowlBlue,
    Count = 56,
    None = 255
};

ShopItem& operator++(ShopItem& d, int);

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
    money16 Cost;
    money16 BaseValue;
    money16 HotValue;
    money16 ColdValue;
    money8 DefaultPrice;
    uint32_t Image;
    ShopItemStrings Naming;
    uint16_t Flags;
    Litter::Type Type;
    uint8_t ConsumptionTime;
    ShopItem DiscardContainer;
    PeepThoughtType TooMuchThought;
    PeepThoughtType GoodValueThought;

    constexpr bool HasFlag(const uint16_t flag) const
    {
        return (Flags & flag) != 0;
    }
    bool IsFood() const;
    bool IsDrink() const;
    bool IsFoodOrDrink() const;
    bool IsSouvenir() const;
    bool IsPhoto() const;
    bool IsRecolourable() const;
};

uint64_t ShopItemsGetAllFoods();
uint64_t ShopItemsGetAllDrinks();
uint64_t ShopItemsGetAllContainers();

enum
{
    SHOP_ITEM_FLAG_IS_FOOD = (1 << 0),
    SHOP_ITEM_FLAG_IS_DRINK = (1 << 1),
    SHOP_ITEM_FLAG_IS_SOUVENIR = (1 << 2),
    SHOP_ITEM_FLAG_IS_PHOTO = (1 << 3),
    SHOP_ITEM_FLAG_IS_CONTAINER = (1 << 4),
    SHOP_ITEM_FLAG_IS_RECOLOURABLE = (1 << 5),
};

extern uint64_t gSamePriceThroughoutPark;

money32 shop_item_get_common_price(Ride* forRide, const ShopItem shopItem);
bool shop_item_has_common_price(const ShopItem shopItem);

const ShopItemDescriptor& GetShopItemDescriptor(ShopItem item);
