/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "../localisation/StringIds.h"
#include "../sprites.h"
#include "ShopItem.h"

uint32_t gSamePriceThroughoutParkA;
uint32_t gSamePriceThroughoutParkB;

/** rct2: 0x00982164 */
static const rct_shop_item_stats ShopItemStats[SHOP_ITEM_COUNT] =
{
    {  3, 14, 14, 14 }, // SHOP_ITEM_BALLOON
    { 15, 30, 30, 30 }, // SHOP_ITEM_TOY
    {  1,  7,  7,  8 }, // SHOP_ITEM_MAP
    {  2, 30, 30, 30 }, // SHOP_ITEM_PHOTO
    { 20, 35, 25, 50 }, // SHOP_ITEM_UMBRELLA
    {  3, 12, 20, 10 }, // SHOP_ITEM_DRINK
    {  5, 19, 19, 22 }, // SHOP_ITEM_BURGER
    {  4, 16, 16, 18 }, // SHOP_ITEM_CHIPS
    {  4, 10, 15,  6 }, // SHOP_ITEM_ICE_CREAM
    {  3,  9,  9,  6 }, // SHOP_ITEM_CANDYFLOSS
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_CAN
    {  0,  0,  0,  0 }, // SHOP_ITEM_RUBBISH
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_BURGER_BOX
    {  6, 21, 21, 25 }, // SHOP_ITEM_PIZZA
    {  0,  0,  0,  0 }, // SHOP_ITEM_VOUCHER
    {  5, 13, 13, 11 }, // SHOP_ITEM_POPCORN
    {  5, 17, 17, 20 }, // SHOP_ITEM_HOT_DOG
    { 11, 22, 20, 18 }, // SHOP_ITEM_TENTACLE
    {  9, 27, 32, 24 }, // SHOP_ITEM_HAT
    {  4, 10, 10, 10 }, // SHOP_ITEM_TOFFEE_APPLE
    { 20, 37, 37, 37 }, // SHOP_ITEM_TSHIRT
    {  4,  8,  7, 10 }, // SHOP_ITEM_DOUGHNUT
    {  3, 11, 15, 20 }, // SHOP_ITEM_COFFEE
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_CUP
    {  5, 19, 19, 22 }, // SHOP_ITEM_CHICKEN
    {  4, 11, 21, 10 }, // SHOP_ITEM_LEMONADE
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_BOX
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_BOTTLE
    {  0,  0,  0,  0 }, // 28
    {  0,  0,  0,  0 }, // 29
    {  0,  0,  0,  0 }, // 30
    {  0,  0,  0,  0 }, // SHOP_ITEM_ADMISSION
    {  2, 30, 30, 30 }, // SHOP_ITEM_PHOTO2
    {  2, 30, 30, 30 }, // SHOP_ITEM_PHOTO3
    {  2, 30, 30, 30 }, // SHOP_ITEM_PHOTO4
    {  5, 11, 11, 11 }, // SHOP_ITEM_PRETZEL
    {  4, 13, 13, 20 }, // SHOP_ITEM_CHOCOLATE
    {  3, 10, 20, 10 }, // SHOP_ITEM_ICED_TEA
    {  5, 13, 11, 14 }, // SHOP_ITEM_FUNNEL_CAKE
    {  8, 15, 20, 12 }, // SHOP_ITEM_SUNGLASSES
    {  7, 17, 17, 20 }, // SHOP_ITEM_BEEF_NOODLES
    {  6, 17, 17, 20 }, // SHOP_ITEM_FRIED_RICE_NOODLES
    {  4, 13, 13, 15 }, // SHOP_ITEM_WONTON_SOUP
    {  5, 14, 14, 16 }, // SHOP_ITEM_MEATBALL_SOUP
    {  4, 11, 19, 11 }, // SHOP_ITEM_FRUIT_JUICE
    {  4, 10, 14, 10 }, // SHOP_ITEM_SOYBEAN_MILK
    {  3, 11, 14, 11 }, // SHOP_ITEM_SUJEONGGWA
    {  5, 19, 19, 17 }, // SHOP_ITEM_SUB_SANDWICH
    {  4,  8,  8,  8 }, // SHOP_ITEM_COOKIE
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_BOWL_RED
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_DRINK_CARTON
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_JUICE_CUP
    {  5, 16, 16, 20 }, // SHOP_ITEM_ROAST_SAUSAGE
    {  0,  0,  0,  0 }, // SHOP_ITEM_EMPTY_BOWL_BLUE
};

