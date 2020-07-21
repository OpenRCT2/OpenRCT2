/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShopItem.h"

#include "../common.h"
#include "../localisation/StringIds.h"
#include "../sprites.h"

uint64_t gSamePriceThroughoutPark;

// clang-format off
/** rct2: 0x00982164 (cost, base value, hot and cold value); 0x00982358 (default price) */
const ShopItemDescriptor ShopItems[SHOP_ITEM_COUNT] = {
    // Item,                              Cost, Base value, Hot value, Cold value, Default price, Image,                               Price label,                                      Singular,                                   Plural,                                     Indefinite,                                     Display (in guest inventory)
    /* SHOP_ITEM_BALLOON */            {  3,    14,         14,        14,         MONEY(0, 90),  SPR_SHOP_ITEM_BALLOON,             { STR_SHOP_ITEM_PRICE_LABEL_BALLOON,                STR_SHOP_ITEM_SINGULAR_BALLOON,             STR_SHOP_ITEM_PLURAL_BALLOON,               STR_SHOP_ITEM_INDEFINITE_BALLOON,               STR_SHOP_ITEM_DISPLAY_BALLOON             }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_TOY */                {  15,   30,         30,        30,         MONEY(2, 50),  SPR_SHOP_ITEM_TOY,                 { STR_SHOP_ITEM_PRICE_LABEL_CUDDLY_TOY,             STR_SHOP_ITEM_SINGULAR_CUDDLY_TOY,          STR_SHOP_ITEM_PLURAL_CUDDLY_TOY,            STR_SHOP_ITEM_INDEFINITE_CUDDLY_TOY,            STR_SHOP_ITEM_DISPLAY_CUDDLY_TOY          }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_MAP */                {  1,    7,          7,         8,          MONEY(0, 60),  SPR_SHOP_ITEM_MAP,                 { STR_SHOP_ITEM_PRICE_LABEL_PARK_MAP,               STR_SHOP_ITEM_SINGULAR_PARK_MAP,            STR_SHOP_ITEM_PLURAL_PARK_MAP,              STR_SHOP_ITEM_INDEFINITE_PARK_MAP,              STR_SHOP_ITEM_DISPLAY_PARK_MAP            }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_PHOTO */              {  2,    30,         30,        30,         MONEY(0, 00),  SPR_SHOP_ITEM_PHOTO,               { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR },
    /* SHOP_ITEM_UMBRELLA */           {  20,   35,         25,        50,         MONEY(2, 50),  SPR_SHOP_ITEM_UMBRELLA,            { STR_SHOP_ITEM_PRICE_LABEL_UMBRELLA,               STR_SHOP_ITEM_SINGULAR_UMBRELLA,            STR_SHOP_ITEM_PLURAL_UMBRELLA,              STR_SHOP_ITEM_INDEFINITE_UMBRELLA,              STR_SHOP_ITEM_DISPLAY_UMBRELLA            }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_DRINK */              {  3,    12,         20,        10,         MONEY(1, 20),  SPR_SHOP_ITEM_DRINK,               { STR_SHOP_ITEM_PRICE_LABEL_DRINK,                  STR_SHOP_ITEM_SINGULAR_DRINK,               STR_SHOP_ITEM_PLURAL_DRINK,                 STR_SHOP_ITEM_INDEFINITE_DRINK,                 STR_SHOP_ITEM_DISPLAY_DRINK               }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_BURGER */             {  5,    19,         19,        22,         MONEY(1, 50),  SPR_SHOP_ITEM_BURGER,              { STR_SHOP_ITEM_PRICE_LABEL_BURGER,                 STR_SHOP_ITEM_SINGULAR_BURGER,              STR_SHOP_ITEM_PLURAL_BURGER,                STR_SHOP_ITEM_INDEFINITE_BURGER,                STR_SHOP_ITEM_DISPLAY_BURGER              }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_CHIPS */              {  4,    16,         16,        18,         MONEY(1, 50),  SPR_SHOP_ITEM_CHIPS,               { STR_SHOP_ITEM_PRICE_LABEL_CHIPS,                  STR_SHOP_ITEM_SINGULAR_CHIPS,               STR_SHOP_ITEM_PLURAL_CHIPS,                 STR_SHOP_ITEM_INDEFINITE_CHIPS,                 STR_SHOP_ITEM_DISPLAY_CHIPS               }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_ICE_CREAM */          {  4,    10,         15,        6,          MONEY(0, 90),  SPR_SHOP_ITEM_ICE_CREAM,           { STR_SHOP_ITEM_PRICE_LABEL_ICE_CREAM,              STR_SHOP_ITEM_SINGULAR_ICE_CREAM,           STR_SHOP_ITEM_PLURAL_ICE_CREAM,             STR_SHOP_ITEM_INDEFINITE_ICE_CREAM,             STR_SHOP_ITEM_DISPLAY_ICE_CREAM           }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_CANDYFLOSS */         {  3,    9,          9,         6,          MONEY(0, 80),  SPR_SHOP_ITEM_CANDYFLOSS,          { STR_SHOP_ITEM_PRICE_LABEL_CANDYFLOSS,             STR_SHOP_ITEM_SINGULAR_CANDYFLOSS,          STR_SHOP_ITEM_PLURAL_CANDYFLOSS,            STR_SHOP_ITEM_INDEFINITE_CANDYFLOSS,            STR_SHOP_ITEM_DISPLAY_CANDYFLOSS          }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_EMPTY_CAN */          {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_CAN,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CAN,              STR_SHOP_ITEM_SINGULAR_EMPTY_CAN,           STR_SHOP_ITEM_PLURAL_EMPTY_CAN,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CAN,             STR_SHOP_ITEM_DISPLAY_EMPTY_CAN           }, 0                                                    },
    /* SHOP_ITEM_RUBBISH */            {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_RUBBISH,             { STR_SHOP_ITEM_PRICE_LABEL_RUBBISH,                STR_SHOP_ITEM_SINGULAR_RUBBISH,             STR_SHOP_ITEM_PLURAL_RUBBISH,               STR_SHOP_ITEM_INDEFINITE_RUBBISH,               STR_SHOP_ITEM_DISPLAY_RUBBISH             }, 0                                                    },
    /* SHOP_ITEM_EMPTY_BURGER_BOX */   {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_BURGER_BOX,    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BURGER_BOX,       STR_SHOP_ITEM_SINGULAR_EMPTY_BURGER_BOX,    STR_SHOP_ITEM_PLURAL_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_INDEFINITE_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_DISPLAY_EMPTY_BURGER_BOX    }, 0                                                    },
    /* SHOP_ITEM_PIZZA */              {  6,    21,         21,        25,         MONEY(1, 60),  SPR_SHOP_ITEM_PIZZA,               { STR_SHOP_ITEM_PRICE_LABEL_PIZZA,                  STR_SHOP_ITEM_SINGULAR_PIZZA,               STR_SHOP_ITEM_PLURAL_PIZZA,                 STR_SHOP_ITEM_INDEFINITE_PIZZA,                 STR_SHOP_ITEM_DISPLAY_PIZZA               }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_VOUCHER */            {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_VOUCHER,             { STR_SHOP_ITEM_PRICE_LABEL_VOUCHER,                STR_SHOP_ITEM_SINGULAR_VOUCHER,             STR_SHOP_ITEM_PLURAL_VOUCHER,               STR_SHOP_ITEM_INDEFINITE_VOUCHER,               STR_SHOP_ITEM_DISPLAY_VOUCHER             }, 0                                                    },
    /* SHOP_ITEM_POPCORN */            {  5,    13,         13,        11,         MONEY(1, 20),  SPR_SHOP_ITEM_POPCORN,             { STR_SHOP_ITEM_PRICE_LABEL_POPCORN,                STR_SHOP_ITEM_SINGULAR_POPCORN,             STR_SHOP_ITEM_PLURAL_POPCORN,               STR_SHOP_ITEM_INDEFINITE_POPCORN,               STR_SHOP_ITEM_DISPLAY_POPCORN             }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_HOT_DOG */            {  5,    17,         17,        20,         MONEY(1, 00),  SPR_SHOP_ITEM_HOT_DOG,             { STR_SHOP_ITEM_PRICE_LABEL_HOT_DOG,                STR_SHOP_ITEM_SINGULAR_HOT_DOG,             STR_SHOP_ITEM_PLURAL_HOT_DOG,               STR_SHOP_ITEM_INDEFINITE_HOT_DOG,               STR_SHOP_ITEM_DISPLAY_HOT_DOG             }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_TENTACLE */           {  11,   22,         20,        18,         MONEY(1, 50),  SPR_SHOP_ITEM_TENTACLE,            { STR_SHOP_ITEM_PRICE_LABEL_TENTACLE,               STR_SHOP_ITEM_SINGULAR_TENTACLE,            STR_SHOP_ITEM_PLURAL_TENTACLE,              STR_SHOP_ITEM_INDEFINITE_TENTACLE,              STR_SHOP_ITEM_DISPLAY_TENTACLE            }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_HAT */                {  9,    27,         32,        24,         MONEY(1, 50),  SPR_SHOP_ITEM_HAT,                 { STR_SHOP_ITEM_PRICE_LABEL_HAT,                    STR_SHOP_ITEM_SINGULAR_HAT,                 STR_SHOP_ITEM_PLURAL_HAT,                   STR_SHOP_ITEM_INDEFINITE_HAT,                   STR_SHOP_ITEM_DISPLAY_HAT                 }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_TOFFEE_APPLE */       {  4,    10,         10,        10,         MONEY(0, 70),  SPR_SHOP_ITEM_TOFFEE_APPLE,        { STR_SHOP_ITEM_PRICE_LABEL_TOFFEE_APPLE,           STR_SHOP_ITEM_SINGULAR_TOFFEE_APPLE,        STR_SHOP_ITEM_PLURAL_TOFFEE_APPLE,          STR_SHOP_ITEM_INDEFINITE_TOFFEE_APPLE,          STR_SHOP_ITEM_DISPLAY_TOFFEE_APPLE        }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_TSHIRT */             {  20,   37,         37,        37,         MONEY(3, 00),  SPR_SHOP_ITEM_TSHIRT,              { STR_SHOP_ITEM_PRICE_LABEL_T_SHIRT,                STR_SHOP_ITEM_SINGULAR_T_SHIRT,             STR_SHOP_ITEM_PLURAL_T_SHIRT,               STR_SHOP_ITEM_INDEFINITE_T_SHIRT,               STR_SHOP_ITEM_DISPLAY_T_SHIRT             }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_DOUGHNUT */           {  4,    8,          7,         10,         MONEY(0, 70),  SPR_SHOP_ITEM_DOUGHNUT,            { STR_SHOP_ITEM_PRICE_LABEL_DOUGHNUT,               STR_SHOP_ITEM_SINGULAR_DOUGHNUT,            STR_SHOP_ITEM_PLURAL_DOUGHNUT,              STR_SHOP_ITEM_INDEFINITE_DOUGHNUT,              STR_SHOP_ITEM_DISPLAY_DOUGHNUT            }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_COFFEE */             {  3,    11,         15,        20,         MONEY(1, 20),  SPR_SHOP_ITEM_COFFEE,              { STR_SHOP_ITEM_PRICE_LABEL_COFFEE,                 STR_SHOP_ITEM_SINGULAR_COFFEE,              STR_SHOP_ITEM_PLURAL_COFFEE,                STR_SHOP_ITEM_INDEFINITE_COFFEE,                STR_SHOP_ITEM_DISPLAY_COFFEE              }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_EMPTY_CUP */          {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_CUP,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CUP,              STR_SHOP_ITEM_SINGULAR_EMPTY_CUP,           STR_SHOP_ITEM_PLURAL_EMPTY_CUP,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CUP,             STR_SHOP_ITEM_DISPLAY_EMPTY_CUP           }, 0                                                    },
    /* SHOP_ITEM_CHICKEN */            {  5,    19,         19,        22,         MONEY(1, 50),  SPR_SHOP_ITEM_CHICKEN,             { STR_SHOP_ITEM_PRICE_LABEL_FRIED_CHICKEN,          STR_SHOP_ITEM_SINGULAR_FRIED_CHICKEN,       STR_SHOP_ITEM_PLURAL_FRIED_CHICKEN,         STR_SHOP_ITEM_INDEFINITE_FRIED_CHICKEN,         STR_SHOP_ITEM_DISPLAY_FRIED_CHICKEN       }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_LEMONADE */           {  4,    11,         21,        10,         MONEY(1, 20),  SPR_SHOP_ITEM_LEMONADE,            { STR_SHOP_ITEM_PRICE_LABEL_LEMONADE,               STR_SHOP_ITEM_SINGULAR_LEMONADE,            STR_SHOP_ITEM_PLURAL_LEMONADE,              STR_SHOP_ITEM_INDEFINITE_LEMONADE,              STR_SHOP_ITEM_DISPLAY_LEMONADE            }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_EMPTY_BOX */          {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_BOX,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOX,              STR_SHOP_ITEM_SINGULAR_EMPTY_BOX,           STR_SHOP_ITEM_PLURAL_EMPTY_BOX,             STR_SHOP_ITEM_INDEFINITE_EMPTY_BOX,             STR_SHOP_ITEM_DISPLAY_EMPTY_BOX           }, 0                                                    },
    /* SHOP_ITEM_EMPTY_BOTTLE */       {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_BOTTLE,        { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOTTLE,           STR_SHOP_ITEM_SINGULAR_EMPTY_BOTTLE,        STR_SHOP_ITEM_PLURAL_EMPTY_BOTTLE,          STR_SHOP_ITEM_INDEFINITE_EMPTY_BOTTLE,          STR_SHOP_ITEM_DISPLAY_EMPTY_BOTTLE        }, 0                                                    },
    /* 28 */                           {  0,    0,          0,         0,          MONEY(0, 00),  0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0                                                    },
    /* 29 */                           {  0,    0,          0,         0,          MONEY(0, 00),  0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0                                                    },
    /* 30 */                           {  0,    0,          0,         0,          MONEY(0, 00),  0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0                                                    },
    /* SHOP_ITEM_ADMISSION */          {  0,    0,          0,         0,          MONEY(0, 00),  0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0                                                    },
    /* SHOP_ITEM_PHOTO2 */             {  2,    30,         30,        30,         MONEY(0, 00),  SPR_SHOP_ITEM_PHOTO2,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR },
    /* SHOP_ITEM_PHOTO3 */             {  2,    30,         30,        30,         MONEY(0, 00),  SPR_SHOP_ITEM_PHOTO3,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR },
    /* SHOP_ITEM_PHOTO4 */             {  2,    30,         30,        30,         MONEY(0, 00),  SPR_SHOP_ITEM_PHOTO4,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR },
    /* SHOP_ITEM_PRETZEL */            {  5,    11,         11,        11,         MONEY(1, 10),  SPR_SHOP_ITEM_PRETZEL,             { STR_SHOP_ITEM_PRICE_LABEL_PRETZEL,                STR_SHOP_ITEM_SINGULAR_PRETZEL,             STR_SHOP_ITEM_PLURAL_PRETZEL,               STR_SHOP_ITEM_INDEFINITE_PRETZEL,               STR_SHOP_ITEM_DISPLAY_PRETZEL             }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_CHOCOLATE */          {  4,    13,         13,        20,         MONEY(1, 20),  SPR_SHOP_ITEM_CHOCOLATE,           { STR_SHOP_ITEM_PRICE_LABEL_HOT_CHOCOLATE,          STR_SHOP_ITEM_SINGULAR_HOT_CHOCOLATE,       STR_SHOP_ITEM_PLURAL_HOT_CHOCOLATE,         STR_SHOP_ITEM_INDEFINITE_HOT_CHOCOLATE,         STR_SHOP_ITEM_DISPLAY_HOT_CHOCOLATE       }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_ICED_TEA */           {  3,    10,         20,        10,         MONEY(1, 10),  SPR_SHOP_ITEM_ICED_TEA,            { STR_SHOP_ITEM_PRICE_LABEL_ICED_TEA,               STR_SHOP_ITEM_SINGULAR_ICED_TEA,            STR_SHOP_ITEM_PLURAL_ICED_TEA,              STR_SHOP_ITEM_INDEFINITE_ICED_TEA,              STR_SHOP_ITEM_DISPLAY_ICED_TEA            }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_FUNNEL_CAKE */        {  5,    13,         11,        14,         MONEY(1, 20),  SPR_SHOP_ITEM_FUNNEL_CAKE,         { STR_SHOP_ITEM_PRICE_LABEL_FUNNEL_CAKE,            STR_SHOP_ITEM_SINGULAR_FUNNEL_CAKE,         STR_SHOP_ITEM_PLURAL_FUNNEL_CAKE,           STR_SHOP_ITEM_INDEFINITE_FUNNEL_CAKE,           STR_SHOP_ITEM_DISPLAY_FUNNEL_CAKE         }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_SUNGLASSES */         {  8,    15,         20,        12,         MONEY(1, 50),  SPR_SHOP_ITEM_SUNGLASSES,          { STR_SHOP_ITEM_PRICE_LABEL_SUNGLASSES,             STR_SHOP_ITEM_SINGULAR_SUNGLASSES,          STR_SHOP_ITEM_PLURAL_SUNGLASSES,            STR_SHOP_ITEM_INDEFINITE_SUNGLASSES,            STR_SHOP_ITEM_DISPLAY_SUNGLASSES          }, SHOP_ITEM_FLAG_IS_SOUVENIR                           },
    /* SHOP_ITEM_BEEF_NOODLES */       {  7,    17,         17,        20,         MONEY(1, 50),  SPR_SHOP_ITEM_BEEF_NOODLES,        { STR_SHOP_ITEM_PRICE_LABEL_BEEF_NOODLES,           STR_SHOP_ITEM_SINGULAR_BEEF_NOODLES,        STR_SHOP_ITEM_PLURAL_BEEF_NOODLES,          STR_SHOP_ITEM_INDEFINITE_BEEF_NOODLES,          STR_SHOP_ITEM_DISPLAY_BEEF_NOODLES        }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_FRIED_RICE_NOODLES */ {  6,    17,         17,        20,         MONEY(1, 50),  SPR_SHOP_ITEM_FRIED_RICE_NOODLES,  { STR_SHOP_ITEM_PRICE_LABEL_FRIED_RICE_NOODLES,     STR_SHOP_ITEM_SINGULAR_FRIED_RICE_NOODLES,  STR_SHOP_ITEM_PLURAL_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_INDEFINITE_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_DISPLAY_FRIED_RICE_NOODLES  }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_WONTON_SOUP */        {  4,    13,         13,        15,         MONEY(1, 50),  SPR_SHOP_ITEM_WONTON_SOUP,         { STR_SHOP_ITEM_PRICE_LABEL_WONTON_SOUP,            STR_SHOP_ITEM_SINGULAR_WONTON_SOUP,         STR_SHOP_ITEM_PLURAL_WONTON_SOUP,           STR_SHOP_ITEM_INDEFINITE_WONTON_SOUP,           STR_SHOP_ITEM_DISPLAY_WONTON_SOUP         }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_MEATBALL_SOUP */      {  5,    14,         14,        16,         MONEY(1, 50),  SPR_SHOP_ITEM_MEATBALL_SOUP,       { STR_SHOP_ITEM_PRICE_LABEL_MEATBALL_SOUP,          STR_SHOP_ITEM_SINGULAR_MEATBALL_SOUP,       STR_SHOP_ITEM_PLURAL_MEATBALL_SOUP,         STR_SHOP_ITEM_INDEFINITE_MEATBALL_SOUP,         STR_SHOP_ITEM_DISPLAY_MEATBALL_SOUP       }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_FRUIT_JUICE */        {  4,    11,         19,        11,         MONEY(1, 20),  SPR_SHOP_ITEM_FRUIT_JUICE,         { STR_SHOP_ITEM_PRICE_LABEL_FRUIT_JUICE,            STR_SHOP_ITEM_SINGULAR_FRUIT_JUICE,         STR_SHOP_ITEM_PLURAL_FRUIT_JUICE,           STR_SHOP_ITEM_INDEFINITE_FRUIT_JUICE,           STR_SHOP_ITEM_DISPLAY_FRUIT_JUICE         }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_SOYBEAN_MILK */       {  4,    10,         14,        10,         MONEY(1, 20),  SPR_SHOP_ITEM_SOYBEAN_MILK,        { STR_SHOP_ITEM_PRICE_LABEL_SOYBEAN_MILK,           STR_SHOP_ITEM_SINGULAR_SOYBEAN_MILK,        STR_SHOP_ITEM_PLURAL_SOYBEAN_MILK,          STR_SHOP_ITEM_INDEFINITE_SOYBEAN_MILK,          STR_SHOP_ITEM_DISPLAY_SOYBEAN_MILK        }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_SUJEONGGWA */         {  3,    11,         14,        11,         MONEY(1, 20),  SPR_SHOP_ITEM_SUJEONGGWA,          { STR_SHOP_ITEM_PRICE_LABEL_SUJONGKWA,              STR_SHOP_ITEM_SINGULAR_SUJONGKWA,           STR_SHOP_ITEM_PLURAL_SUJONGKWA,             STR_SHOP_ITEM_INDEFINITE_SUJONGKWA,             STR_SHOP_ITEM_DISPLAY_SUJONGKWA           }, SHOP_ITEM_FLAG_IS_DRINK                              },
    /* SHOP_ITEM_SUB_SANDWICH */       {  5,    19,         19,        17,         MONEY(1, 50),  SPR_SHOP_ITEM_SUB_SANDWICH,        { STR_SHOP_ITEM_PRICE_LABEL_SUB_SANDWICH,           STR_SHOP_ITEM_SINGULAR_SUB_SANDWICH,        STR_SHOP_ITEM_PLURAL_SUB_SANDWICH,          STR_SHOP_ITEM_INDEFINITE_SUB_SANDWICH,          STR_SHOP_ITEM_DISPLAY_SUB_SANDWICH        }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_COOKIE */             {  4,    8,          8,         8,          MONEY(0, 70),  SPR_SHOP_ITEM_COOKIE,              { STR_SHOP_ITEM_PRICE_LABEL_COOKIE,                 STR_SHOP_ITEM_SINGULAR_COOKIE,              STR_SHOP_ITEM_PLURAL_COOKIE,                STR_SHOP_ITEM_INDEFINITE_COOKIE,                STR_SHOP_ITEM_DISPLAY_COOKIE              }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_EMPTY_BOWL_RED */     {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_BOWL_RED,      { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_RED,         STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_RED,      STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_RED,        STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_RED,        STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_RED      }, 0                                                    },
    /* SHOP_ITEM_EMPTY_DRINK_CARTON */ {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_DRINK_CARTON,  { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_DRINK_CARTON,     STR_SHOP_ITEM_SINGULAR_EMPTY_DRINK_CARTON,  STR_SHOP_ITEM_PLURAL_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_INDEFINITE_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_DISPLAY_EMPTY_DRINK_CARTON  }, 0                                                    },
    /* SHOP_ITEM_EMPTY_JUICE_CUP */    {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_JUICE_CUP,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_JUICE_CUP,        STR_SHOP_ITEM_SINGULAR_EMPTY_JUICE_CUP,     STR_SHOP_ITEM_PLURAL_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_INDEFINITE_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_DISPLAY_EMPTY_JUICE_CUP     }, 0                                                    },
    /* SHOP_ITEM_ROAST_SAUSAGE */      {  5,    16,         16,        20,         MONEY(1, 50),  SPR_SHOP_ITEM_ROAST_SAUSAGE,       { STR_SHOP_ITEM_PRICE_LABEL_ROAST_SAUSAGE,          STR_SHOP_ITEM_SINGULAR_ROAST_SAUSAGE,       STR_SHOP_ITEM_PLURAL_ROAST_SAUSAGE,         STR_SHOP_ITEM_INDEFINITE_ROAST_SAUSAGE,         STR_SHOP_ITEM_DISPLAY_ROAST_SAUSAGE       }, SHOP_ITEM_FLAG_IS_FOOD                               },
    /* SHOP_ITEM_EMPTY_BOWL_BLUE */    {  0,    0,          0,         0,          MONEY(0, 00),  SPR_SHOP_ITEM_EMPTY_BOWL_BLUE,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_BLUE,        STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_BLUE,     STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_BLUE     }, 0                                                    },
};
// clang-format on

money32 shop_item_get_common_price(Ride* forRide, const int32_t shopItem)
{
    for (const auto& ride : GetRideManager())
    {
        if (&ride != forRide)
        {
            auto rideEntry = ride.GetRideEntry();
            if (rideEntry == nullptr)
            {
                continue;
            }
            if (rideEntry->shop_item[0] == shopItem)
            {
                return ride.price[0];
            }
            if (rideEntry->shop_item[1] == shopItem)
            {
                return ride.price[1];
            }
            if (ShopItems[shopItem].IsPhoto() && (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            {
                return ride.price[1];
            }
        }
    }

    return MONEY32_UNDEFINED;
}

bool shop_item_has_common_price(const int32_t shopItem)
{
    return (gSamePriceThroughoutPark & (1ULL << shopItem)) != 0;
}

bool ShopItemDescriptor::HasFlag(const uint16_t flag) const
{
    return (Flags & flag) != 0;
}

bool ShopItemDescriptor::IsFood() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_FOOD);
}

bool ShopItemDescriptor::IsDrink() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_DRINK);
}

bool ShopItemDescriptor::IsFoodOrDrink() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_FOOD | SHOP_ITEM_FLAG_IS_DRINK);
}

bool ShopItemDescriptor::IsSouvenir() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_SOUVENIR);
}

bool ShopItemDescriptor::IsPhoto() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_PHOTO);
}
