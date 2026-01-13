/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../entity/Litter.h"

struct Ride;

enum class PeepThoughtType : uint8_t;

enum class ShopItem : uint8_t
{
    balloon,
    toy,
    map,
    photo,
    umbrella,
    drink,
    burger,
    chips,
    iceCream,
    candyfloss,
    emptyCan,
    rubbish,
    emptyBurgerBox,
    pizza,
    voucher,
    popcorn,
    hotDog,
    tentacle,
    hat,
    toffeeApple,
    tShirt,
    doughnut,
    coffee,
    emptyCup,
    chicken,
    lemonade,
    emptyBox,
    emptyBottle = 27,
    admission = 31,
    photo2 = 32,
    photo3,
    photo4,
    pretzel,
    chocolate,
    icedTea,
    funnelCake,
    sunglasses,
    beefNoodles,
    friedRiceNoodles,
    wontonSoup,
    meatballSoup,
    fruitJuice,
    soybeanMilk,
    sujeonggwa,
    subSandwich,
    cookie,
    emptyBowlRed,
    emptyDrinkCarton,
    emptyJuiceCup,
    roastSausage,
    emptyBowlBlue,
    count = 56,
    none = 255
};

ShopItem& operator++(ShopItem& d, int);

using ShopItemIndex = ShopItem;

struct ShopItemStrings
{
    StringId PriceLabel; // Balloon price:
    StringId Singular;   // Balloon
    StringId Plural;     // Balloons
    StringId Indefinite; // a Balloon
    StringId Display;    // "Diamond Heights" Balloon
};

struct ShopItemDescriptor
{
    money64 Cost;
    money64 BaseValue;
    money64 HotValue;
    money64 ColdValue;
    money64 DefaultPrice;
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

money64 ShopItemGetCommonPrice(Ride* forRide, ShopItem shopItem);
bool ShopItemHasCommonPrice(ShopItem shopItem);

const ShopItemDescriptor& GetShopItemDescriptor(ShopItem item);