// rct2: 0x00982358
const money8 DefaultShopItemPrice[SHOP_ITEM_COUNT] =
{
    MONEY(0,90),                        // SHOP_ITEM_BALLOON
    MONEY(2,50),                        // SHOP_ITEM_TOY
    MONEY(0,60),                        // SHOP_ITEM_MAP
    MONEY(0,00),                        // SHOP_ITEM_PHOTO
    MONEY(2,50),                        // SHOP_ITEM_UMBRELLA
    MONEY(1,20),                        // SHOP_ITEM_DRINK
    MONEY(1,50),                        // SHOP_ITEM_BURGER
    MONEY(1,50),                        // SHOP_ITEM_CHIPS
    MONEY(0,90),                        // SHOP_ITEM_ICE_CREAM
    MONEY(0,80),                        // SHOP_ITEM_CANDYFLOSS
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_CAN
    MONEY(0,00),                        // SHOP_ITEM_RUBBISH
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_BURGER_BOX
    MONEY(1,60),                        // SHOP_ITEM_PIZZA
    MONEY(0,00),                        // SHOP_ITEM_VOUCHER
    MONEY(1,20),                        // SHOP_ITEM_POPCORN
    MONEY(1,00),                        // SHOP_ITEM_HOT_DOG
    MONEY(1,50),                        // SHOP_ITEM_TENTACLE
    MONEY(1,50),                        // SHOP_ITEM_HAT
    MONEY(0,70),                        // SHOP_ITEM_TOFFEE_APPLE
    MONEY(3,00),                        // SHOP_ITEM_TSHIRT
    MONEY(0,70),                        // SHOP_ITEM_DOUGHNUT
    MONEY(1,20),                        // SHOP_ITEM_COFFEE
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_CUP
    MONEY(1,50),                        // SHOP_ITEM_CHICKEN
    MONEY(1,20),                        // SHOP_ITEM_LEMONADE
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_BOX
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_BOTTLE
    MONEY(0,00),                        // 28
    MONEY(0,00),                        // 29
    MONEY(0,00),                        // 30
    MONEY(0,00),                        // 31
    MONEY(0,00),                        // SHOP_ITEM_PHOTO2
    MONEY(0,00),                        // SHOP_ITEM_PHOTO3
    MONEY(0,00),                        // SHOP_ITEM_PHOTO4
    MONEY(1,10),                        // SHOP_ITEM_PRETZEL
    MONEY(1,20),                        // SHOP_ITEM_CHOCOLATE
    MONEY(1,10),                        // SHOP_ITEM_ICED_TEA
    MONEY(1,20),                        // SHOP_ITEM_FUNNEL_CAKE
    MONEY(1,50),                        // SHOP_ITEM_SUNGLASSES
    MONEY(1,50),                        // SHOP_ITEM_BEEF_NOODLES
    MONEY(1,50),                        // SHOP_ITEM_FRIED_RICE_NOODLES
    MONEY(1,50),                        // SHOP_ITEM_WONTON_SOUP
    MONEY(1,50),                        // SHOP_ITEM_MEATBALL_SOUP
    MONEY(1,20),                        // SHOP_ITEM_FRUIT_JUICE
    MONEY(1,20),                        // SHOP_ITEM_SOYBEAN_MILK
    MONEY(1,20),                        // SHOP_ITEM_SUJEONGGWA
    MONEY(1,50),                        // SHOP_ITEM_SUB_SANDWICH
    MONEY(0,70),                        // SHOP_ITEM_COOKIE
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_BOWL_RED
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_DRINK_CARTON
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_JUICE_CUP
    MONEY(1,50),                        // SHOP_ITEM_ROAST_SAUSAGE
    MONEY(0,00),                        // SHOP_ITEM_EMPTY_BOWL_BLUE
    MONEY(0,00),                        // 54
    MONEY(0,00),                        // 55
};

