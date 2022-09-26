#include "Litter.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../core/DataSerialiser.h"
#include "../localisation/StringIds.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../sprites.h"
#include "../world/Map.h"
#include "EntityList.h"
#include "EntityRegistry.h"

template<> bool EntityBase::Is<Litter>() const
{
    return Type == EntityType::Litter;
}

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
        if (tileElement->GetType() != TileElementType::Path)
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
            EntityRemove(newestLitter);
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
        EntityRemove(litter);
    }
}

static const StringId litterNames[12] = {
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

StringId Litter::GetName() const
{
    if (EnumValue(SubType) >= std::size(litterNames))
        return STR_NONE;
    return litterNames[EnumValue(SubType)];
}

uint32_t Litter::GetAge() const
{
    return gCurrentTicks - creationTick;
}

void Litter::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);

    stream << SubType;
    stream << creationTick;
}

struct LitterSprite
{
    uint16_t base_id;
    uint8_t direction_mask;
};

/** rct2: 0x0097EF6C */
static constexpr const LitterSprite _litterSprites[] = {
    { SPR_LITTER_SICK, 0x1 },
    { SPR_LITTER_SICK_ALT, 0x1 },
    { SPR_LITTER_EMPTY_CAN, 0x1 },
    { SPR_LITTER_RUBBISH, 0x1 },
    { SPR_LITTER_EMPTY_BURGER_BOX, 0x1 },
    { SPR_LITTER_EMPTY_CUP, 0x1 },
    { SPR_LITTER_EMPTY_BOX, 0x1 },
    { SPR_LITTER_EMPTY_BOTTLE, 0x1 },
    { SPR_LITTER_EMPTY_BOWL_RED, 0x3 },
    { SPR_LITTER_EMPTY_DRINK_CART, 0x3 },
    { SPR_LITTER_EMPTY_JUICE_CUP, 0x3 },
    { SPR_LITTER_EMPTY_BOWL_BLUE, 0x3 },
};

void Litter::Paint(paint_session& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    rct_drawpixelinfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 0 })
        return; // If zoomed at all no litter drawn

    // litter has no sprite direction so remove that
    imageDirection >>= 3;
    // Some litter types have only 1 direction so remove
    // anything that isn't required.
    imageDirection &= _litterSprites[EnumValue(SubType)].direction_mask;

    uint32_t image_id = imageDirection + _litterSprites[EnumValue(SubType)].base_id;

    // In the following call to PaintAddImageAsParent, we add 4 (instead of 2) to the
    // bound_box_offset_z to make sure litter is drawn on top of railways
    PaintAddImageAsParent(session, image_id, { 0, 0, z }, { 4, 4, -1 }, { -4, -4, z + 4 });
}
