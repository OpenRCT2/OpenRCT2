#include "Litter.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../localisation/StringIds.h"
#include "EntityList.h"
#include "Map.h"
#include "Sprite.h"

static bool isLocationLitterable(const CoordsXYZ& mapPos)
{
    TileElement* tileElement;

    if (!map_is_location_owned(mapPos))
        return false;

    tileElement = map_get_first_element_at(mapPos);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        int32_t pathZ = tileElement->GetBaseZ();
        if (pathZ < mapPos.z || pathZ >= mapPos.z + PATH_CLEARANCE)
            continue;

        return !tile_element_is_underground(tileElement);
    } while (!(tileElement++)->IsLastForTile());
    return false;
}

/**
 *
 *  rct2: 0x0067375D
 */
void Litter::Create(const CoordsXYZD& litterPos, Type type)
{
    if (gCheatsDisableLittering)
        return;

    auto offsetLitterPos = litterPos
        + CoordsXY{ CoordsDirectionDelta[litterPos.direction >> 3].x / 8,
                    CoordsDirectionDelta[litterPos.direction >> 3].y / 8 };

    if (!isLocationLitterable(offsetLitterPos))
        return;

    if (GetEntityListCount(EntityType::Litter) >= 500)
    {
        Litter* newestLitter = nullptr;
        uint32_t newestLitterCreationTick = 0;
        for (auto litter : EntityList<Litter>())
        {
            if (newestLitterCreationTick <= litter->creationTick)
            {
                newestLitterCreationTick = litter->creationTick;
                newestLitter = litter;
            }
        }

        if (newestLitter != nullptr)
        {
            newestLitter->Invalidate();
            sprite_remove(newestLitter);
        }
    }

    Litter* litter = CreateEntity<Litter>();
    if (litter == nullptr)
        return;

    litter->sprite_direction = offsetLitterPos.direction;
    litter->sprite_width = 6;
    litter->sprite_height_negative = 6;
    litter->sprite_height_positive = 3;
    litter->SubType = type;
    litter->MoveTo(offsetLitterPos);
    litter->creationTick = gCurrentTicks;
}

/**
 *
 *  rct2: 0x006738E1
 */
void Litter::RemoveAt(const CoordsXYZ& litterPos)
{
    std::vector<Litter*> removals;
    for (auto litter : EntityTileList<Litter>(litterPos))
    {
        if (abs(litter->z - litterPos.z) <= 16)
        {
            if (abs(litter->x - litterPos.x) <= 8 && abs(litter->y - litterPos.y) <= 8)
            {
                removals.push_back(litter);
            }
        }
    }
    for (auto* litter : removals)
    {
        litter->Invalidate();
        sprite_remove(litter);
    }
}

static const rct_string_id litterNames[12] = {
    STR_LITTER_VOMIT,
    STR_LITTER_VOMIT,
    STR_SHOP_ITEM_SINGULAR_EMPTY_CAN,
    STR_SHOP_ITEM_SINGULAR_RUBBISH,
    STR_SHOP_ITEM_SINGULAR_EMPTY_BURGER_BOX,
    STR_SHOP_ITEM_SINGULAR_EMPTY_CUP,
    STR_SHOP_ITEM_SINGULAR_EMPTY_BOX,
    STR_SHOP_ITEM_SINGULAR_EMPTY_BOTTLE,
    STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_RED,
    STR_SHOP_ITEM_SINGULAR_EMPTY_DRINK_CARTON,
    STR_SHOP_ITEM_SINGULAR_EMPTY_JUICE_CUP,
    STR_SHOP_ITEM_SINGULAR_EMPTY_BOWL_BLUE,
};

rct_string_id Litter::GetName() const
{
    if (EnumValue(SubType) >= std::size(litterNames))
        return STR_NONE;
    return litterNames[EnumValue(SubType)];
}

uint32_t Litter::GetAge() const
{
    return gCurrentTicks - creationTick;
}