const rct_shop_item_string_types ShopItemStringIds[SHOP_ITEM_COUNT] =
{
    { STR_SHOP_ITEM_PRICE_LABEL_BALLOON,                STR_SHOP_ITEM_SINGULAR_BALLOON,             STR_SHOP_ITEM_PLURAL_BALLOON,               STR_SHOP_ITEM_INDEFINITE_BALLOON,               STR_SHOP_ITEM_DISPLAY_BALLOON               },
    { STR_SHOP_ITEM_PRICE_LABEL_CUDDLY_TOY,             STR_SHOP_ITEM_SINGULAR_CUDDLY_TOY,          STR_SHOP_ITEM_PLURAL_CUDDLY_TOY,            STR_SHOP_ITEM_INDEFINITE_CUDDLY_TOY,            STR_SHOP_ITEM_DISPLAY_CUDDLY_TOY            },
    { STR_SHOP_ITEM_PRICE_LABEL_PARK_MAP,               STR_SHOP_ITEM_SINGULAR_PARK_MAP,            STR_SHOP_ITEM_PLURAL_PARK_MAP,              STR_SHOP_ITEM_INDEFINITE_PARK_MAP,              STR_SHOP_ITEM_DISPLAY_PARK_MAP              },
    { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO         },
    { STR_SHOP_ITEM_PRICE_LABEL_UMBRELLA,               STR_SHOP_ITEM_SINGULAR_UMBRELLA,            STR_SHOP_ITEM_PLURAL_UMBRELLA,              STR_SHOP_ITEM_INDEFINITE_UMBRELLA,              STR_SHOP_ITEM_DISPLAY_UMBRELLA              },
    { STR_SHOP_ITEM_PRICE_LABEL_DRINK,                  STR_SHOP_ITEM_SINGULAR_DRINK,               STR_SHOP_ITEM_PLURAL_DRINK,                 STR_SHOP_ITEM_INDEFINITE_DRINK,                 STR_SHOP_ITEM_DISPLAY_DRINK                 },
    { STR_SHOP_ITEM_PRICE_LABEL_BURGER,                 STR_SHOP_ITEM_SINGULAR_BURGER,              STR_SHOP_ITEM_PLURAL_BURGER,                STR_SHOP_ITEM_INDEFINITE_BURGER,                STR_SHOP_ITEM_DISPLAY_BURGER                },
    { STR_SHOP_ITEM_PRICE_LABEL_CHIPS,                  STR_SHOP_ITEM_SINGULAR_CHIPS,               STR_SHOP_ITEM_PLURAL_CHIPS,                 STR_SHOP_ITEM_INDEFINITE_CHIPS,                 STR_SHOP_ITEM_DISPLAY_CHIPS                 },
    { STR_SHOP_ITEM_PRICE_LABEL_ICE_CREAM,              STR_SHOP_ITEM_SINGULAR_ICE_CREAM,           STR_SHOP_ITEM_PLURAL_ICE_CREAM,             STR_SHOP_ITEM_INDEFINITE_ICE_CREAM,             STR_SHOP_ITEM_DISPLAY_ICE_CREAM             },
    { STR_SHOP_ITEM_PRICE_LABEL_CANDYFLOSS,             STR_SHOP_ITEM_SINGULAR_CANDYFLOSS,          STR_SHOP_ITEM_PLURAL_CANDYFLOSS,            STR_SHOP_ITEM_INDEFINITE_CANDYFLOSS,            STR_SHOP_ITEM_DISPLAY_CANDYFLOSS            },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CAN,              STR_SHOP_ITEM_SINGULAR_EMPTY_CAN,           STR_SHOP_ITEM_PLURAL_EMPTY_CAN,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CAN,             STR_SHOP_ITEM_DISPLAY_EMPTY_CAN             },
    { STR_SHOP_ITEM_PRICE_LABEL_RUBBISH,                STR_SHOP_ITEM_SINGULAR_RUBBISH,             STR_SHOP_ITEM_PLURAL_RUBBISH,               STR_SHOP_ITEM_INDEFINITE_RUBBISH,               STR_SHOP_ITEM_DISPLAY_RUBBISH               },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BURGER_BOX,       STR_SHOP_ITEM_SINGULAR_EMPTY_BURGER_BOX,    STR_SHOP_ITEM_PLURAL_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_INDEFINITE_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_DISPLAY_EMPTY_BURGER_BOX      },
    { STR_SHOP_ITEM_PRICE_LABEL_PIZZA,                  STR_SHOP_ITEM_SINGULAR_PIZZA,               STR_SHOP_ITEM_PLURAL_PIZZA,                 STR_SHOP_ITEM_INDEFINITE_PIZZA,                 STR_SHOP_ITEM_DISPLAY_PIZZA                 },
    { STR_SHOP_ITEM_PRICE_LABEL_VOUCHER,                STR_SHOP_ITEM_SINGULAR_VOUCHER,             STR_SHOP_ITEM_PLURAL_VOUCHER,               STR_SHOP_ITEM_INDEFINITE_VOUCHER,               STR_SHOP_ITEM_DISPLAY_VOUCHER               },
    { STR_SHOP_ITEM_PRICE_LABEL_POPCORN,                STR_SHOP_ITEM_SINGULAR_POPCORN,             STR_SHOP_ITEM_PLURAL_POPCORN,               STR_SHOP_ITEM_INDEFINITE_POPCORN,               STR_SHOP_ITEM_DISPLAY_POPCORN               },
    { STR_SHOP_ITEM_PRICE_LABEL_HOT_DOG,                STR_SHOP_ITEM_SINGULAR_HOT_DOG,             STR_SHOP_ITEM_PLURAL_HOT_DOG,               STR_SHOP_ITEM_INDEFINITE_HOT_DOG,               STR_SHOP_ITEM_DISPLAY_HOT_DOG               },
    { STR_SHOP_ITEM_PRICE_LABEL_TENTACLE,               STR_SHOP_ITEM_SINGULAR_TENTACLE,            STR_SHOP_ITEM_PLURAL_TENTACLE,              STR_SHOP_ITEM_INDEFINITE_TENTACLE,              STR_SHOP_ITEM_DISPLAY_TENTACLE              },
    { STR_SHOP_ITEM_PRICE_LABEL_HAT,                    STR_SHOP_ITEM_SINGULAR_HAT,                 STR_SHOP_ITEM_PLURAL_HAT,                   STR_SHOP_ITEM_INDEFINITE_HAT,                   STR_SHOP_ITEM_DISPLAY_HAT                   },
    { STR_SHOP_ITEM_PRICE_LABEL_TOFFEE_APPLE,           STR_SHOP_ITEM_SINGULAR_TOFFEE_APPLE,        STR_SHOP_ITEM_PLURAL_TOFFEE_APPLE,          STR_SHOP_ITEM_INDEFINITE_TOFFEE_APPLE,          STR_SHOP_ITEM_DISPLAY_TOFFEE_APPLE          },
    { STR_SHOP_ITEM_PRICE_LABEL_T_SHIRT,                STR_SHOP_ITEM_SINGULAR_T_SHIRT,             STR_SHOP_ITEM_PLURAL_T_SHIRT,               STR_SHOP_ITEM_INDEFINITE_T_SHIRT,               STR_SHOP_ITEM_DISPLAY_T_SHIRT               },
    { STR_SHOP_ITEM_PRICE_LABEL_DOUGHNUT,               STR_SHOP_ITEM_SINGULAR_DOUGHNUT,            STR_SHOP_ITEM_PLURAL_DOUGHNUT,              STR_SHOP_ITEM_INDEFINITE_DOUGHNUT,              STR_SHOP_ITEM_DISPLAY_DOUGHNUT              },
    { STR_SHOP_ITEM_PRICE_LABEL_COFFEE,                 STR_SHOP_ITEM_SINGULAR_COFFEE,              STR_SHOP_ITEM_PLURAL_COFFEE,                STR_SHOP_ITEM_INDEFINITE_COFFEE,                STR_SHOP_ITEM_DISPLAY_COFFEE                },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CUP,              STR_SHOP_ITEM_SINGULAR_EMPTY_CUP,           STR_SHOP_ITEM_PLURAL_EMPTY_CUP,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CUP,             STR_SHOP_ITEM_DISPLAY_EMPTY_CUP             },
    { STR_SHOP_ITEM_PRICE_LABEL_FRIED_CHICKEN,          STR_SHOP_ITEM_SINGULAR_FRIED_CHICKEN,       STR_SHOP_ITEM_PLURAL_FRIED_CHICKEN,         STR_SHOP_ITEM_INDEFINITE_FRIED_CHICKEN,         STR_SHOP_ITEM_DISPLAY_FRIED_CHICKEN         },
    { STR_SHOP_ITEM_PRICE_LABEL_LEMONADE,               STR_SHOP_ITEM_SINGULAR_LEMONADE,            STR_SHOP_ITEM_PLURAL_LEMONADE,              STR_SHOP_ITEM_INDEFINITE_LEMONADE,              STR_SHOP_ITEM_DISPLAY_LEMONADE              },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOX,              STR_SHOP_ITEM_SINGULAR_EMPTY_BOX,           STR_SHOP_ITEM_PLURAL_EMPTY_BOX,             STR_SHOP_ITEM_INDEFINITE_EMPTY_BOX,             STR_SHOP_ITEM_DISPLAY_EMPTY_BOX             },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOTTLE,           STR_SHOP_ITEM_SINGULAR_EMPTY_BOTTLE,        STR_SHOP_ITEM_PLURAL_EMPTY_BOTTLE,          STR_SHOP_ITEM_INDEFINITE_EMPTY_BOTTLE,          STR_SHOP_ITEM_DISPLAY_EMPTY_BOTTLE          },
    { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                    },
    { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                    },
    { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                    },
    { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                    },
    { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO         },
    { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO         },
    { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO         },
    { STR_SHOP_ITEM_PRICE_LABEL_PRETZEL,                STR_SHOP_ITEM_SINGULAR_PRETZEL,             STR_SHOP_ITEM_PLURAL_PRETZEL,               STR_SHOP_ITEM_INDEFINITE_PRETZEL,               STR_SHOP_ITEM_DISPLAY_PRETZEL               },
    { STR_SHOP_ITEM_PRICE_LABEL_HOT_CHOCOLATE,          STR_SHOP_ITEM_SINGULAR_HOT_CHOCOLATE,       STR_SHOP_ITEM_PLURAL_HOT_CHOCOLATE,         STR_SHOP_ITEM_INDEFINITE_HOT_CHOCOLATE,         STR_SHOP_ITEM_DISPLAY_HOT_CHOCOLATE         },
    { STR_SHOP_ITEM_PRICE_LABEL_ICED_TEA,               STR_SHOP_ITEM_SINGULAR_ICED_TEA,            STR_SHOP_ITEM_PLURAL_ICED_TEA,              STR_SHOP_ITEM_INDEFINITE_ICED_TEA,              STR_SHOP_ITEM_DISPLAY_ICED_TEA              },
    { STR_SHOP_ITEM_PRICE_LABEL_FUNNEL_CAKE,            STR_SHOP_ITEM_SINGULAR_FUNNEL_CAKE,         STR_SHOP_ITEM_PLURAL_FUNNEL_CAKE,           STR_SHOP_ITEM_INDEFINITE_FUNNEL_CAKE,           STR_SHOP_ITEM_DISPLAY_FUNNEL_CAKE           },
    { STR_SHOP_ITEM_PRICE_LABEL_SUNGLASSES,             STR_SHOP_ITEM_SINGULAR_SUNGLASSES,          STR_SHOP_ITEM_PLURAL_SUNGLASSES,            STR_SHOP_ITEM_INDEFINITE_SUNGLASSES,            STR_SHOP_ITEM_DISPLAY_SUNGLASSES            },
    { STR_SHOP_ITEM_PRICE_LABEL_BEEF_NOODLES,           STR_SHOP_ITEM_SINGULAR_BEEF_NOODLES,        STR_SHOP_ITEM_PLURAL_BEEF_NOODLES,          STR_SHOP_ITEM_INDEFINITE_BEEF_NOODLES,          STR_SHOP_ITEM_DISPLAY_BEEF_NOODLES          },
    { STR_SHOP_ITEM_PRICE_LABEL_FRIED_RICE_NOODLES,     STR_SHOP_ITEM_SINGULAR_FRIED_RICE_NOODLES,  STR_SHOP_ITEM_PLURAL_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_INDEFINITE_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_DISPLAY_FRIED_RICE_NOODLES    },
    { STR_SHOP_ITEM_PRICE_LABEL_WONTON_SOUP,            STR_SHOP_ITEM_SINGULAR_WONTON_SOUP,         STR_SHOP_ITEM_PLURAL_WONTON_SOUP,           STR_SHOP_ITEM_INDEFINITE_WONTON_SOUP,           STR_SHOP_ITEM_DISPLAY_WONTON_SOUP           },
    { STR_SHOP_ITEM_PRICE_LABEL_MEATBALL_SOUP,          STR_SHOP_ITEM_SINGULAR_MEATBALL_SOUP,       STR_SHOP_ITEM_PLURAL_MEATBALL_SOUP,         STR_SHOP_ITEM_INDEFINITE_MEATBALL_SOUP,         STR_SHOP_ITEM_DISPLAY_MEATBALL_SOUP         },
    { STR_SHOP_ITEM_PRICE_LABEL_FRUIT_JUICE,            STR_SHOP_ITEM_SINGULAR_FRUIT_JUICE,         STR_SHOP_ITEM_PLURAL_FRUIT_JUICE,           STR_SHOP_ITEM_INDEFINITE_FRUIT_JUICE,           STR_SHOP_ITEM_DISPLAY_FRUIT_JUICE           },
    { STR_SHOP_ITEM_PRICE_LABEL_SOYBEAN_MILK,           STR_SHOP_ITEM_SINGULAR_SOYBEAN_MILK,        STR_SHOP_ITEM_PLURAL_SOYBEAN_MILK,          STR_SHOP_ITEM_INDEFINITE_SOYBEAN_MILK,          STR_SHOP_ITEM_DISPLAY_SOYBEAN_MILK          },
    { STR_SHOP_ITEM_PRICE_LABEL_SUJONGKWA,              STR_SHOP_ITEM_SINGULAR_SUJONGKWA,           STR_SHOP_ITEM_PLURAL_SUJONGKWA,             STR_SHOP_ITEM_INDEFINITE_SUJONGKWA,             STR_SHOP_ITEM_DISPLAY_SUJONGKWA             },
    { STR_SHOP_ITEM_PRICE_LABEL_SUB_SANDWICH,           STR_SHOP_ITEM_SINGULAR_SUB_SANDWICH,        STR_SHOP_ITEM_PLURAL_SUB_SANDWICH,          STR_SHOP_ITEM_INDEFINITE_SUB_SANDWICH,          STR_SHOP_ITEM_DISPLAY_SUB_SANDWICH          },
    { STR_SHOP_ITEM_PRICE_LABEL_COOKIE,                 STR_SHOP_ITEM_SINGULAR_COOKIE,              STR_SHOP_ITEM_PLURAL_COOKIE,                STR_SHOP_ITEM_INDEFINITE_COOKIE,                STR_SHOP_ITEM_DISPLAY_COOKIE                },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_RED,         STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_RED,      STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_RED,        STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_RED,        STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_RED        },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_DRINK_CARTON,     STR_SHOP_ITEM_SINGULAR_EMPTY_DRINK_CARTON,  STR_SHOP_ITEM_PLURAL_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_INDEFINITE_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_DISPLAY_EMPTY_DRINK_CARTON    },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_JUICE_CUP,        STR_SHOP_ITEM_SINGULAR_EMPTY_JUICE_CUP,     STR_SHOP_ITEM_PLURAL_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_INDEFINITE_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_DISPLAY_EMPTY_JUICE_CUP       },
    { STR_SHOP_ITEM_PRICE_LABEL_ROAST_SAUSAGE,          STR_SHOP_ITEM_SINGULAR_ROAST_SAUSAGE,       STR_SHOP_ITEM_PLURAL_ROAST_SAUSAGE,         STR_SHOP_ITEM_INDEFINITE_ROAST_SAUSAGE,         STR_SHOP_ITEM_DISPLAY_ROAST_SAUSAGE         },
    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_BLUE,        STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_BLUE,     STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_BLUE       },
};

