/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShopItem.h"

#include "../GameState.h"
#include "../SpriteIds.h"
#include "../entity/Guest.h"
#include "../localisation/StringIds.h"
#include "../ride/RideEntry.h"
#include "../ride/RideManager.hpp"

using namespace OpenRCT2;

ShopItem& operator++(ShopItem& d, int)
{
    return d = (d == ShopItem::count) ? ShopItem::balloon : ShopItem(EnumValue(d) + 1);
}

// clang-format off
/** rct2: 0x00982164 (cost, base value, hot and cold value); 0x00982358 (default price) */
constexpr ShopItemDescriptor ShopItems[EnumValue(ShopItem::count)] = {
    // Item,                            Cost,     Base value, Hot value, Cold value, Default price, Image,                               Price label,                                      Singular,                                   Plural,                                     Indefinite,                                     Display (in guest inventory),                Shop Item Flag,                                              Litter type,                    Consumption time, Discard Container,          Peep thought price too much,            Peep thought price good value,
    /* ShopItem::balloon */          {  0.30_GBP, 1.40_GBP,   1.40_GBP,  1.40_GBP,   0.90_GBP,      SPR_SHOP_ITEM_BALLOON,             { STR_SHOP_ITEM_PRICE_LABEL_BALLOON,                STR_SHOP_ITEM_SINGULAR_BALLOON,             STR_SHOP_ITEM_PLURAL_BALLOON,               STR_SHOP_ITEM_INDEFINITE_BALLOON,               STR_SHOP_ITEM_DISPLAY_BALLOON             }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::BalloonMuch,           PeepThoughtType::Balloon            },
    /* ShopItem::toy */              {  1.50_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   2.50_GBP,      SPR_SHOP_ITEM_TOY,                 { STR_SHOP_ITEM_PRICE_LABEL_CUDDLY_TOY,             STR_SHOP_ITEM_SINGULAR_CUDDLY_TOY,          STR_SHOP_ITEM_PLURAL_CUDDLY_TOY,            STR_SHOP_ITEM_INDEFINITE_CUDDLY_TOY,            STR_SHOP_ITEM_DISPLAY_CUDDLY_TOY          }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::ToyMuch,               PeepThoughtType::Toy                },
    /* ShopItem::map */              {  0.10_GBP, 0.70_GBP,   0.70_GBP,  0.80_GBP,   0.60_GBP,      SPR_SHOP_ITEM_MAP,                 { STR_SHOP_ITEM_PRICE_LABEL_PARK_MAP,               STR_SHOP_ITEM_SINGULAR_PARK_MAP,            STR_SHOP_ITEM_PLURAL_PARK_MAP,              STR_SHOP_ITEM_INDEFINITE_PARK_MAP,              STR_SHOP_ITEM_DISPLAY_PARK_MAP            }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::MapMuch,               PeepThoughtType::Map                },
    /* ShopItem::photo */            {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO,               { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::PhotoMuch,             PeepThoughtType::Photo              },
    /* ShopItem::umbrella */         {  2.00_GBP, 3.50_GBP,   2.50_GBP,  5.00_GBP,   2.50_GBP,      SPR_SHOP_ITEM_UMBRELLA,            { STR_SHOP_ITEM_PRICE_LABEL_UMBRELLA,               STR_SHOP_ITEM_SINGULAR_UMBRELLA,            STR_SHOP_ITEM_PLURAL_UMBRELLA,              STR_SHOP_ITEM_INDEFINITE_UMBRELLA,              STR_SHOP_ITEM_DISPLAY_UMBRELLA            }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::UmbrellaMuch,          PeepThoughtType::Umbrella           },
    /* ShopItem::drink */            {  0.30_GBP, 1.20_GBP,   2.00_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_DRINK,               { STR_SHOP_ITEM_PRICE_LABEL_DRINK,                  STR_SHOP_ITEM_SINGULAR_DRINK,               STR_SHOP_ITEM_PLURAL_DRINK,                 STR_SHOP_ITEM_INDEFINITE_DRINK,                 STR_SHOP_ITEM_DISPLAY_DRINK               }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          100,              ShopItem::emptyCan,         PeepThoughtType::DrinkMuch,             PeepThoughtType::Drink              },
    /* ShopItem::burger */           {  0.50_GBP, 1.90_GBP,   1.90_GBP,  2.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_BURGER,              { STR_SHOP_ITEM_PRICE_LABEL_BURGER,                 STR_SHOP_ITEM_SINGULAR_BURGER,              STR_SHOP_ITEM_PLURAL_BURGER,                STR_SHOP_ITEM_INDEFINITE_BURGER,                STR_SHOP_ITEM_DISPLAY_BURGER              }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          150,              ShopItem::emptyBurgerBox,   PeepThoughtType::BurgerMuch,            PeepThoughtType::Burger             },
    /* ShopItem::chips */            {  0.40_GBP, 1.60_GBP,   1.60_GBP,  1.80_GBP,   1.50_GBP,      SPR_SHOP_ITEM_CHIPS,               { STR_SHOP_ITEM_PRICE_LABEL_CHIPS,                  STR_SHOP_ITEM_SINGULAR_CHIPS,               STR_SHOP_ITEM_PLURAL_CHIPS,                 STR_SHOP_ITEM_INDEFINITE_CHIPS,                 STR_SHOP_ITEM_DISPLAY_CHIPS               }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          120,              ShopItem::rubbish,          PeepThoughtType::ChipsMuch,             PeepThoughtType::Chips              },
    /* ShopItem::iceCream */         {  0.40_GBP, 1.00_GBP,   1.50_GBP,  0.60_GBP,   0.90_GBP,      SPR_SHOP_ITEM_ICE_CREAM,           { STR_SHOP_ITEM_PRICE_LABEL_ICE_CREAM,              STR_SHOP_ITEM_SINGULAR_ICE_CREAM,           STR_SHOP_ITEM_PLURAL_ICE_CREAM,             STR_SHOP_ITEM_INDEFINITE_ICE_CREAM,             STR_SHOP_ITEM_DISPLAY_ICE_CREAM           }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          60,               ShopItem::none,             PeepThoughtType::IceCreamMuch,          PeepThoughtType::IceCream           },
    /* ShopItem::candyfloss */       {  0.30_GBP, 0.90_GBP,   0.90_GBP,  0.60_GBP,   0.80_GBP,      SPR_SHOP_ITEM_CANDYFLOSS,          { STR_SHOP_ITEM_PRICE_LABEL_CANDYFLOSS,             STR_SHOP_ITEM_SINGULAR_CANDYFLOSS,          STR_SHOP_ITEM_PLURAL_CANDYFLOSS,            STR_SHOP_ITEM_INDEFINITE_CANDYFLOSS,            STR_SHOP_ITEM_DISPLAY_CANDYFLOSS          }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          50,               ShopItem::none,             PeepThoughtType::CandyflossMuch,        PeepThoughtType::Candyfloss         },
    /* ShopItem::emptyCan */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_CAN,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CAN,              STR_SHOP_ITEM_SINGULAR_EMPTY_CAN,           STR_SHOP_ITEM_PLURAL_EMPTY_CAN,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CAN,             STR_SHOP_ITEM_DISPLAY_EMPTY_CAN           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyCan,         0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::rubbish */          {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_RUBBISH,             { STR_SHOP_ITEM_PRICE_LABEL_RUBBISH,                STR_SHOP_ITEM_SINGULAR_RUBBISH,             STR_SHOP_ITEM_PLURAL_RUBBISH,               STR_SHOP_ITEM_INDEFINITE_RUBBISH,               STR_SHOP_ITEM_DISPLAY_RUBBISH             }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::emptyBurgerBox */   {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BURGER_BOX,    { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BURGER_BOX,       STR_SHOP_ITEM_SINGULAR_EMPTY_BURGER_BOX,    STR_SHOP_ITEM_PLURAL_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_INDEFINITE_EMPTY_BURGER_BOX,      STR_SHOP_ITEM_DISPLAY_EMPTY_BURGER_BOX    }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::burgerBox,        0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::pizza */            {  0.60_GBP, 2.10_GBP,   2.10_GBP,  2.50_GBP,   1.60_GBP,      SPR_SHOP_ITEM_PIZZA,               { STR_SHOP_ITEM_PRICE_LABEL_PIZZA,                  STR_SHOP_ITEM_SINGULAR_PIZZA,               STR_SHOP_ITEM_PLURAL_PIZZA,                 STR_SHOP_ITEM_INDEFINITE_PIZZA,                 STR_SHOP_ITEM_DISPLAY_PIZZA               }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          150,              ShopItem::rubbish,          PeepThoughtType::PizzaMuch,             PeepThoughtType::Pizza              },
    /* ShopItem::voucher */          {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_VOUCHER,             { STR_SHOP_ITEM_PRICE_LABEL_VOUCHER,                STR_SHOP_ITEM_SINGULAR_VOUCHER,             STR_SHOP_ITEM_PLURAL_VOUCHER,               STR_SHOP_ITEM_INDEFINITE_VOUCHER,               STR_SHOP_ITEM_DISPLAY_VOUCHER             }, 0,                                                           Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::popcorn */          {  0.50_GBP, 1.30_GBP,   1.30_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_POPCORN,             { STR_SHOP_ITEM_PRICE_LABEL_POPCORN,                STR_SHOP_ITEM_SINGULAR_POPCORN,             STR_SHOP_ITEM_PLURAL_POPCORN,               STR_SHOP_ITEM_INDEFINITE_POPCORN,               STR_SHOP_ITEM_DISPLAY_POPCORN             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          75,               ShopItem::rubbish,          PeepThoughtType::PopcornMuch,           PeepThoughtType::Popcorn            },
    /* ShopItem::hotDog */           {  0.50_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.00_GBP,      SPR_SHOP_ITEM_HOT_DOG,             { STR_SHOP_ITEM_PRICE_LABEL_HOT_DOG,                STR_SHOP_ITEM_SINGULAR_HOT_DOG,             STR_SHOP_ITEM_PLURAL_HOT_DOG,               STR_SHOP_ITEM_INDEFINITE_HOT_DOG,               STR_SHOP_ITEM_DISPLAY_HOT_DOG             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          133,              ShopItem::none,             PeepThoughtType::HotDogMuch,            PeepThoughtType::HotDog             },
    /* ShopItem::tentacle */         {  1.10_GBP, 2.20_GBP,   2.00_GBP,  1.80_GBP,   1.50_GBP,      SPR_SHOP_ITEM_TENTACLE,            { STR_SHOP_ITEM_PRICE_LABEL_TENTACLE,               STR_SHOP_ITEM_SINGULAR_TENTACLE,            STR_SHOP_ITEM_PLURAL_TENTACLE,              STR_SHOP_ITEM_INDEFINITE_TENTACLE,              STR_SHOP_ITEM_DISPLAY_TENTACLE            }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          110,              ShopItem::none,             PeepThoughtType::TentacleMuch,          PeepThoughtType::Tentacle           },
    /* ShopItem::hat */              {  0.90_GBP, 2.70_GBP,   3.20_GBP,  2.40_GBP,   1.50_GBP,      SPR_SHOP_ITEM_HAT,                 { STR_SHOP_ITEM_PRICE_LABEL_HAT,                    STR_SHOP_ITEM_SINGULAR_HAT,                 STR_SHOP_ITEM_PLURAL_HAT,                   STR_SHOP_ITEM_INDEFINITE_HAT,                   STR_SHOP_ITEM_DISPLAY_HAT                 }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::HatMuch,               PeepThoughtType::Hat                },
    /* ShopItem::toffeeApple */      {  0.40_GBP, 1.00_GBP,   1.00_GBP,  1.00_GBP,   0.70_GBP,      SPR_SHOP_ITEM_TOFFEE_APPLE,        { STR_SHOP_ITEM_PRICE_LABEL_TOFFEE_APPLE,           STR_SHOP_ITEM_SINGULAR_TOFFEE_APPLE,        STR_SHOP_ITEM_PLURAL_TOFFEE_APPLE,          STR_SHOP_ITEM_INDEFINITE_TOFFEE_APPLE,          STR_SHOP_ITEM_DISPLAY_TOFFEE_APPLE        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          50,               ShopItem::none,             PeepThoughtType::ToffeeAppleMuch,       PeepThoughtType::ToffeeApple        },
    /* ShopItem::tShirt */           {  2.00_GBP, 3.70_GBP,   3.70_GBP,  3.70_GBP,   3.00_GBP,      SPR_SHOP_ITEM_TSHIRT,              { STR_SHOP_ITEM_PRICE_LABEL_T_SHIRT,                STR_SHOP_ITEM_SINGULAR_T_SHIRT,             STR_SHOP_ITEM_PLURAL_T_SHIRT,               STR_SHOP_ITEM_INDEFINITE_T_SHIRT,               STR_SHOP_ITEM_DISPLAY_T_SHIRT             }, SHOP_ITEM_FLAG_IS_SOUVENIR | SHOP_ITEM_FLAG_IS_RECOLOURABLE, Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::TshirtMuch,            PeepThoughtType::Tshirt             },
    /* ShopItem::doughnut */         {  0.40_GBP, 0.80_GBP,   0.70_GBP,  1.00_GBP,   0.70_GBP,      SPR_SHOP_ITEM_DOUGHNUT,            { STR_SHOP_ITEM_PRICE_LABEL_DOUGHNUT,               STR_SHOP_ITEM_SINGULAR_DOUGHNUT,            STR_SHOP_ITEM_PLURAL_DOUGHNUT,              STR_SHOP_ITEM_INDEFINITE_DOUGHNUT,              STR_SHOP_ITEM_DISPLAY_DOUGHNUT            }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          80,               ShopItem::none,             PeepThoughtType::DoughnutMuch,          PeepThoughtType::Doughnut           },
    /* ShopItem::coffee */           {  0.30_GBP, 1.10_GBP,   1.50_GBP,  2.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_COFFEE,              { STR_SHOP_ITEM_PRICE_LABEL_COFFEE,                 STR_SHOP_ITEM_SINGULAR_COFFEE,              STR_SHOP_ITEM_PLURAL_COFFEE,                STR_SHOP_ITEM_INDEFINITE_COFFEE,                STR_SHOP_ITEM_DISPLAY_COFFEE              }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          90,               ShopItem::emptyCup,         PeepThoughtType::CoffeeMuch,            PeepThoughtType::Coffee             },
    /* ShopItem::emptyCup */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_CUP,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_CUP,              STR_SHOP_ITEM_SINGULAR_EMPTY_CUP,           STR_SHOP_ITEM_PLURAL_EMPTY_CUP,             STR_SHOP_ITEM_INDEFINITE_EMPTY_CUP,             STR_SHOP_ITEM_DISPLAY_EMPTY_CUP           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyCup,         0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::chicken */          {  0.50_GBP, 1.90_GBP,   1.90_GBP,  2.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_CHICKEN,             { STR_SHOP_ITEM_PRICE_LABEL_FRIED_CHICKEN,          STR_SHOP_ITEM_SINGULAR_FRIED_CHICKEN,       STR_SHOP_ITEM_PLURAL_FRIED_CHICKEN,         STR_SHOP_ITEM_INDEFINITE_FRIED_CHICKEN,         STR_SHOP_ITEM_DISPLAY_FRIED_CHICKEN       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::emptyBox,         170,              ShopItem::emptyBox,         PeepThoughtType::ChickenMuch,           PeepThoughtType::Chicken            },
    /* ShopItem::lemonade */         {  0.40_GBP, 1.10_GBP,   2.10_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_LEMONADE,            { STR_SHOP_ITEM_PRICE_LABEL_LEMONADE,               STR_SHOP_ITEM_SINGULAR_LEMONADE,            STR_SHOP_ITEM_PLURAL_LEMONADE,              STR_SHOP_ITEM_INDEFINITE_LEMONADE,              STR_SHOP_ITEM_DISPLAY_LEMONADE            }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::emptyBottle,      115,              ShopItem::emptyBottle,      PeepThoughtType::LemonadeMuch,          PeepThoughtType::Lemonade           },
    /* ShopItem::emptyBox */         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOX,           { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOX,              STR_SHOP_ITEM_SINGULAR_EMPTY_BOX,           STR_SHOP_ITEM_PLURAL_EMPTY_BOX,             STR_SHOP_ITEM_INDEFINITE_EMPTY_BOX,             STR_SHOP_ITEM_DISPLAY_EMPTY_BOX           }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyBox,         0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::emptyBottle */      {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOTTLE,        { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOTTLE,           STR_SHOP_ITEM_SINGULAR_EMPTY_BOTTLE,        STR_SHOP_ITEM_PLURAL_EMPTY_BOTTLE,          STR_SHOP_ITEM_INDEFINITE_EMPTY_BOTTLE,          STR_SHOP_ITEM_DISPLAY_EMPTY_BOTTLE        }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyBottle,      0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 28 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { kStringIdNone,                                         kStringIdNone,                                   kStringIdNone,                                   kStringIdNone,                                       kStringIdNone                                  }, 0,                                                           Litter::Type::vomit,            0xFF,             ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 29 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { kStringIdNone,                                         kStringIdNone,                                   kStringIdNone,                                   kStringIdNone,                                       kStringIdNone                                  }, 0,                                                           Litter::Type::vomit,            0xFF,             ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* 30 */                         {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { kStringIdNone,                                         kStringIdNone,                                   kStringIdNone,                                   kStringIdNone,                                       kStringIdNone                                  }, 0,                                                           Litter::Type::vomit,            0xFF,             ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::admission */        {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      0,                                 { kStringIdNone,                                         kStringIdNone,                                   kStringIdNone,                                   kStringIdNone,                                       kStringIdNone                                  }, 0,                                                           Litter::Type::vomit,            0xFF,             ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::photo2 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO2,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::Photo2Much,            PeepThoughtType::Photo2             },
    /* ShopItem::photo3 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO3,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::Photo3Much,            PeepThoughtType::Photo3             },
    /* ShopItem::photo4 */           {  0.20_GBP, 3.00_GBP,   3.00_GBP,  3.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_PHOTO4,              { STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO,          STR_SHOP_ITEM_SINGULAR_ON_RIDE_PHOTO,       STR_SHOP_ITEM_PLURAL_ON_RIDE_PHOTO,         STR_SHOP_ITEM_INDEFINITE_ON_RIDE_PHOTO,         STR_SHOP_ITEM_DISPLAY_ON_RIDE_PHOTO       }, SHOP_ITEM_FLAG_IS_PHOTO | SHOP_ITEM_FLAG_IS_SOUVENIR,        Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::Photo4Much,            PeepThoughtType::Photo4             },
    /* ShopItem::pretzel */          {  0.50_GBP, 1.10_GBP,   1.10_GBP,  1.10_GBP,   1.10_GBP,      SPR_SHOP_ITEM_PRETZEL,             { STR_SHOP_ITEM_PRICE_LABEL_PRETZEL,                STR_SHOP_ITEM_SINGULAR_PRETZEL,             STR_SHOP_ITEM_PLURAL_PRETZEL,               STR_SHOP_ITEM_INDEFINITE_PRETZEL,               STR_SHOP_ITEM_DISPLAY_PRETZEL             }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          70,               ShopItem::none,             PeepThoughtType::PretzelMuch,           PeepThoughtType::Pretzel            },
    /* ShopItem::chocolate */        {  0.40_GBP, 1.30_GBP,   1.30_GBP,  2.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_CHOCOLATE,           { STR_SHOP_ITEM_PRICE_LABEL_HOT_CHOCOLATE,          STR_SHOP_ITEM_SINGULAR_HOT_CHOCOLATE,       STR_SHOP_ITEM_PLURAL_HOT_CHOCOLATE,         STR_SHOP_ITEM_INDEFINITE_HOT_CHOCOLATE,         STR_SHOP_ITEM_DISPLAY_HOT_CHOCOLATE       }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          85,               ShopItem::emptyCup,         PeepThoughtType::HotChocolateMuch,      PeepThoughtType::HotChocolate       },
    /* ShopItem::icedTea */          {  0.30_GBP, 1.00_GBP,   2.00_GBP,  1.00_GBP,   1.10_GBP,      SPR_SHOP_ITEM_ICED_TEA,            { STR_SHOP_ITEM_PRICE_LABEL_ICED_TEA,               STR_SHOP_ITEM_SINGULAR_ICED_TEA,            STR_SHOP_ITEM_PLURAL_ICED_TEA,              STR_SHOP_ITEM_INDEFINITE_ICED_TEA,              STR_SHOP_ITEM_DISPLAY_ICED_TEA            }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          95,               ShopItem::emptyCup,         PeepThoughtType::IcedTeaMuch,           PeepThoughtType::IcedTea            },
    /* ShopItem::funnelCake */       {  0.50_GBP, 1.30_GBP,   1.10_GBP,  1.40_GBP,   1.20_GBP,      SPR_SHOP_ITEM_FUNNEL_CAKE,         { STR_SHOP_ITEM_PRICE_LABEL_FUNNEL_CAKE,            STR_SHOP_ITEM_SINGULAR_FUNNEL_CAKE,         STR_SHOP_ITEM_PLURAL_FUNNEL_CAKE,           STR_SHOP_ITEM_INDEFINITE_FUNNEL_CAKE,           STR_SHOP_ITEM_DISPLAY_FUNNEL_CAKE         }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          90,               ShopItem::none,             PeepThoughtType::FunnelCakeMuch,        PeepThoughtType::FunnelCake         },
    /* ShopItem::sunglasses */       {  0.80_GBP, 1.50_GBP,   2.00_GBP,  1.20_GBP,   1.50_GBP,      SPR_SHOP_ITEM_SUNGLASSES,          { STR_SHOP_ITEM_PRICE_LABEL_SUNGLASSES,             STR_SHOP_ITEM_SINGULAR_SUNGLASSES,          STR_SHOP_ITEM_PLURAL_SUNGLASSES,            STR_SHOP_ITEM_INDEFINITE_SUNGLASSES,            STR_SHOP_ITEM_DISPLAY_SUNGLASSES          }, SHOP_ITEM_FLAG_IS_SOUVENIR,                                  Litter::Type::rubbish,          0,                ShopItem::none,             PeepThoughtType::SunglassesMuch,        PeepThoughtType::Sunglasses         },
    /* ShopItem::beefNoodles */      {  0.70_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_BEEF_NOODLES,        { STR_SHOP_ITEM_PRICE_LABEL_BEEF_NOODLES,           STR_SHOP_ITEM_SINGULAR_BEEF_NOODLES,        STR_SHOP_ITEM_PLURAL_BEEF_NOODLES,          STR_SHOP_ITEM_INDEFINITE_BEEF_NOODLES,          STR_SHOP_ITEM_DISPLAY_BEEF_NOODLES        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          130,              ShopItem::emptyBowlBlue,    PeepThoughtType::BeefNoodlesMuch,       PeepThoughtType::BeefNoodles        },
    /* ShopItem::friedRiceNoodles */ {  0.60_GBP, 1.70_GBP,   1.70_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_FRIED_RICE_NOODLES,  { STR_SHOP_ITEM_PRICE_LABEL_FRIED_RICE_NOODLES,     STR_SHOP_ITEM_SINGULAR_FRIED_RICE_NOODLES,  STR_SHOP_ITEM_PLURAL_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_INDEFINITE_FRIED_RICE_NOODLES,    STR_SHOP_ITEM_DISPLAY_FRIED_RICE_NOODLES  }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          120,              ShopItem::emptyBowlBlue,    PeepThoughtType::FriedRiceNoodlesMuch,  PeepThoughtType::FriedRiceNoodles   },
    /* ShopItem::wontonSoup */       {  0.40_GBP, 1.30_GBP,   1.30_GBP,  1.50_GBP,   1.50_GBP,      SPR_SHOP_ITEM_WONTON_SOUP,         { STR_SHOP_ITEM_PRICE_LABEL_WONTON_SOUP,            STR_SHOP_ITEM_SINGULAR_WONTON_SOUP,         STR_SHOP_ITEM_PLURAL_WONTON_SOUP,           STR_SHOP_ITEM_INDEFINITE_WONTON_SOUP,           STR_SHOP_ITEM_DISPLAY_WONTON_SOUP         }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          100,              ShopItem::emptyBowlRed,     PeepThoughtType::WontonSoupMuch,        PeepThoughtType::WontonSoup         },
    /* ShopItem::meatballSoup */     {  0.50_GBP, 1.40_GBP,   1.40_GBP,  1.60_GBP,   1.50_GBP,      SPR_SHOP_ITEM_MEATBALL_SOUP,       { STR_SHOP_ITEM_PRICE_LABEL_MEATBALL_SOUP,          STR_SHOP_ITEM_SINGULAR_MEATBALL_SOUP,       STR_SHOP_ITEM_PLURAL_MEATBALL_SOUP,         STR_SHOP_ITEM_INDEFINITE_MEATBALL_SOUP,         STR_SHOP_ITEM_DISPLAY_MEATBALL_SOUP       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          110,              ShopItem::emptyBowlRed,     PeepThoughtType::MeatballSoupMuch,      PeepThoughtType::MeatballSoup       },
    /* ShopItem::fruitJuice */       {  0.40_GBP, 1.10_GBP,   1.90_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_FRUIT_JUICE,         { STR_SHOP_ITEM_PRICE_LABEL_FRUIT_JUICE,            STR_SHOP_ITEM_SINGULAR_FRUIT_JUICE,         STR_SHOP_ITEM_PLURAL_FRUIT_JUICE,           STR_SHOP_ITEM_INDEFINITE_FRUIT_JUICE,           STR_SHOP_ITEM_DISPLAY_FRUIT_JUICE         }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          110,              ShopItem::emptyJuiceCup,    PeepThoughtType::FruitJuiceMuch,        PeepThoughtType::FruitJuice         },
    /* ShopItem::soybeanMilk */      {  0.40_GBP, 1.00_GBP,   1.40_GBP,  1.00_GBP,   1.20_GBP,      SPR_SHOP_ITEM_SOYBEAN_MILK,        { STR_SHOP_ITEM_PRICE_LABEL_SOYBEAN_MILK,           STR_SHOP_ITEM_SINGULAR_SOYBEAN_MILK,        STR_SHOP_ITEM_PLURAL_SOYBEAN_MILK,          STR_SHOP_ITEM_INDEFINITE_SOYBEAN_MILK,          STR_SHOP_ITEM_DISPLAY_SOYBEAN_MILK        }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          90,               ShopItem::emptyDrinkCarton, PeepThoughtType::SoybeanMilkMuch,       PeepThoughtType::SoybeanMilk        },
    /* ShopItem::sujeonggwa */       {  0.30_GBP, 1.10_GBP,   1.40_GBP,  1.10_GBP,   1.20_GBP,      SPR_SHOP_ITEM_SUJEONGGWA,          { STR_SHOP_ITEM_PRICE_LABEL_SUJONGKWA,              STR_SHOP_ITEM_SINGULAR_SUJONGKWA,           STR_SHOP_ITEM_PLURAL_SUJONGKWA,             STR_SHOP_ITEM_INDEFINITE_SUJONGKWA,             STR_SHOP_ITEM_DISPLAY_SUJONGKWA           }, SHOP_ITEM_FLAG_IS_DRINK,                                     Litter::Type::rubbish,          100,              ShopItem::emptyDrinkCarton, PeepThoughtType::SujongkwaMuch,         PeepThoughtType::Sujongkwa          },
    /* ShopItem::subSandwich */      {  0.50_GBP, 1.90_GBP,   1.90_GBP,  1.70_GBP,   1.50_GBP,      SPR_SHOP_ITEM_SUB_SANDWICH,        { STR_SHOP_ITEM_PRICE_LABEL_SUB_SANDWICH,           STR_SHOP_ITEM_SINGULAR_SUB_SANDWICH,        STR_SHOP_ITEM_PLURAL_SUB_SANDWICH,          STR_SHOP_ITEM_INDEFINITE_SUB_SANDWICH,          STR_SHOP_ITEM_DISPLAY_SUB_SANDWICH        }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          130,              ShopItem::none,             PeepThoughtType::SubSandwichMuch,       PeepThoughtType::SubSandwich        },
    /* ShopItem::cookie */           {  0.40_GBP, 0.80_GBP,   0.80_GBP,  0.80_GBP,   0.70_GBP,      SPR_SHOP_ITEM_COOKIE,              { STR_SHOP_ITEM_PRICE_LABEL_COOKIE,                 STR_SHOP_ITEM_SINGULAR_COOKIE,              STR_SHOP_ITEM_PLURAL_COOKIE,                STR_SHOP_ITEM_INDEFINITE_COOKIE,                STR_SHOP_ITEM_DISPLAY_COOKIE              }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          75,               ShopItem::none,             PeepThoughtType::CookieMuch,            PeepThoughtType::Cookie             },
    /* ShopItem::emptyBowlRed */     {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOWL_RED,      { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_RED,         STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_RED,      STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_RED,        STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_RED,        STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_RED      }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyBowlRed,     0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::emptyDrinkCarton */ {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_DRINK_CARTON,  { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_DRINK_CARTON,     STR_SHOP_ITEM_SINGULAR_EMPTY_DRINK_CARTON,  STR_SHOP_ITEM_PLURAL_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_INDEFINITE_EMPTY_DRINK_CARTON,    STR_SHOP_ITEM_DISPLAY_EMPTY_DRINK_CARTON  }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyDrinkCarton, 0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::emptyJuiceCup */    {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_JUICE_CUP,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_JUICE_CUP,        STR_SHOP_ITEM_SINGULAR_EMPTY_JUICE_CUP,     STR_SHOP_ITEM_PLURAL_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_INDEFINITE_EMPTY_JUICE_CUP,       STR_SHOP_ITEM_DISPLAY_EMPTY_JUICE_CUP     }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyJuiceCup,    0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
    /* ShopItem::roastSausage */     {  0.50_GBP, 1.60_GBP,   1.60_GBP,  2.00_GBP,   1.50_GBP,      SPR_SHOP_ITEM_ROAST_SAUSAGE,       { STR_SHOP_ITEM_PRICE_LABEL_ROAST_SAUSAGE,          STR_SHOP_ITEM_SINGULAR_ROAST_SAUSAGE,       STR_SHOP_ITEM_PLURAL_ROAST_SAUSAGE,         STR_SHOP_ITEM_INDEFINITE_ROAST_SAUSAGE,         STR_SHOP_ITEM_DISPLAY_ROAST_SAUSAGE       }, SHOP_ITEM_FLAG_IS_FOOD,                                      Litter::Type::rubbish,          115,              ShopItem::none,             PeepThoughtType::RoastSausageMuch,      PeepThoughtType::RoastSausage       },
    /* ShopItem::emptyBowlBlue */    {  0.00_GBP, 0.00_GBP,   0.00_GBP,  0.00_GBP,   0.00_GBP,      SPR_SHOP_ITEM_EMPTY_BOWL_BLUE,     { STR_SHOP_ITEM_PRICE_LABEL_EMPTY_BOWL_BLUE,        STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_BLUE,     STR_SHOP_ITEM_PLURAL_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_INDEFINITE_EMPTY_BOWL_BLUE,       STR_SHOP_ITEM_DISPLAY_EMPTY_BOWL_BLUE     }, SHOP_ITEM_FLAG_IS_CONTAINER,                                 Litter::Type::emptyBowlBlue,    0,                ShopItem::none,             PeepThoughtType::None,                  PeepThoughtType::None               },
};
// clang-format on

constexpr uint64_t GetAllShopItemsWithFlag(uint16_t flag)
{
    uint64_t ret = 0;
    for (auto i = 0; i < EnumValue(ShopItem::count); ++i)
    {
        const auto& sid = ShopItems[i];
        if (sid.HasFlag(flag))
        {
            ret |= (1uLL << i);
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

money64 ShopItemGetCommonPrice(Ride* forRide, const ShopItem shopItem)
{
    auto& gameState = getGameState();
    for (const auto& ride : RideManager(gameState))
    {
        if (&ride != forRide)
        {
            auto rideEntry = ride.getRideEntry();
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
            if (GetShopItemDescriptor(shopItem).IsPhoto() && (ride.lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            {
                return ride.price[1];
            }
        }
    }

    return kMoney64Undefined;
}

bool ShopItemHasCommonPrice(const ShopItem shopItem)
{
    return (getGameState().park.samePriceThroughoutPark & EnumToFlag(shopItem)) != 0;
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
