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
#include "../entity/Guest.h"
#include "../localisation/StringIds.h"
#include "../ride/RideEntry.h"
#include "../sprites.h"

ShopItem& operator++(ShopItem& d, int)
{
    return d = (d == ShopItem::Count) ? ShopItem::Balloon : ShopItem(EnumValue(d) + 1);
}

uint64_t gSamePriceThroughoutPark;

// clang-format off
/** rct2: 0x00982164 (cost, base value, hot and cold value); 0x00982358 (default price) */
constexpr ShopItemDescriptor ShopItems[EnumValue(ShopItem::Count)] = {
    // Item,                            Cost,     Base value, Hot value, Cold value, Default price, Image,                               Price label,                                      Singular,                                   Plural,                                     Indefinite,                                     Display (in guest inventory),                Shop Item Flag,                                              Litter type,                    Consumption time, Discard Container,          Peep thought price too much,            Peep thought price good value,
    /* ShopItem::Balloon */          {  0.30_GBP, 1.40_GBP,   1.40_GBP,  1.40_GBP,   0.90_GBP,      SPR_SHOP_ITEM_BALLOON,             { STR_SHOP_ITEM_PRICE_LABEL_BALLOON,                STR_SHOP_ITEM_SINGULAR_BALLOON,             STR_SHOP_ITEM_PLURAL_BALLOON,               STR_SHOP_ITEM_INDEFINITE_BALLOON,               STR_SHOP_ITEM_DISPLAY_BALLOON             }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::BalloonMuch,           PeepThoughtType::Balloon            },
    /* ShopItem::Toy */              {  1.50_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   2.50_GBP,      SPR_SHOP_ITEM_TOY,                 { STR_SHOP_ITEM_PRICE_LABEL_CUDDLY_TOY,             STR_SHOP_ITEM_SINGULAR_CUDDLY_TOY,          STR_SHOP_ITEM_PLURAL_CUDDLY_TOY,            STR_SHOP_ITEM_INDEFINITE_CUDDLY_TOY,            STR_SHOP_ITEM_DISPLAY_CUDDLY_TOY          }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::ToyMuch,               PeepThoughtType::Toy                },
    /* ShopItem::Map */              {  0.10_GBP, 0.70_GBP,   0.70_GBP,  0.80_GBP,   0.60_GBP,      SPR_SHOP_ITEM_MAP,                 { STR_SHOP_ITEM_PRICE_LABEL_PARK_MAP,               STR_SHOP_ITEM_SINGULAR_PARK_MAP,            STR_SHOP_ITEM_PLURAL_PARK_MAP,              STR_SHOP_ITEM_INDEFINITE_PARK_MAP,              STR_SHOP_ITEM_DISPLAY_PARK_MAP            }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::MapMuch,               PeepThoughtType::Map                },
    /* ShopItem::Photo */            {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO,               { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::PhotoMuch,             PeepThoughtType::Photo              },
    /* ShopItem::Umbrella */         {  2.00_GBP, 3.50_GBP,   2.50_GBP,  5.00_GBP,   2.50_GBP,      SPR_SHOP_ITEM_UMBRELLA,            { STR_SHOP_ITEM_PRICE_LABEL_UMBRELLA,               STR_SHOP_ITEM_SINGULAR_UMBRELLA,            STR_SHOP_ITEM_PLURAL_UMBRELLA,              STR_SHOP_ITEM_INDEFINITE_UMBRELLA,              STR_SHOP_ITEM_DISPLAY_UMBRELLA            }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::UmbrellaMuch,          PeepThoughtType::Umbrella           },
    /* ShopItem::Drink */            {  0.30_GBP, 1.20_GBP,   2.00_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_DRINK,               { STR_SHOP_ITEM_PRICE_LABEL_DRINK,                  STR_SHOP_ITEM_SINGULAR_DRINK,               STR_SHOP_ITEM_PLURAL_DRINK,                 STR_SHOP_ITEM_INDEFINITE_DRINK,                 STR_SHOP_ITEM_DISPLAY_DRINK               }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          100,              ShopItem::EmptyCan,         PeepThoughtType::DrinkMuch,             PeepThoughtType::Drink              },
    /* ShopItem::Burger */           {  0.50_GBP, 1.90_GBP,   1.90_GBP,  2.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_BURGER,              { STR_SHOP_ITEM_PRICE_LABEL_BURGER,                 STR_SHOP_ITEM_SINGULAR_BURGER,              STR_SHOP_ITEM_PLURAL_BURGER,                STR_SHOP_ITEM_INDEFINITE_BURGER,                STR_SHOP_ITEM_DISPLAY_BURGER              }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          150,              ShopItem::EmptyBurgerBox,   PeepThoughtType::BurgerMuch,            PeepThoughtType::Burger             },
    /* ShopItem::Chips */            {  0.40_GBP, 1.60_GBP,   1.60_GBP,  1.80_GBP,   1.50_GBP,      SPR_SHOP_ITEM_CHIPS,               { STR_SHOP_ITEM_PRICE_LABEL_CHIPS,                  STR_SHOP_ITEM_SINGULAR_CHIPS,               STR_SHOP_ITEM_PLURAL_CHIPS,                 STR_SHOP_ITEM_INDEFINITE_CHIPS,                 STR_SHOP_ITEM_DISPLAY_CHIPS               }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          120,              ShopItem::Rubbish,          PeepThoughtType::ChipsMuch,             PeepThoughtType::Chips              },
    /* ShopItem::IceCream */         {  0.40_GBP, 1.00_GBP,   1.50_GBP,  0.60_GBP,   0.90_GBP,      SPR_SHOP_ITEM_ICE_CREAM,           { STR_SHOP_ITEM_PRICE_LABEL_ICE_CREAM,              STR_SHOP_ITEM_SINGULAR_ICE_CREAM,           STR_SHOP_ITEM_PLURAL_ICE_CREAM,             STR_SHOP_ITEM_INDEFINITE_ICE_CREAM,             STR_SHOP_ITEM_DISPLAY_ICE_CREAM           }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          60,               ShopItem::None,             PeepThoughtType::IceCreamMuch,          PeepThoughtType::IceCream           },
    /* ShopItem::Candyfloss */       {  0.30_GBP, 0.90_GBP,   0.90_GBP,  0.60_GBP,   0.80_GBP,      SPR_SHOP_ITEM_CANDYFLOSS,          { STR_SHOP_ITEM_PRICE_LABEL_CANDYFLOSS,             STR_SHOP_ITEM_SINGULAR_CANDYFLOSS,          STR_SHOP_ITEM_PLURAL_CANDYFLOSS,            STR_SHOP_ITEM_INDEFINITE_CANDYFLOSS,            STR_SHOP_ITEM_DISPLAY_CANDYFLOSS          }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          50,               ShopItem::None,             PeepThoughtType::CandyflossMuch,        PeepThoughtType::Candyfloss         },
    /* ShopItem::EmptyCan */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_CAN,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CAN,              STR_SHOP_ITEM_SINGULAR_EMPTY_CAN,           STR_SHOP_ITEM_PLURAL_EMPTY_CAN,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CAN,             STR_SHOP_ITEM_DISPLAY_EMPTY_CAN           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyCan,         0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Rubbish */          {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_RUBBISH,             { STR_SHOP_ITEM_PRICE_LABEL_RUBBISH,                STR_SHOP_ITEM_SINGULAR_RUBBISH,             STR_SHOP_ITEM_PLURAL_RUBBISH,               STR_SHOP_ITEM_INDEFINITE_RUBBISH,               STR_SHOP_ITEM_DISPLAY_RUBBISH             }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::EmptyBurgerBox */   {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BURGER_BOX,    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BURGER_BOX,       STR_SHOP_ITEM_SINGULAR_EMPTY_BURGER_BOX,    STR_SHOP_ITEM_PLURAL_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_INDEFINITE_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_DISPLAY_EMPTY_BURGER_BOX    }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::BurgerBox,        0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Pizza */            {  0.60_GBP, 2.10_GBP,   2.10_GBP,  2.50_GBP,   1.60_GBP,      SPR_SHOP_ITEM_PIZZA,               { STR_SHOP_ITEM_PRICE_LABEL_PIZZA,                  STR_SHOP_ITEM_SINGULAR_PIZZA,               STR_SHOP_ITEM_PLURAL_PIZZA,                 STR_SHOP_ITEM_INDEFINITE_PIZZA,                 STR_SHOP_ITEM_DISPLAY_PIZZA               }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          150,              ShopItem::Rubbish,          PeepThoughtType::PizzaMuch,             PeepThoughtType::Pizza              },
    /* ShopItem::Voucher */          {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_VOUCHER,             { STR_SHOP_ITEM_PRICE_LABEL_VOUCHER,                STR_SHOP_ITEM_SINGULAR_VOUCHER,             STR_SHOP_ITEM_PLURAL_VOUCHER,               STR_SHOP_ITEM_INDEFINITE_VOUCHER,               STR_SHOP_ITEM_DISPLAY_VOUCHER             }, 0,                                                           Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Popcorn */          {  0.50_GBP, 1.30_GBP,   1.30_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_POPCORN,             { STR_SHOP_ITEM_PRICE_LABEL_POPCORN,                STR_SHOP_ITEM_SINGULAR_POPCORN,             STR_SHOP_ITEM_PLURAL_POPCORN,               STR_SHOP_ITEM_INDEFINITE_POPCORN,               STR_SHOP_ITEM_DISPLAY_POPCORN             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          75,               ShopItem::Rubbish,          PeepThoughtType::PopcornMuch,           PeepThoughtType::Popcorn            },
    /* ShopItem::HotDog */           {  0.50_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.00_GBP,      SPR_SHOP_ITEM_HOT_DOG,             { STR_SHOP_ITEM_PRICE_LABEL_HOT_DOG,                STR_SHOP_ITEM_SINGULAR_HOT_DOG,             STR_SHOP_ITEM_PLURAL_HOT_DOG,               STR_SHOP_ITEM_INDEFINITE_HOT_DOG,               STR_SHOP_ITEM_DISPLAY_HOT_DOG             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          133,              ShopItem::None,             PeepThoughtType::HotDogMuch,            PeepThoughtType::HotDog             },
    /* ShopItem::Tentacle */         {  1.10_GBP, 2.20_GBP,   2.00_GBP,  1.80_GBP,   1.50_GBP,      SPR_SHOP_ITEM_TENTACLE,            { STR_SHOP_ITEM_PRICE_LABEL_TENTACLE,               STR_SHOP_ITEM_SINGULAR_TENTACLE,            STR_SHOP_ITEM_PLURAL_TENTACLE,              STR_SHOP_ITEM_INDEFINITE_TENTACLE,              STR_SHOP_ITEM_DISPLAY_TENTACLE            }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          110,              ShopItem::None,             PeepThoughtType::TentacleMuch,          PeepThoughtType::Tentacle           },
    /* ShopItem::Hat */              {  0.90_GBP, 2.70_GBP,   3.20_GBP,  2.40_GBP,   1.50_GBP,      SPR_SHOP_ITEM_HAT,                 { STR_SHOP_ITEM_PRICE_LABEL_HAT,                    STR_SHOP_ITEM_SINGULAR_HAT,                 STR_SHOP_ITEM_PLURAL_HAT,                   STR_SHOP_ITEM_INDEFINITE_HAT,                   STR_SHOP_ITEM_DISPLAY_HAT                 }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::HatMuch,               PeepThoughtType::Hat                },
    /* ShopItem::ToffeeApple */      {  0.40_GBP, 1.00_GBP,   1.00_GBP,  1.00_GBP,   0.70_GBP,      SPR_SHOP_ITEM_TOFFEE_APPLE,        { STR_SHOP_ITEM_PRICE_LABEL_TOFFEE_APPLE,           STR_SHOP_ITEM_SINGULAR_TOFFEE_APPLE,        STR_SHOP_ITEM_PLURAL_TOFFEE_APPLE,          STR_SHOP_ITEM_INDEFINITE_TOFFEE_APPLE,          STR_SHOP_ITEM_DISPLAY_TOFFEE_APPLE        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          50,               ShopItem::None,             PeepThoughtType::ToffeeAppleMuch,       PeepThoughtType::ToffeeApple        },
    /* ShopItem::TShirt */           {  2.00_GBP, 3.70_GBP,   3.70_GBP,  3.70_GBP,   3.00_GBP,      SPR_SHOP_ITEM_TSHIRT,              { STR_SHOP_ITEM_PRICE_LABEL_T_SHIRT,                STR_SHOP_ITEM_SINGULAR_T_SHIRT,             STR_SHOP_ITEM_PLURAL_T_SHIRT,               STR_SHOP_ITEM_INDEFINITE_T_SHIRT,               STR_SHOP_ITEM_DISPLAY_T_SHIRT             }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::TshirtMuch,            PeepThoughtType::Tshirt             },
    /* ShopItem::Doughnut */         {  0.40_GBP, 0.80_GBP,   0.70_GBP,  1.00_GBP,   0.70_GBP,      SPR_SHOP_ITEM_DOUGHNUT,            { STR_SHOP_ITEM_PRICE_LABEL_DOUGHNUT,               STR_SHOP_ITEM_SINGULAR_DOUGHNUT,            STR_SHOP_ITEM_PLURAL_DOUGHNUT,              STR_SHOP_ITEM_INDEFINITE_DOUGHNUT,              STR_SHOP_ITEM_DISPLAY_DOUGHNUT            }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          80,               ShopItem::None,             PeepThoughtType::DoughnutMuch,          PeepThoughtType::Doughnut           },
    /* ShopItem::Coffee */           {  0.30_GBP, 1.10_GBP,   1.50_GBP,  2.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_COFFEE,              { STR_SHOP_ITEM_PRICE_LABEL_COFFEE,                 STR_SHOP_ITEM_SINGULAR_COFFEE,              STR_SHOP_ITEM_PLURAL_COFFEE,                STR_SHOP_ITEM_INDEFINITE_COFFEE,                STR_SHOP_ITEM_DISPLAY_COFFEE              }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          90,               ShopItem::EmptyCup,         PeepThoughtType::CoffeeMuch,            PeepThoughtType::Coffee             },
    /* ShopItem::EmptyCup */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_CUP,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CUP,              STR_SHOP_ITEM_SINGULAR_EMPTY_CUP,           STR_SHOP_ITEM_PLURAL_EMPTY_CUP,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CUP,             STR_SHOP_ITEM_DISPLAY_EMPTY_CUP           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyCup,         0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Chicken */          {  0.50_GBP, 1.90_GBP,   1.90_GBP,  2.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_CHICKEN,             { STR_SHOP_ITEM_PRICE_LABEL_FRIED_CHICKEN,          STR_SHOP_ITEM_SINGULAR_FRIED_CHICKEN,       STR_SHOP_ITEM_PLURAL_FRIED_CHICKEN,         STR_SHOP_ITEM_INDEFINITE_FRIED_CHICKEN,         STR_SHOP_ITEM_DISPLAY_FRIED_CHICKEN       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::EmptyBox,         170,              ShopItem::EmptyBox,         PeepThoughtType::ChickenMuch,           PeepThoughtType::Chicken            },
    /* ShopItem::Lemonade */         {  0.40_GBP, 1.10_GBP,   2.10_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_LEMONADE,            { STR_SHOP_ITEM_PRICE_LABEL_LEMONADE,               STR_SHOP_ITEM_SINGULAR_LEMONADE,            STR_SHOP_ITEM_PLURAL_LEMONADE,              STR_SHOP_ITEM_INDEFINITE_LEMONADE,              STR_SHOP_ITEM_DISPLAY_LEMONADE            }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::EmptyBottle,      115,              ShopItem::EmptyBottle,      PeepThoughtType::LemonadeMuch,          PeepThoughtType::Lemonade           },
    /* ShopItem::EmptyBox */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOX,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOX,              STR_SHOP_ITEM_SINGULAR_EMPTY_BOX,           STR_SHOP_ITEM_PLURAL_EMPTY_BOX,             STR_SHOP_ITEM_INDEFINITE_EMPTY_BOX,             STR_SHOP_ITEM_DISPLAY_EMPTY_BOX           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyBox,         0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::EmptyBottle */      {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOTTLE,        { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOTTLE,           STR_SHOP_ITEM_SINGULAR_EMPTY_BOTTLE,        STR_SHOP_ITEM_PLURAL_EMPTY_BOTTLE,          STR_SHOP_ITEM_INDEFINITE_EMPTY_BOTTLE,          STR_SHOP_ITEM_DISPLAY_EMPTY_BOTTLE        }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyBottle,      0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 28 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0,                                                           Litter::Type::Vomit,            0xFF,             ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 29 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0,                                                           Litter::Type::Vomit,            0xFF,             ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 30 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0,                                                           Litter::Type::Vomit,            0xFF,             ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Admission */        {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { STR_NONE,                                         STR_NONE,                                   STR_NONE,                                   STR_NONE,                                       STR_NONE                                  }, 0,                                                           Litter::Type::Vomit,            0xFF,             ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::Photo2 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO2,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::Photo2Much,            PeepThoughtType::Photo2             },
    /* ShopItem::Photo3 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO3,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::Photo3Much,            PeepThoughtType::Photo3             },
    /* ShopItem::Photo4 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO4,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::Photo4Much,            PeepThoughtType::Photo4             },
    /* ShopItem::Pretzel */          {  0.50_GBP, 1.10_GBP,   1.10_GBP,  1.10_GBP,   1.10_GBP,      SPR_SHOP_ITEM_PRETZEL,             { STR_SHOP_ITEM_PRICE_LABEL_PRETZEL,                STR_SHOP_ITEM_SINGULAR_PRETZEL,             STR_SHOP_ITEM_PLURAL_PRETZEL,               STR_SHOP_ITEM_INDEFINITE_PRETZEL,               STR_SHOP_ITEM_DISPLAY_PRETZEL             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          70,               ShopItem::None,             PeepThoughtType::PretzelMuch,           PeepThoughtType::Pretzel            },
    /* ShopItem::Chocolate */        {  0.40_GBP, 1.30_GBP,   1.30_GBP,  2.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_CHOCOLATE,           { STR_SHOP_ITEM_PRICE_LABEL_HOT_CHOCOLATE,          STR_SHOP_ITEM_SINGULAR_HOT_CHOCOLATE,       STR_SHOP_ITEM_PLURAL_HOT_CHOCOLATE,         STR_SHOP_ITEM_INDEFINITE_HOT_CHOCOLATE,         STR_SHOP_ITEM_DISPLAY_HOT_CHOCOLATE       }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          85,               ShopItem::EmptyCup,         PeepThoughtType::HotChocolateMuch,      PeepThoughtType::HotChocolate       },
    /* ShopItem::IcedTea */          {  0.30_GBP, 1.00_GBP,   2.00_GBP,  1.00_GBP,   1.10_GBP,      SPR_SHOP_ITEM_ICED_TEA,            { STR_SHOP_ITEM_PRICE_LABEL_ICED_TEA,               STR_SHOP_ITEM_SINGULAR_ICED_TEA,            STR_SHOP_ITEM_PLURAL_ICED_TEA,              STR_SHOP_ITEM_INDEFINITE_ICED_TEA,              STR_SHOP_ITEM_DISPLAY_ICED_TEA            }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          95,               ShopItem::EmptyCup,         PeepThoughtType::IcedTeaMuch,           PeepThoughtType::IcedTea            },
    /* ShopItem::FunnelCake */       {  0.50_GBP, 1.30_GBP,   1.10_GBP,  1.40_GBP,   1.20_GBP,      SPR_SHOP_ITEM_FUNNEL_CAKE,         { STR_SHOP_ITEM_PRICE_LABEL_FUNNEL_CAKE,            STR_SHOP_ITEM_SINGULAR_FUNNEL_CAKE,         STR_SHOP_ITEM_PLURAL_FUNNEL_CAKE,           STR_SHOP_ITEM_INDEFINITE_FUNNEL_CAKE,           STR_SHOP_ITEM_DISPLAY_FUNNEL_CAKE         }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          90,               ShopItem::None,             PeepThoughtType::FunnelCakeMuch,        PeepThoughtType::FunnelCake         },
    /* ShopItem::Sunglasses */       {  0.80_GBP, 1.50_GBP,   2.00_GBP,  1.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_SUNGLASSES,          { STR_SHOP_ITEM_PRICE_LABEL_SUNGLASSES,             STR_SHOP_ITEM_SINGULAR_SUNGLASSES,          STR_SHOP_ITEM_PLURAL_SUNGLASSES,            STR_SHOP_ITEM_INDEFINITE_SUNGLASSES,            STR_SHOP_ITEM_DISPLAY_SUNGLASSES          }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::Rubbish,          0,                ShopItem::None,             PeepThoughtType::SunglassesMuch,        PeepThoughtType::Sunglasses         },
    /* ShopItem::BeefNoodles */      {  0.70_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_BEEF_NOODLES,        { STR_SHOP_ITEM_PRICE_LABEL_BEEF_NOODLES,           STR_SHOP_ITEM_SINGULAR_BEEF_NOODLES,        STR_SHOP_ITEM_PLURAL_BEEF_NOODLES,          STR_SHOP_ITEM_INDEFINITE_BEEF_NOODLES,          STR_SHOP_ITEM_DISPLAY_BEEF_NOODLES        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          130,              ShopItem::EmptyBowlBlue,    PeepThoughtType::BeefNoodlesMuch,       PeepThoughtType::BeefNoodles        },
    /* ShopItem::FriedRiceNoodles */ {  0.60_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_FRIED_RICE_NOODLES,  { STR_SHOP_ITEM_PRICE_LABEL_FRIED_RICE_NOODLES,     STR_SHOP_ITEM_SINGULAR_FRIED_RICE_NOODLES,  STR_SHOP_ITEM_PLURAL_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_INDEFINITE_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_DISPLAY_FRIED_RICE_NOODLES  }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          120,              ShopItem::EmptyBowlBlue,    PeepThoughtType::FriedRiceNoodlesMuch,  PeepThoughtType::FriedRiceNoodles   },
    /* ShopItem::WontonSoup */       {  0.40_GBP, 1.30_GBP,   1.30_GBP,  1.50_GBP,   1.50_GBP,      SPR_SHOP_ITEM_WONTON_SOUP,         { STR_SHOP_ITEM_PRICE_LABEL_WONTON_SOUP,            STR_SHOP_ITEM_SINGULAR_WONTON_SOUP,         STR_SHOP_ITEM_PLURAL_WONTON_SOUP,           STR_SHOP_ITEM_INDEFINITE_WONTON_SOUP,           STR_SHOP_ITEM_DISPLAY_WONTON_SOUP         }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          100,              ShopItem::EmptyBowlRed,     PeepThoughtType::WontonSoupMuch,        PeepThoughtType::WontonSoup         },
    /* ShopItem::MeatballSoup */     {  0.50_GBP, 1.40_GBP,   1.40_GBP,  1.60_GBP,   1.50_GBP,      SPR_SHOP_ITEM_MEATBALL_SOUP,       { STR_SHOP_ITEM_PRICE_LABEL_MEATBALL_SOUP,          STR_SHOP_ITEM_SINGULAR_MEATBALL_SOUP,       STR_SHOP_ITEM_PLURAL_MEATBALL_SOUP,         STR_SHOP_ITEM_INDEFINITE_MEATBALL_SOUP,         STR_SHOP_ITEM_DISPLAY_MEATBALL_SOUP       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          110,              ShopItem::EmptyBowlRed,     PeepThoughtType::MeatballSoupMuch,      PeepThoughtType::MeatballSoup       },
    /* ShopItem::FruitJuice */       {  0.40_GBP, 1.10_GBP,   1.90_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_FRUIT_JUICE,         { STR_SHOP_ITEM_PRICE_LABEL_FRUIT_JUICE,            STR_SHOP_ITEM_SINGULAR_FRUIT_JUICE,         STR_SHOP_ITEM_PLURAL_FRUIT_JUICE,           STR_SHOP_ITEM_INDEFINITE_FRUIT_JUICE,           STR_SHOP_ITEM_DISPLAY_FRUIT_JUICE         }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          110,              ShopItem::EmptyJuiceCup,    PeepThoughtType::FruitJuiceMuch,        PeepThoughtType::FruitJuice         },
    /* ShopItem::SoybeanMilk */      {  0.40_GBP, 1.00_GBP,   1.40_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_SOYBEAN_MILK,        { STR_SHOP_ITEM_PRICE_LABEL_SOYBEAN_MILK,           STR_SHOP_ITEM_SINGULAR_SOYBEAN_MILK,        STR_SHOP_ITEM_PLURAL_SOYBEAN_MILK,          STR_SHOP_ITEM_INDEFINITE_SOYBEAN_MILK,          STR_SHOP_ITEM_DISPLAY_SOYBEAN_MILK        }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          90,               ShopItem::EmptyDrinkCarton, PeepThoughtType::SoybeanMilkMuch,       PeepThoughtType::SoybeanMilk        },
    /* ShopItem::Sujeonggwa */       {  0.30_GBP, 1.10_GBP,   1.40_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_SUJEONGGWA,          { STR_SHOP_ITEM_PRICE_LABEL_SUJONGKWA,              STR_SHOP_ITEM_SINGULAR_SUJONGKWA,           STR_SHOP_ITEM_PLURAL_SUJONGKWA,             STR_SHOP_ITEM_INDEFINITE_SUJONGKWA,             STR_SHOP_ITEM_DISPLAY_SUJONGKWA           }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::Rubbish,          100,              ShopItem::EmptyDrinkCarton, PeepThoughtType::SujongkwaMuch,         PeepThoughtType::Sujongkwa          },
    /* ShopItem::SubSandwich */      {  0.50_GBP, 1.90_GBP,   1.90_GBP,  1.70_GBP,   1.50_GBP,      SPR_SHOP_ITEM_SUB_SANDWICH,        { STR_SHOP_ITEM_PRICE_LABEL_SUB_SANDWICH,           STR_SHOP_ITEM_SINGULAR_SUB_SANDWICH,        STR_SHOP_ITEM_PLURAL_SUB_SANDWICH,          STR_SHOP_ITEM_INDEFINITE_SUB_SANDWICH,          STR_SHOP_ITEM_DISPLAY_SUB_SANDWICH        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          130,              ShopItem::None,             PeepThoughtType::SubSandwichMuch,       PeepThoughtType::SubSandwich        },
    /* ShopItem::Cookie */           {  0.40_GBP, 0.80_GBP,   0.80_GBP,  0.80_GBP,   0.70_GBP,      SPR_SHOP_ITEM_COOKIE,              { STR_SHOP_ITEM_PRICE_LABEL_COOKIE,                 STR_SHOP_ITEM_SINGULAR_COOKIE,              STR_SHOP_ITEM_PLURAL_COOKIE,                STR_SHOP_ITEM_INDEFINITE_COOKIE,                STR_SHOP_ITEM_DISPLAY_COOKIE              }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          75,               ShopItem::None,             PeepThoughtType::CookieMuch,            PeepThoughtType::Cookie             },
    /* ShopItem::EmptyBowlRed */     {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOWL_RED,      { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_RED,         STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_RED,      STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_RED,        STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_RED,        STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_RED      }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyBowlRed,     0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::EmptyDrinkCarton */ {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_DRINK_CARTON,  { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_DRINK_CARTON,     STR_SHOP_ITEM_SINGULAR_EMPTY_DRINK_CARTON,  STR_SHOP_ITEM_PLURAL_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_INDEFINITE_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_DISPLAY_EMPTY_DRINK_CARTON  }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyDrinkCarton, 0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::EmptyJuiceCup */    {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_JUICE_CUP,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_JUICE_CUP,        STR_SHOP_ITEM_SINGULAR_EMPTY_JUICE_CUP,     STR_SHOP_ITEM_PLURAL_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_INDEFINITE_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_DISPLAY_EMPTY_JUICE_CUP     }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyJuiceCup,    0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::RoastSausage */     {  0.50_GBP, 1.60_GBP,   1.60_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_ROAST_SAUSAGE,       { STR_SHOP_ITEM_PRICE_LABEL_ROAST_SAUSAGE,          STR_SHOP_ITEM_SINGULAR_ROAST_SAUSAGE,       STR_SHOP_ITEM_PLURAL_ROAST_SAUSAGE,         STR_SHOP_ITEM_INDEFINITE_ROAST_SAUSAGE,         STR_SHOP_ITEM_DISPLAY_ROAST_SAUSAGE       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::Rubbish,          115,              ShopItem::None,             PeepThoughtType::RoastSausageMuch,      PeepThoughtType::RoastSausage       },
    /* ShopItem::EmptyBowlBlue */    {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOWL_BLUE,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_BLUE,        STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_BLUE,     STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_BLUE     }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::EmptyBowlBlue,    0,                ShopItem::None,             PeepThoughtType::None,                  PeepThoughtType::None               },
};
// clang-format on

constexpr uint64_t GetAllShopItemsWithFlag(uint16_t flag)
{
    uint64_t ret = 0;
    for (auto i = 0; i < EnumValue(ShopItem::Count); ++i)
    {
        const auto& sid = ShopItems[i];
        if (sid.HasFlag(flag))
        {
            ret |= (1ULL << i);
        }
    }
    return ret;
}

constexpr auto AllFoodFlags = GetAllShopItemsWithFlag(SHOP_ITEM_FLAG_IS_FOOD);
uint64_t ShopItemsGetAllFoods()
{
    return AllFoodFlags;
}

constexpr auto AllDrinkFlags = GetAllShopItemsWithFlag(SHOP_ITEM_FLAG_IS_DRINK);
uint64_t ShopItemsGetAllDrinks()
{
    return AllDrinkFlags;
}

constexpr auto AllContainerFlags = GetAllShopItemsWithFlag(SHOP_ITEM_FLAG_IS_CONTAINER);
uint64_t ShopItemsGetAllContainers()
{
    return AllContainerFlags;
}

money32 shop_item_get_common_price(Ride* forRide, const ShopItem shopItem)
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
            if (GetShopItemDescriptor(shopItem).IsPhoto() && (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            {
                return ride.price[1];
            }
        }
    }

    return MONEY32_UNDEFINED;
}

bool shop_item_has_common_price(const ShopItem shopItem)
{
    return (gSamePriceThroughoutPark & EnumToFlag(shopItem)) != 0;
}

bool shop_item_sold_elsewhere(const ShopItem shopItem, RideId excludeRideId)
{
    if (shopItem == ShopItem::None)
        return false;

    for (const auto& ride : GetRideManager())
    {
        if (ride.id == excludeRideId)
            continue;

        if (shopItem == ShopItem::Admission && ride.type == RIDE_TYPE_TOILETS)
            return true;

        const auto rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
            continue;

        for (auto currentShopItem : rideEntry->shop_item)
        {
            if (currentShopItem == shopItem) {
                return true;
            }
        }
    }

    return false;
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

bool ShopItemDescriptor::IsRecolourable() const
{
    return HasFlag(SHOP_ITEM_FLAG_IS_RECOLOURABLE);
}

const ShopItemDescriptor& GetShopItemDescriptor(const ShopItem item)
{
    return ShopItems[EnumValue(item)];
}