const uint32_t ShopItemImage[SHOP_ITEM_COUNT] =
{
    SPR_SHOP_ITEM_BALLOON,
    SPR_SHOP_ITEM_TOY,
    SPR_SHOP_ITEM_MAP,
    SPR_SHOP_ITEM_PHOTO,
    SPR_SHOP_ITEM_UMBRELLA,
    SPR_SHOP_ITEM_DRINK,
    SPR_SHOP_ITEM_BURGER,
    SPR_SHOP_ITEM_CHIPS,
    SPR_SHOP_ITEM_ICE_CREAM,
    SPR_SHOP_ITEM_CANDYFLOSS,
    SPR_SHOP_ITEM_EMPTY_CAN,
    SPR_SHOP_ITEM_RUBBISH,
    SPR_SHOP_ITEM_EMPTY_BURGER_BOX,
    SPR_SHOP_ITEM_PIZZA,
    SPR_SHOP_ITEM_VOUCHER,
    SPR_SHOP_ITEM_POPCORN,
    SPR_SHOP_ITEM_HOT_DOG,
    SPR_SHOP_ITEM_TENTACLE,
    SPR_SHOP_ITEM_HAT,
    SPR_SHOP_ITEM_TOFFEE_APPLE,
    SPR_SHOP_ITEM_TSHIRT,
    SPR_SHOP_ITEM_DOUGHNUT,
    SPR_SHOP_ITEM_COFFEE,
    SPR_SHOP_ITEM_EMPTY_CUP,
    SPR_SHOP_ITEM_CHICKEN,
    SPR_SHOP_ITEM_LEMONADE,
    SPR_SHOP_ITEM_EMPTY_BOX,
    SPR_SHOP_ITEM_EMPTY_BOTTLE,
    0,                                      // 28
    0,                                      // 29
    0,                                      // 30
    0,                                      // 31
    SPR_SHOP_ITEM_PHOTO2,
    SPR_SHOP_ITEM_PHOTO3,
    SPR_SHOP_ITEM_PHOTO4,
    SPR_SHOP_ITEM_PRETZEL,
    SPR_SHOP_ITEM_CHOCOLATE,
    SPR_SHOP_ITEM_ICED_TEA,
    SPR_SHOP_ITEM_FUNNEL_CAKE,
    SPR_SHOP_ITEM_SUNGLASSES,
    SPR_SHOP_ITEM_BEEF_NOODLES,
    SPR_SHOP_ITEM_FRIED_RICE_NOODLES,
    SPR_SHOP_ITEM_WONTON_SOUP,
    SPR_SHOP_ITEM_MEATBALL_SOUP,
    SPR_SHOP_ITEM_FRUIT_JUICE,
    SPR_SHOP_ITEM_SOYBEAN_MILK,
    SPR_SHOP_ITEM_SUJEONGGWA,
    SPR_SHOP_ITEM_SUB_SANDWICH,
    SPR_SHOP_ITEM_COOKIE,
    SPR_SHOP_ITEM_EMPTY_BOWL_RED,
    SPR_SHOP_ITEM_EMPTY_DRINK_CARTON,
    SPR_SHOP_ITEM_EMPTY_JUICE_CUP,
    SPR_SHOP_ITEM_ROAST_SAUSAGE,
    SPR_SHOP_ITEM_EMPTY_BOWL_BLUE,
};

money32 get_shop_item_cost(int32_t shopItem)
{
    return ShopItemStats[shopItem].cost;
}

money16 get_shop_base_value(int32_t shopItem)
{
    return ShopItemStats[shopItem].base_value;
}

money16 get_shop_cold_value(int32_t shopItem)
{
    return ShopItemStats[shopItem].cold_value;
}

money16 get_shop_hot_value(int32_t shopItem)
{
    return ShopItemStats[shopItem].hot_value;
}

money32 shop_item_get_common_price(Ride* forRide, int32_t shopItem)
{
    rct_ride_entry* rideEntry;
    Ride* ride;
    int32_t i;

    FOR_ALL_RIDES(i, ride)
    {
        if (ride != forRide)
        {
            rideEntry = get_ride_entry(ride->subtype);
            if (rideEntry == nullptr)
            {
                continue;
            }
            if (rideEntry->shop_item == shopItem)
            {
                return ride->price;
            }
            if (rideEntry->shop_item_secondary == shopItem)
            {
                return ride->price_secondary;
            }
            if (shop_item_is_photo(shopItem) && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            {
                return ride->price_secondary;
            }
        }
    }

    return MONEY32_UNDEFINED;
}

bool shop_item_is_photo(int32_t shopItem)
{
    return (
        shopItem == SHOP_ITEM_PHOTO || shopItem == SHOP_ITEM_PHOTO2 ||
        shopItem == SHOP_ITEM_PHOTO3 || shopItem == SHOP_ITEM_PHOTO4);
}

bool shop_item_has_common_price(int32_t shopItem)
{
    if (shopItem < 32)
    {
        return (gSamePriceThroughoutParkA & (1u << shopItem)) != 0;
    }
    else
    {
        return (gSamePriceThroughoutParkB & (1u << (shopItem - 32))) != 0;
    }
}

bool shop_item_is_food_or_drink(int32_t shopItem)
{
    switch (shopItem)
    {
        case SHOP_ITEM_DRINK:
        case SHOP_ITEM_BURGER:
        case SHOP_ITEM_CHIPS:
        case SHOP_ITEM_ICE_CREAM:
        case SHOP_ITEM_CANDYFLOSS:
        case SHOP_ITEM_PIZZA:
        case SHOP_ITEM_POPCORN:
        case SHOP_ITEM_HOT_DOG:
        case SHOP_ITEM_TENTACLE:
        case SHOP_ITEM_TOFFEE_APPLE:
        case SHOP_ITEM_DOUGHNUT:
        case SHOP_ITEM_COFFEE:
        case SHOP_ITEM_CHICKEN:
        case SHOP_ITEM_LEMONADE:
        case SHOP_ITEM_PRETZEL:
        case SHOP_ITEM_CHOCOLATE:
        case SHOP_ITEM_ICED_TEA:
        case SHOP_ITEM_FUNNEL_CAKE:
        case SHOP_ITEM_BEEF_NOODLES:
        case SHOP_ITEM_FRIED_RICE_NOODLES:
        case SHOP_ITEM_WONTON_SOUP:
        case SHOP_ITEM_MEATBALL_SOUP:
        case SHOP_ITEM_FRUIT_JUICE:
        case SHOP_ITEM_SOYBEAN_MILK:
        case SHOP_ITEM_SUJEONGGWA:
        case SHOP_ITEM_SUB_SANDWICH:
        case SHOP_ITEM_COOKIE:
        case SHOP_ITEM_ROAST_SAUSAGE:
            return true;
        default:
            return false;
    }
}

bool shop_item_is_food(int32_t shopItem)
{
    switch (shopItem)
    {
        case SHOP_ITEM_BURGER:
        case SHOP_ITEM_CHIPS:
        case SHOP_ITEM_ICE_CREAM:
        case SHOP_ITEM_CANDYFLOSS:
        case SHOP_ITEM_PIZZA:
        case SHOP_ITEM_POPCORN:
        case SHOP_ITEM_HOT_DOG:
        case SHOP_ITEM_TENTACLE:
        case SHOP_ITEM_TOFFEE_APPLE:
        case SHOP_ITEM_DOUGHNUT:
        case SHOP_ITEM_CHICKEN:
        case SHOP_ITEM_PRETZEL:
        case SHOP_ITEM_FUNNEL_CAKE:
        case SHOP_ITEM_BEEF_NOODLES:
        case SHOP_ITEM_FRIED_RICE_NOODLES:
        case SHOP_ITEM_WONTON_SOUP:
        case SHOP_ITEM_MEATBALL_SOUP:
        case SHOP_ITEM_SUB_SANDWICH:
        case SHOP_ITEM_COOKIE:
        case SHOP_ITEM_ROAST_SAUSAGE:
            return true;
        default:
            return false;
    }
}

bool shop_item_is_drink(int32_t shopItem)
{
    switch (shopItem)
    {
        case SHOP_ITEM_DRINK:
        case SHOP_ITEM_COFFEE:
        case SHOP_ITEM_LEMONADE:
        case SHOP_ITEM_CHOCOLATE:
        case SHOP_ITEM_ICED_TEA:
        case SHOP_ITEM_FRUIT_JUICE:
        case SHOP_ITEM_SOYBEAN_MILK:
        case SHOP_ITEM_SUJEONGGWA:
            return true;
        default:
            return false;
    }
}

bool shop_item_is_souvenir(int32_t shopItem)
{
    switch (shopItem)
    {
        case SHOP_ITEM_BALLOON:
        case SHOP_ITEM_TOY:
        case SHOP_ITEM_MAP:
        case SHOP_ITEM_PHOTO:
        case SHOP_ITEM_UMBRELLA:
        case SHOP_ITEM_HAT:
        case SHOP_ITEM_TSHIRT:
        case SHOP_ITEM_PHOTO2:
        case SHOP_ITEM_PHOTO3:
        case SHOP_ITEM_PHOTO4:
        case SHOP_ITEM_SUNGLASSES:
            return true;
        default:
            return false;
    }
}
