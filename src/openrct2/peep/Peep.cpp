/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <limits>

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../Cheats.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../Input.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/LargeScenery.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "Peep.h"
#include "Staff.h"

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
bool gPathFindDebug = false;
utf8 gPathFindDebugPeepName[256];
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

uint8_t  gGuestChangeModifier;
uint16_t gNumGuestsInPark;
uint16_t gNumGuestsInParkLastWeek;
uint16_t gNumGuestsHeadingForPark;

money16 gGuestInitialCash;
uint8_t   gGuestInitialHappiness;
uint8_t   gGuestInitialHunger;
uint8_t   gGuestInitialThirst;

uint32_t gNextGuestNumber;

uint8_t gPeepWarningThrottle[16];

TileCoordsXYZ gPeepPathFindGoalPosition;
bool          gPeepPathFindIgnoreForeignQueues;
uint8_t         gPeepPathFindQueueRideIndex;
// uint32_t gPeepPathFindAltStationNum;


static uint8_t             _unk_F1AEF0;
static rct_tile_element * _peepRideEntranceExitElement;

static void * _crowdSoundChannel = nullptr;

static void   peep_128_tick_update(rct_peep * peep, int32_t index);
static void   peep_easter_egg_peep_interactions(rct_peep * peep);
static void   peep_give_real_name(rct_peep * peep);
static void   peep_release_balloon(rct_peep * peep, int16_t spawn_height);

// clang-format off
static constexpr const char *gPeepEasterEggNames[] = {
    "MICHAEL SCHUMACHER",
    "JACQUES VILLENEUVE",
    "DAMON HILL",
    "MR BEAN",
    "CHRIS SAWYER",
    "KATIE BRAYSHAW",
    "MELANIE WARN",
    "SIMON FOSTER",
    "JOHN WARDLEY",
    "LISA STIRLING",
    "DONALD MACRAE",
    "KATHERINE MCGOWAN",
    "FRANCES MCGOWAN",
    "CORINA MASSOURA",
    "CAROL YOUNG",
    "MIA SHERIDAN",
    "KATIE RODGER",
    "EMMA GARRELL",
    "JOANNE BARTON",
    "FELICITY ANDERSON",
    "KATIE SMITH",
    "EILIDH BELL",
    "NANCY STILLWAGON",
    "DAVID ELLIS"
};

/** rct2: 0x00981DB0 */
static struct
{
    uint8_t action;
    uint8_t flags;
} PeepThoughtToActionMap[] = {
    { PEEP_ACTION_SHAKE_HEAD, 1 },
    { PEEP_ACTION_EMPTY_POCKETS, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_WOW, 1 },
    { PEEP_ACTION_NONE_2, 2 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 4 },
    { PEEP_ACTION_SHAKE_HEAD, 4 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 0 },
    { PEEP_ACTION_JOY, 1 },
    { PEEP_ACTION_CHECK_TIME, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_WAVE, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_DISGUST, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_BEING_WATCHED, 0 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 1 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_SHAKE_HEAD, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_NONE_2, 0 },
    { PEEP_ACTION_JOY, 0 },
    { PEEP_ACTION_NONE_2, 1 },
};

static uint8_t PeepSpecialSpriteToSpriteTypeMap[] = {
    PEEP_ACTION_SPRITE_TYPE_NONE,
    PEEP_ACTION_SPRITE_TYPE_HOLD_MAT,
    PEEP_ACTION_SPRITE_TYPE_STAFF_MOWER
};

static uint8_t PeepActionToSpriteTypeMap[] = {
    PEEP_ACTION_SPRITE_TYPE_CHECK_TIME,
    PEEP_ACTION_SPRITE_TYPE_EAT_FOOD,
    PEEP_ACTION_SPRITE_TYPE_SHAKE_HEAD,
    PEEP_ACTION_SPRITE_TYPE_EMPTY_POCKETS,
    PEEP_ACTION_SPRITE_TYPE_SITTING_EAT_FOOD,
    PEEP_ACTION_SPRITE_TYPE_SITTING_LOOK_AROUND_LEFT,
    PEEP_ACTION_SPRITE_TYPE_SITTING_LOOK_AROUND_RIGHT,
    PEEP_ACTION_SPRITE_TYPE_WOW,
    PEEP_ACTION_SPRITE_TYPE_THROW_UP,
    PEEP_ACTION_SPRITE_TYPE_JUMP,
    PEEP_ACTION_SPRITE_TYPE_STAFF_SWEEP,
    PEEP_ACTION_SPRITE_TYPE_DROWNING,
    PEEP_ACTION_SPRITE_TYPE_STAFF_ANSWER_CALL,
    PEEP_ACTION_SPRITE_TYPE_STAFF_ANSWER_CALL_2,
    PEEP_ACTION_SPRITE_TYPE_STAFF_CHECKBOARD,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_2,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_GROUND,
    PEEP_ACTION_SPRITE_TYPE_STAFF_FIX_3,
    PEEP_ACTION_SPRITE_TYPE_STAFF_WATERING,
    PEEP_ACTION_SPRITE_TYPE_JOY,
    PEEP_ACTION_SPRITE_TYPE_READ_MAP,
    PEEP_ACTION_SPRITE_TYPE_WAVE,
    PEEP_ACTION_SPRITE_TYPE_STAFF_EMPTY_BIN,
    PEEP_ACTION_SPRITE_TYPE_WAVE_2,
    PEEP_ACTION_SPRITE_TYPE_TAKE_PHOTO,
    PEEP_ACTION_SPRITE_TYPE_CLAP,
    PEEP_ACTION_SPRITE_TYPE_DISGUST,
    PEEP_ACTION_SPRITE_TYPE_DRAW_PICTURE,
    PEEP_ACTION_SPRITE_TYPE_BEING_WATCHED,
    PEEP_ACTION_SPRITE_TYPE_WITHDRAW_MONEY
};

const bool gSpriteTypeToSlowWalkMap[] = {
    false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, true,  true,
    true,  true,  true,  false, true,  false, true,  true,
    true,  false, false, true,  true,  false, false, true,
    true,  true,  true,  true,  true,  true,  false, true,
    false, true,  true,  true,  true,  true,  true,  true,
};

// clang-format on

bool rct_sprite::IsPeep()
{
    return peep.sprite_identifier == SPRITE_IDENTIFIER_PEEP;
}

rct_peep * rct_sprite::AsPeep()
{
    rct_peep * result = nullptr;
    if (IsPeep())
    {
        return (rct_peep *)this;
    }
    return result;
}

void rct_peep::Invalidate()
{
    invalidate_sprite_2((rct_sprite*)this);
}

void rct_peep::MoveTo(int16_t destX, int16_t destY, int16_t destZ)
{
    sprite_move(destX, destY, destZ, (rct_sprite*)this);
}

uint8_t rct_peep::GetNextDirection() const
{
    return next_flags & PEEP_NEXT_FLAG_DIRECTION_MASK;
}

bool rct_peep::GetNextIsSloped() const
{
    return next_flags & PEEP_NEXT_FLAG_IS_SLOPED;
}

bool rct_peep::GetNextIsSurface() const
{
    return next_flags & PEEP_NEXT_FLAG_IS_SURFACE;
}

void rct_peep::SetNextFlags(uint8_t next_direction, bool is_sloped, bool is_surface)
{
    next_flags = next_direction & PEEP_NEXT_FLAG_DIRECTION_MASK;
    next_flags |= is_sloped ? PEEP_NEXT_FLAG_IS_SLOPED : 0;
    next_flags |= is_surface ? PEEP_NEXT_FLAG_IS_SURFACE : 0;
}

rct_peep * try_get_guest(uint16_t spriteIndex)
{
    rct_sprite * sprite = try_get_sprite(spriteIndex);
    if (sprite == nullptr)
        return nullptr;
    if (sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_PEEP)
        return nullptr;
    if (sprite->peep.type != PEEP_TYPE_GUEST)
        return nullptr;
    return &sprite->peep;
}

int32_t peep_get_staff_count()
{
    uint16_t     spriteIndex;
    rct_peep * peep;
    int32_t     count = 0;

    FOR_ALL_STAFF(spriteIndex, peep)
    count++;

    return count;
}

/**
 *
 *  rct2: 0x0068F0A9
 */
void peep_update_all()
{
    int32_t     i;
    uint16_t     spriteIndex;
    rct_peep * peep;

    if (gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        return;

    spriteIndex = gSpriteListHead[SPRITE_LIST_PEEP];
    i           = 0;
    while (spriteIndex != SPRITE_INDEX_NULL)
    {
        peep        = &(get_sprite(spriteIndex)->peep);
        spriteIndex = peep->next;

        if ((uint32_t)(i & 0x7F) != (gCurrentTicks & 0x7F))
        {
            peep->Update();
        }
        else
        {
            peep_128_tick_update(peep, i);
            if (peep->linked_list_type_offset == SPRITE_LIST_PEEP * 2)
            {
                peep->Update();
            }
        }

        i++;
    }
}

/**
 *
 *  rct2: 0x0068F41A
 *  Called every 128 ticks
 */
static void peep_128_tick_update(rct_peep * peep, int32_t index)
{
    if (peep->type == PEEP_TYPE_STAFF)
    {
        peep->Tick128UpdateStaff();
    }
    else
    {
        peep->Tick128UpdateGuest(index);
    }
}

/*
 * rct2: 0x68F3AE
 * Set peep state to falling if path below has gone missing, return true if current path is valid, false if peep starts falling.
 */
bool rct_peep::CheckForPath()
{
    path_check_optimisation++;
    if ((path_check_optimisation & 0xF) != (sprite_index & 0xF))
    {
        // This condition makes the check happen less often
        // As a side effect peeps hover for a short,
        // random time when a path below them has been deleted
        return true;
    }

    rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

    uint8_t map_type = TILE_ELEMENT_TYPE_PATH;
    if (GetNextIsSurface())
    {
        map_type = TILE_ELEMENT_TYPE_SURFACE;
    }

    int32_t height = next_z;

    do
    {
        if (tile_element->GetType() == map_type)
        {
            if (height == tile_element->base_height)
            {
                // Found a suitable path or surface
                return true;
            }
        }
    } while (!(tile_element++)->IsLastForTile());

    // Found no suitable path
    SetState(PEEP_STATE_FALLING);
    return false;
}

uint8_t rct_peep::GetActionSpriteType()
{
    if (action >= PEEP_ACTION_NONE_1)
    { // PEEP_ACTION_NONE_1 or PEEP_ACTION_NONE_2
        return PeepSpecialSpriteToSpriteTypeMap[special_sprite];
    }
    else if (action < Util::CountOf(PeepActionToSpriteTypeMap))
    {
        return PeepActionToSpriteTypeMap[action];
    }
    else
    {
        openrct2_assert(action >= Util::CountOf(PeepActionToSpriteTypeMap) && action < PEEP_ACTION_NONE_1,
                        "Invalid peep action %u", action);
        return 0;
    }
}

/*
 *  rct2: 0x00693B58
 */
void rct_peep::UpdateCurrentActionSpriteType()
{
    if (sprite_type >= Util::CountOf(g_peep_animation_entries))
    {
        return;
    }
    uint8_t newActionSpriteType = GetActionSpriteType();
    if (action_sprite_type == newActionSpriteType)
    {
        return;
    }

    Invalidate();
    action_sprite_type = newActionSpriteType;

    const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[sprite_type].sprite_bounds;
    sprite_width                     = spriteBounds[action_sprite_type].sprite_width;
    sprite_height_negative           = spriteBounds[action_sprite_type].sprite_height_negative;
    sprite_height_positive           = spriteBounds[action_sprite_type].sprite_height_positive;

    Invalidate();
}

/* rct2: 0x00693BE5 */
void rct_peep::SwitchToSpecialSprite(uint8_t special_sprite_id)
{
    if (special_sprite_id == special_sprite)
        return;

    special_sprite = special_sprite_id;

    // If NONE_1 or NONE_2
    if (action >= PEEP_ACTION_NONE_1)
    {
        action_sprite_image_offset = 0;
    }
    UpdateCurrentActionSpriteType();
}

void rct_peep::StateReset()
{
    SetState(PEEP_STATE_1);
    SwitchToSpecialSprite(0);
}

/** rct2: 0x00981D7C, 0x00981D7E */
static constexpr const LocationXY16 word_981D7C[4] = { { -2, 0 }, { 0, 2 }, { 2, 0 }, { 0, -2 } };

bool rct_peep::UpdateAction()
{
    int16_t actionX, actionY, xy_distance;
    return UpdateAction(&actionX, &actionY, &xy_distance);
}

/**
 *
 *  rct2: 0x6939EB
 * Also used to move peeps to the correct position to
 * start an action. Returns true if the correct destination
 * has not yet been reached. xy_distance is how close the
 * peep is to the target.
 */
bool rct_peep::UpdateAction(int16_t * actionX, int16_t * actionY, int16_t * xy_distance)
{
    _unk_F1AEF0 = action_sprite_image_offset;
    if (action == PEEP_ACTION_NONE_1)
    {
        action = PEEP_ACTION_NONE_2;
    }

    *actionX = x - destination_x;
    *actionY = y - destination_y;

    int32_t x_delta = abs(*actionX);
    int32_t y_delta = abs(*actionY);

    *xy_distance = x_delta + y_delta;

    if (action == PEEP_ACTION_NONE_1 || action == PEEP_ACTION_NONE_2)
    {
        if (*xy_distance <= destination_tolerance)
        {
            return false;
        }
        int32_t nextDirection = 0;
        if (x_delta < y_delta)
        {
            nextDirection = 8;
            if (*actionY >= 0)
            {
                nextDirection = 24;
            }
        }
        else
        {
            nextDirection = 16;
            if (*actionX >= 0)
            {
                nextDirection = 0;
            }
        }
        sprite_direction = nextDirection;
        *actionX                     = x + word_981D7C[nextDirection / 8].x;
        *actionY                     = y + word_981D7C[nextDirection / 8].y;
        no_action_frame_num++;
        const rct_peep_animation * peepAnimation = g_peep_animation_entries[sprite_type].sprite_animation;
        const uint8_t *              imageOffset   = peepAnimation[action_sprite_type].frame_offsets;
        if (no_action_frame_num >= peepAnimation[action_sprite_type].num_frames)
        {
            no_action_frame_num = 0;
        }
        action_sprite_image_offset = imageOffset[no_action_frame_num];
        return true;
    }

    const rct_peep_animation * peepAnimation = g_peep_animation_entries[sprite_type].sprite_animation;
    action_frame++;

    // If last frame of action
    if (action_frame >= peepAnimation[action_sprite_type].num_frames)
    {
        action_sprite_image_offset = 0;
        action                     = 0xFF;
        UpdateCurrentActionSpriteType();
        Invalidate();
        *actionX = x;
        *actionY = y;
        return true;
    }
    action_sprite_image_offset = peepAnimation[action_sprite_type].frame_offsets[action_frame];

    // If not throwing up and not at the frame where sick appears.
    if (action != PEEP_ACTION_THROW_UP || action_frame != 15)
    {
        Invalidate();
        *actionX = x;
        *actionY = y;
        return true;
    }

    // We are throwing up
    hunger /= 2;
    nausea_target /= 2;

    if (nausea < 30)
        nausea = 0;
    else
        nausea -= 30;

    window_invalidate_flags |= PEEP_INVALIDATE_PEEP_2;

    // Create sick at location
    litter_create(x, y, z, sprite_direction,
                  (sprite_index & 1) ? LITTER_TYPE_SICK_ALT : LITTER_TYPE_SICK);

    int32_t sound_id = SOUND_COUGH_1 + (scenario_rand() & 3);
    audio_play_sound_at_location(sound_id, x, y, z);

    Invalidate();
    *actionX = x;
    *actionY = y;
    return true;
}

/**
 *  rct2: 0x0069A409
 * Decreases rider count if on/entering a ride.
 */
void peep_decrement_num_riders(rct_peep * peep)
{
    if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
    {

        Ride * ride = get_ride(peep->current_ride);
        ride->num_riders = std::max(0, ride->num_riders - 1);
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    }
}

/**
 * Call after changing a peeps state to insure that all relevant windows update.
 * Note also increase ride count if on/entering a ride.
 *  rct2: 0x0069A42F
 */
void peep_window_state_update(rct_peep * peep)
{
    rct_window * w = window_find_by_number(WC_PEEP, peep->sprite_index);
    if (w != nullptr)
        window_event_invalidate_call(w);

    if (peep->type == PEEP_TYPE_GUEST)
    {
        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
        {
            Ride * ride = get_ride(peep->current_ride);
            ride->num_riders++;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
        }

        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_GUEST_LIST);
    }
    else
    {
        window_invalidate_by_number(WC_PEEP, peep->sprite_index);
        window_invalidate_by_class(WC_STAFF_LIST);
    }
}

void rct_peep::Pickup()
{
    RemoveFromRide();
    Invalidate();

    sprite_move(LOCATION_NULL, y, z, (rct_sprite *)this);
    SetState(PEEP_STATE_PICKED);
    sub_state = 0;
}

void rct_peep::PickupAbort(int32_t old_x)
{
    if (state != PEEP_STATE_PICKED)
        return;

    sprite_move(old_x, y, z + 8, (rct_sprite *)this);
    Invalidate();

    if (x != (int16_t)LOCATION_NULL)
    {
        SetState(PEEP_STATE_FALLING);
        action                     = 0xFF;
        special_sprite             = 0;
        action_sprite_image_offset = 0;
        action_sprite_type         = 0;
        path_check_optimisation    = 0;
    }

    gPickupPeepImage = UINT32_MAX;
}

// Returns true when a peep can be dropped at the given location. When apply is set to true the peep gets dropped.
bool rct_peep::Place(TileCoordsXYZ location, bool apply)
{
    rct_tile_element * tileElement = map_get_path_element_at(location.x, location.y, location.z);

    if (!tileElement)
    {
        tileElement = map_get_surface_element_at(location.x, location.y);
    }

    if (!tileElement)
        return false;

    CoordsXYZ destination = { location.x * 32, location.y * 32, location.z * 8 };

    // Set the coordinate of destination to be exactly
    // in the middle of a tile.
    destination.x += 16;
    destination.y += 16;
    destination.z = tileElement->base_height * 8 + 16;

    if (!map_is_location_owned(location.x * 32, location.y * 32, destination.z))
    {
        gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
        return false;
    }

    if (!map_can_construct_at(destination.x, destination.y, destination.z / 8, (destination.z / 8) + 1, 15))
    {
        if (gGameCommandErrorText != STR_RAISE_OR_LOWER_LAND_FIRST)
        {
            if (gGameCommandErrorText != STR_FOOTPATH_IN_THE_WAY)
            {
                gGameCommandErrorTitle = STR_ERR_CANT_PLACE_PERSON_HERE;
                return false;
            }
        }
    }

    if (apply)
    {
        sprite_move(destination.x, destination.y, destination.z, (rct_sprite *)this);
        Invalidate();
        SetState(PEEP_STATE_FALLING);
        action                     = 0xFF;
        special_sprite             = 0;
        action_sprite_image_offset = 0;
        action_sprite_type         = 0;
        path_check_optimisation    = 0;
        sprite_position_tween_reset();

        if (type == PEEP_TYPE_GUEST)
        {
            action_sprite_type = 0xFF;
            happiness_target   = std::max(happiness_target - 10, 0);
            UpdateCurrentActionSpriteType();
        }

        network_set_pickup_peep(game_command_playerid, nullptr);
    }

    return true;
}

bool peep_pickup_command(uint32_t peepnum, int32_t x, int32_t y, int32_t z, int32_t action, bool apply)
{
    if (peepnum >= MAX_SPRITES)
    {
        log_error("Failed to pick up peep for sprite %d", peepnum);
        return false;
    }

    rct_peep * const peep = GET_PEEP(peepnum);
    if (!peep || peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP)
    {
        return false;
    }

    switch (action)
    {
    case 0: // pickup
    {
        if (!peep_can_be_picked_up(peep))
        {
            return false;
        }
        rct_peep * existing = network_get_pickup_peep(game_command_playerid);
        if (existing)
        {
            // already picking up a peep
            bool result = peep_pickup_command(existing->sprite_index, network_get_pickup_peep_old_x(game_command_playerid), 0,
                                              0, 1, apply);
            if (existing == peep)
            {
                return result;
            }
            if (game_command_playerid == network_get_current_player_id())
            {
                // prevent tool_cancel()
                input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);
            }
        }

        if (apply)
        {
            network_set_pickup_peep(game_command_playerid, peep);
            network_set_pickup_peep_old_x(game_command_playerid, peep->x);
            peep->Pickup();
        }
    }
    break;
    case 1: // cancel
        if (apply)
        {
            // TODO: Verify if this is really needed or that we can use `peep` instead
            rct_peep * const pickedUpPeep = network_get_pickup_peep(game_command_playerid);
            if (pickedUpPeep)
            {
                pickedUpPeep->PickupAbort(x);
            }

            network_set_pickup_peep(game_command_playerid, nullptr);
        }
        break;
    case 2: // place
        if (network_get_pickup_peep(game_command_playerid) != peep)
        {
            return false;
        }

        if (!peep->Place({ x / 32, y / 32, z }, apply))
        {
            return false;
        }
        break;
    }
    return true;
}

void game_command_pickup_guest(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    int32_t peepnum = *eax;
    int32_t x       = *edi;
    int32_t y       = *ebp;
    int32_t z       = *edx;
    int32_t action  = *ecx;
    if (peep_pickup_command(peepnum, x, y, z, action, *ebx & GAME_COMMAND_FLAG_APPLY))
    {
        *ebx = 0;
    }
    else
    {
        *ebx = MONEY32_UNDEFINED;
    }
}

/**
 *
 *  rct2: 0x0069A535
 */
void peep_sprite_remove(rct_peep * peep)
{
    peep->RemoveFromRide();
    peep->Invalidate();

    window_close_by_number(WC_PEEP, peep->sprite_index);

    window_close_by_number(WC_FIRE_PROMPT, peep->sprite_identifier);

    if (peep->type == PEEP_TYPE_GUEST)
    {
        window_invalidate_by_class(WC_GUEST_LIST);

        news_item_disable_news(NEWS_ITEM_PEEP_ON_RIDE, peep->sprite_index);
    }
    else
    {
        window_invalidate_by_class(WC_STAFF_LIST);

        gStaffModes[peep->staff_id] = 0;
        peep->type                  = 0xFF;
        staff_update_greyed_patrol_areas();
        peep->type = PEEP_TYPE_STAFF;

        news_item_disable_news(NEWS_ITEM_PEEP, peep->sprite_index);
    }
    sprite_remove((rct_sprite *)peep);
}

/**
 * New function removes peep from park existence. Works with staff.
 */
void rct_peep::Remove()
{
    if (type == PEEP_TYPE_GUEST)
    {
        if (outside_of_park == 0)
        {
            decrement_guests_in_park();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            context_broadcast_intent(&intent);
        }
        if (state == PEEP_STATE_ENTERING_PARK)
        {
            decrement_guests_heading_for_park();
        }
    }
    peep_sprite_remove(this);
}

/**
 * Falling and its subset drowning
 *  rct2: 0x690028
 */
void rct_peep::UpdateFalling()
{
    if (action == PEEP_ACTION_DROWNING)
    {
        // Check to see if we are ready to drown.
        int16_t actionX, actionY, xy_distance;

        UpdateAction(&actionX, &actionY, &xy_distance);
        if (action == PEEP_ACTION_DROWNING)
            return;

        if (gConfigNotifications.guest_died)
        {
            set_format_arg(0, rct_string_id, name_string_idx);
            set_format_arg(2, uint32_t, id);
            news_item_add_to_queue(NEWS_ITEM_BLANK, STR_NEWS_ITEM_GUEST_DROWNED, actionX | (actionY << 16));
        }

        gParkRatingCasualtyPenalty = std::min(gParkRatingCasualtyPenalty + 25, 1000);
        Remove();
        return;
    }

    // If not drowning then falling. Note: peeps 'fall' after leaving a ride/enter the park.
    rct_tile_element * tile_element  = map_get_first_element_at(x / 32, y / 32);
    rct_tile_element * saved_map    = nullptr;
    int32_t            saved_height = 0;

    if (tile_element != nullptr)
    {
        do
        {
            // If a path check if we are on it
            if (tile_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                int32_t height = map_height_from_slope(x, y, tile_element->properties.surface.slope) +
                                tile_element->base_height * 8;

                if (height < z - 1 || height > z + 4)
                    continue;

                saved_height = height;
                saved_map    = tile_element;
                break;
            } // If a surface get the height and see if we are on it
            else if (tile_element->GetType() == TILE_ELEMENT_TYPE_SURFACE)
            {
                // If the surface is water check to see if we could be drowning
                if (surface_get_water_height(tile_element) > 0)
                {
                    int32_t height = surface_get_water_height(tile_element) * 16;

                    if (height - 4 >= z && height < z + 20)
                    {
                        // Looks like we are drowning!
                        Invalidate();
                        MoveTo(x, y, height);
                        // Drop balloon if held
                        peep_release_balloon(this, height);

                        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_DROWNING, PEEP_THOUGHT_ITEM_NONE);

                        action                     = PEEP_ACTION_DROWNING;
                        action_frame               = 0;
                        action_sprite_image_offset = 0;

                        UpdateCurrentActionSpriteType();
                        Invalidate();
                        peep_window_state_update(this);
                        return;
                    }
                }
                int32_t map_height = tile_element_height(0xFFFF & x, 0xFFFF & y) & 0xFFFF;
                if (map_height < z || map_height - 4 > z)
                    continue;
                saved_height = map_height;
                saved_map    = tile_element;
            } // If not a path or surface go see next element
            else
                continue;
        } while (!(tile_element++)->IsLastForTile());
    }

    // This will be null if peep is falling
    if (saved_map == nullptr)
    {
        Invalidate();
        if (z <= 1)
        {
            // Remove peep if it has gone to the void
            Remove();
            return;
        }
        MoveTo(x, y, z - 2);
        Invalidate();
        return;
    }

    Invalidate();
    MoveTo(x, y, saved_height);
    Invalidate();

    next_x = x & 0xFFE0;
    next_y = y & 0xFFE0;
    next_z = saved_map->base_height;

    if (saved_map->GetType() != TILE_ELEMENT_TYPE_PATH)
    {
        SetNextFlags(0, false, true);
    }
    else
    {
        SetNextFlags(saved_map->properties.path.type & 0x3, saved_map->properties.path.type & 0x4, false);
    }
    SetState(PEEP_STATE_1);
}

/**
 *
 *  rct2: 0x6902A2
 */
void rct_peep::Update1()
{
    if (!CheckForPath())
        return;

    if (type == PEEP_TYPE_GUEST)
    {
        SetState(PEEP_STATE_WALKING);
    }
    else
    {
        SetState(PEEP_STATE_PATROLLING);
    }

    destination_x         = x;
    destination_y         = y;
    destination_tolerance = 10;
    direction             = sprite_direction >> 3;
}

void rct_peep::SetState(uint8_t new_state)
{
    peep_decrement_num_riders(this);
    state = new_state;
    peep_window_state_update(this);
}

/**
 *
 *  rct2: 0x690009
 */
void rct_peep::UpdatePicked()
{
    if (gCurrentTicks & 0x1F)
        return;
    sub_state++;
    if (sub_state == 13)
    {
        peep_insert_new_thought(this, PEEP_THOUGHT_TYPE_HELP, PEEP_THOUGHT_ITEM_NONE);
    }
}

/* From peep_update */
static void peep_update_thoughts(rct_peep * peep)
{
    // Thoughts must always have a gap of at least
    // 220 ticks in age between them. In order to
    // allow this when a thought is new it enters
    // a holding zone. Before it becomes fresh.
    int32_t add_fresh     = 1;
    int32_t fresh_thought = -1;
    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; i++)
    {
        if (peep->thoughts[i].type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (peep->thoughts[i].freshness == 1)
        {
            add_fresh = 0;
            // If thought is fresh we wait 220 ticks
            // before allowing a new thought to become fresh.
            if (++peep->thoughts[i].fresh_timeout >= 220)
            {
                peep->thoughts[i].fresh_timeout = 0;
                // Thought is no longer fresh
                peep->thoughts[i].freshness++;
                add_fresh = 1;
            }
        }
        else if (peep->thoughts[i].freshness > 1)
        {
            if (++peep->thoughts[i].fresh_timeout == 0)
            {
                // When thought is older than ~6900 ticks remove it
                if (++peep->thoughts[i].freshness >= 28)
                {
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;

                    // Clear top thought, push others up
                    if (i < PEEP_MAX_THOUGHTS - 2)
                    {
                        memmove(&peep->thoughts[i], &peep->thoughts[i + 1],
                                sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
                    }
                    peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                }
            }
        }
        else
        {
            fresh_thought = i;
        }
    }
    // If there are no fresh thoughts
    // a previously new thought can become
    // fresh.
    if (add_fresh && fresh_thought != -1)
    {
        peep->thoughts[fresh_thought].freshness = 1;
        peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
    }
}

/**
 *
 *  rct2: 0x0068FC1E
 */
void rct_peep::Update()
{
    if (type == PEEP_TYPE_GUEST)
    {
        if (previous_ride != 255)
            if (++previous_ride_time_out >= 720)
                previous_ride = 255;

        peep_update_thoughts(this);
    }

    // Walking speed logic
    uint32_t stepsToTake = energy;
    if (stepsToTake < 95 && state == PEEP_STATE_QUEUING)
        stepsToTake = 95;
    if ((peep_flags & PEEP_FLAGS_SLOW_WALK) && state != PEEP_STATE_QUEUING)
        stepsToTake /= 2;
    if (action == 255 && (GetNextIsSloped()))
    {
        stepsToTake /= 2;
        if (state == PEEP_STATE_QUEUING)
            stepsToTake += stepsToTake / 2;
    }

    uint32_t carryCheck = step_progress + stepsToTake;
    step_progress      = carryCheck;
    if (carryCheck <= 255)
    {
        peep_easter_egg_peep_interactions(this);
    }
    else
    {
        // loc_68FD2F
        switch (state)
        {
        case PEEP_STATE_FALLING:
            UpdateFalling();
            break;
        case PEEP_STATE_1:
            Update1();
            break;
        case PEEP_STATE_QUEUING_FRONT:
            UpdateRide();
            break;
        case PEEP_STATE_ON_RIDE:
            // No action
            break;
        case PEEP_STATE_LEAVING_RIDE:
            UpdateRide();
            break;
        case PEEP_STATE_WALKING:
            UpdateWalking();
            break;
        case PEEP_STATE_QUEUING:
            UpdateQueuing();
            break;
        case PEEP_STATE_ENTERING_RIDE:
            UpdateRide();
            break;
        case PEEP_STATE_SITTING:
            UpdateSitting();
            break;
        case PEEP_STATE_PICKED:
            UpdatePicked();
            break;
        case PEEP_STATE_PATROLLING:
            UpdatePatrolling();
            break;
        case PEEP_STATE_MOWING:
            UpdateMowing();
            break;
        case PEEP_STATE_SWEEPING:
            UpdateSweeping();
            break;
        case PEEP_STATE_ENTERING_PARK:
            UpdateEnteringPark();
            break;
        case PEEP_STATE_LEAVING_PARK:
            UpdateLeavingPark();
            break;
        case PEEP_STATE_ANSWERING:
            UpdateAnswering();
            break;
        case PEEP_STATE_FIXING:
            UpdateFixing(stepsToTake);
            break;
        case PEEP_STATE_BUYING:
            UpdateBuying();
            break;
        case PEEP_STATE_WATCHING:
            UpdateWatching();
            break;
        case PEEP_STATE_EMPTYING_BIN:
            UpdateEmptyingBin();
            break;
        case PEEP_STATE_USING_BIN:
            UpdateUsingBin();
            break;
        case PEEP_STATE_WATERING:
            UpdateWatering();
            break;
        case PEEP_STATE_HEADING_TO_INSPECTION:
            UpdateHeadingToInspect();
            break;
        case PEEP_STATE_INSPECTING:
            UpdateFixing(stepsToTake);
            break;
            // There shouldn't be any more
        default:
            assert(0);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x0069BF41
 */
void peep_problem_warnings_update()
{
    rct_peep * peep;
    Ride *     ride;
    uint16_t     spriteIndex;
    uint16_t     guests_in_park = gNumGuestsInPark;
    int32_t     hunger_counter = 0, lost_counter = 0, noexit_counter = 0, thirst_counter = 0, litter_counter = 0,
           disgust_counter = 0, bathroom_counter = 0, vandalism_counter = 0;
    uint8_t * warning_throttle = gPeepWarningThrottle;

    gRideCount = ride_get_count(); // refactor this to somewhere else

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->outside_of_park != 0 || peep->thoughts[0].freshness > 5)
            continue;

        switch (peep->thoughts[0].type)
        {
        case PEEP_THOUGHT_TYPE_LOST: // 0x10
            lost_counter++;
            break;

        case PEEP_THOUGHT_TYPE_HUNGRY: // 0x14
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                hunger_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                hunger_counter++;
            break;

        case PEEP_THOUGHT_TYPE_THIRSTY:
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                thirst_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SELLS_DRINKS))
                thirst_counter++;
            break;

        case PEEP_THOUGHT_TYPE_BATHROOM:
            if (peep->guest_heading_to_ride_id == 0xFF)
            {
                bathroom_counter++;
                break;
            }
            ride = get_ride(peep->guest_heading_to_ride_id);
            if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_BATHROOM))
                bathroom_counter++;
            break;

        case PEEP_THOUGHT_TYPE_BAD_LITTER: // 0x1a
            litter_counter++;
            break;
        case PEEP_THOUGHT_TYPE_CANT_FIND_EXIT: // 0x1b
            noexit_counter++;
            break;
        case PEEP_THOUGHT_TYPE_PATH_DISGUSTING: // 0x1f
            disgust_counter++;
            break;
        case PEEP_THOUGHT_TYPE_VANDALISM: // 0x21
            vandalism_counter++;
            break;
        default:
            break;
        }
    }
    // could maybe be packed into a loop, would lose a lot of clarity though
    if (warning_throttle[0])
        --warning_throttle[0];
    else if (hunger_counter >= PEEP_HUNGER_WARNING_THRESHOLD && hunger_counter >= guests_in_park / 16)
    {
        warning_throttle[0] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_HUNGRY, 20);
        }
    }

    if (warning_throttle[1])
        --warning_throttle[1];
    else if (thirst_counter >= PEEP_THIRST_WARNING_THRESHOLD && thirst_counter >= guests_in_park / 16)
    {
        warning_throttle[1] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_ARE_THIRSTY, 21);
        }
    }

    if (warning_throttle[2])
        --warning_throttle[2];
    else if (bathroom_counter >= PEEP_BATHROOM_WARNING_THRESHOLD && bathroom_counter >= guests_in_park / 16)
    {
        warning_throttle[2] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_CANT_FIND_BATHROOM, 22);
        }
    }

    if (warning_throttle[3])
        --warning_throttle[3];
    else if (litter_counter >= PEEP_LITTER_WARNING_THRESHOLD && litter_counter >= guests_in_park / 32)
    {
        warning_throttle[3] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_LITTER, 26);
        }
    }

    if (warning_throttle[4])
        --warning_throttle[4];
    else if (disgust_counter >= PEEP_DISGUST_WARNING_THRESHOLD && disgust_counter >= guests_in_park / 32)
    {
        warning_throttle[4] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISGUSTED_BY_PATHS, 31);
        }
    }

    if (warning_throttle[5])
        --warning_throttle[5];
    else if (vandalism_counter >= PEEP_VANDALISM_WARNING_THRESHOLD && vandalism_counter >= guests_in_park / 32)
    {
        warning_throttle[5] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_DISLIKE_VANDALISM, 33);
        }
    }

    if (warning_throttle[6])
        --warning_throttle[6];
    else if (noexit_counter >= PEEP_NOEXIT_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 27);
        }
    }
    else if (lost_counter >= PEEP_LOST_WARNING_THRESHOLD)
    {
        warning_throttle[6] = 4;
        if (gConfigNotifications.guest_warnings)
        {
            news_item_add_to_queue(NEWS_ITEM_PEEPS, STR_PEEPS_GETTING_LOST_OR_STUCK, 16);
        }
    }
}

void peep_stop_crowd_noise()
{
    if (_crowdSoundChannel != nullptr)
    {
        Mixer_Stop_Channel(_crowdSoundChannel);
        _crowdSoundChannel = nullptr;
    }
}

/**
 *
 *  rct2: 0x006BD18A
 */
void peep_update_crowd_noise()
{
    rct_viewport * viewport;
    uint16_t         spriteIndex;
    rct_peep *     peep;
    int32_t         visiblePeeps;

    if (gGameSoundsOff)
        return;

    if (!gConfigSound.sound_enabled)
        return;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    viewport = g_music_tracking_viewport;
    if (viewport == nullptr)
        return;

    // Count the number of peeps visible
    visiblePeeps = 0;

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->sprite_left == LOCATION_NULL)
            continue;
        if (viewport->view_x > peep->sprite_right)
            continue;
        if (viewport->view_x + viewport->view_width < peep->sprite_left)
            continue;
        if (viewport->view_y > peep->sprite_bottom)
            continue;
        if (viewport->view_y + viewport->view_height < peep->sprite_top)
            continue;

        visiblePeeps += peep->state == PEEP_STATE_QUEUING ? 1 : 2;
    }

    // This function doesn't account for the fact that the screen might be so big that 100 peeps could potentially be very
    // spread out and therefore not produce any crowd noise. Perhaps a more sophisticated solution would check how many peeps
    // were in close proximity to each other.

    // Allows queuing peeps to make half as much noise, and at least 6 peeps must be visible for any crowd noise
    visiblePeeps = (visiblePeeps / 2) - 6;
    if (visiblePeeps < 0)
    {
        // Mute crowd noise
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Stop_Channel(_crowdSoundChannel);
            _crowdSoundChannel = nullptr;
        }
    }
    else
    {
        int32_t volume;

        // Formula to scale peeps to dB where peeps [0, 120] scales approximately logarithmically to [-3314, -150] dB/100
        // 207360000 maybe related to DSBVOLUME_MIN which is -10,000 (dB/100)
        volume = 120 - std::min(visiblePeeps, 120);
        volume = volume * volume * volume * volume;
        volume = (((207360000 - volume) >> viewport->zoom) - 207360000) / 65536 - 150;

        // Load and play crowd noise if needed and set volume
        if (_crowdSoundChannel == nullptr)
        {
            _crowdSoundChannel = Mixer_Play_Music(PATH_ID_CSS2, MIXER_LOOP_INFINITE, false);
            if (_crowdSoundChannel != nullptr)
            {
                Mixer_Channel_SetGroup(_crowdSoundChannel, MIXER_GROUP_SOUND);
            }
        }
        if (_crowdSoundChannel != nullptr)
        {
            Mixer_Channel_Volume(_crowdSoundChannel, DStoMixerVolume(volume));
        }
    }
}

/**
 *
 *  rct2: 0x0069BE9B
 */
void peep_applause()
{
    uint16_t     spriteIndex;
    rct_peep * peep;

    FOR_ALL_GUESTS(spriteIndex, peep)
    {
        if (peep->outside_of_park != 0)
            continue;

        // Release balloon
        peep_release_balloon(peep, peep->z + 9);

        // Clap
        if ((peep->state == PEEP_STATE_WALKING || peep->state == PEEP_STATE_QUEUING) && peep->action >= 254)
        {
            peep->action                     = PEEP_ACTION_CLAP;
            peep->action_frame               = 0;
            peep->action_sprite_image_offset = 0;
            peep->UpdateCurrentActionSpriteType();
            peep->Invalidate();
        }
    }

    // Play applause noise
    audio_play_sound(SOUND_APPLAUSE, 0, context_get_width() / 2);
}

/**
 *
 *  rct2: 0x0069C35E
 */
void peep_update_days_in_queue()
{
    uint16_t     sprite_index;
    rct_peep * peep;

    FOR_ALL_GUESTS(sprite_index, peep)
    {
        if (peep->outside_of_park == 0 && peep->state == PEEP_STATE_QUEUING)
        {
            if (peep->days_in_queue < 255)
            {
                peep->days_in_queue += 1;
            }
        }
    }
}

// clang-format off
/** rct2: 0x009823A0 */
static constexpr const enum PEEP_NAUSEA_TOLERANCE nausea_tolerance_distribution[] = {
    PEEP_NAUSEA_TOLERANCE_NONE,
    PEEP_NAUSEA_TOLERANCE_LOW, PEEP_NAUSEA_TOLERANCE_LOW,
    PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE, PEEP_NAUSEA_TOLERANCE_AVERAGE,
    PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH, PEEP_NAUSEA_TOLERANCE_HIGH,
};

/** rct2: 0x009823BC */
static constexpr const uint8_t trouser_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
};

/** rct2: 0x009823D5 */
static constexpr const uint8_t tshirt_colours[] = {
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_BLACK,
    COLOUR_GREY,
    COLOUR_LIGHT_BROWN,
    COLOUR_SATURATED_BROWN,
    COLOUR_DARK_BROWN,
    COLOUR_SALMON_PINK,
    COLOUR_DARK_PURPLE,
    COLOUR_LIGHT_PURPLE,
    COLOUR_DARK_BLUE,
    COLOUR_SATURATED_GREEN,
    COLOUR_SATURATED_RED,
    COLOUR_DARK_ORANGE,
    COLOUR_BORDEAUX_RED,
    COLOUR_WHITE,
    COLOUR_BRIGHT_PURPLE,
    COLOUR_LIGHT_BLUE,
    COLOUR_TEAL,
    COLOUR_DARK_GREEN,
    COLOUR_MOSS_GREEN,
    COLOUR_BRIGHT_GREEN,
    COLOUR_OLIVE_GREEN,
    COLOUR_DARK_OLIVE_GREEN,
    COLOUR_YELLOW,
    COLOUR_LIGHT_ORANGE,
    COLOUR_BRIGHT_RED,
    COLOUR_DARK_PINK,
    COLOUR_BRIGHT_PINK,
};
// clang-format on

/**
 *
 *  rct2: 0x0069A05D
 */
rct_peep * peep_generate(int32_t x, int32_t y, int32_t z)
{
    if (gSpriteListCount[SPRITE_LIST_NULL] < 400)
        return nullptr;

    rct_peep * peep = (rct_peep *)create_sprite(1);

    move_sprite_to_list((rct_sprite *)peep, SPRITE_LIST_PEEP * 2);

    peep->sprite_identifier          = SPRITE_IDENTIFIER_PEEP;
    peep->sprite_type                = PEEP_SPRITE_TYPE_NORMAL;
    peep->outside_of_park            = 1;
    peep->state                      = PEEP_STATE_FALLING;
    peep->action                     = PEEP_ACTION_NONE_2;
    peep->special_sprite             = 0;
    peep->action_sprite_image_offset = 0;
    peep->no_action_frame_num         = 0;
    peep->action_sprite_type         = 0;
    peep->peep_flags                 = 0;
    peep->favourite_ride             = RIDE_ID_NULL;
    peep->favourite_ride_rating      = 0;

    const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[peep->sprite_type].sprite_bounds;
    peep->sprite_width                     = spriteBounds[peep->action_sprite_type].sprite_width;
    peep->sprite_height_negative           = spriteBounds[peep->action_sprite_type].sprite_height_negative;
    peep->sprite_height_positive           = spriteBounds[peep->action_sprite_type].sprite_height_positive;

    peep->sprite_direction = 0;

    sprite_move(x, y, z, (rct_sprite *)peep);
    peep->Invalidate();

    peep->mass                  = (scenario_rand() & 0x1F) + 45;
    peep->path_check_optimisation                  = 0;
    peep->interaction_ride_index  = 0xFF;
    peep->type                    = PEEP_TYPE_GUEST;
    peep->previous_ride           = 0xFF;
    peep->thoughts->type          = PEEP_THOUGHT_TYPE_NONE;
    peep->window_invalidate_flags = 0;

    uint8_t intensityHighest = (scenario_rand() & 0x7) + 3;
    uint8_t intensityLowest = std::min(intensityHighest, static_cast<uint8_t>(7)) - 3;

    if (intensityHighest >= 7)
        intensityHighest = 15;

    /* Check which intensity boxes are enabled
     * and apply the appropriate intensity settings. */
    if (gParkFlags & PARK_FLAGS_PREF_LESS_INTENSE_RIDES)
    {
        if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
        {
            intensityLowest = 0;
            intensityHighest = 15;
        }
        else
        {
            intensityLowest = 0;
            intensityHighest = 4;
        }
    }
    else if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        intensityLowest = 9;
        intensityHighest = 15;
    }

    peep->intensity = (intensityHighest << 4) | intensityLowest;

    uint8_t nausea_tolerance = scenario_rand() & 0x7;
    if (gParkFlags & PARK_FLAGS_PREF_MORE_INTENSE_RIDES)
    {
        nausea_tolerance += 4;
    }

    peep->nausea_tolerance = nausea_tolerance_distribution[nausea_tolerance];

    /* Scenario editor limits initial guest happiness to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->happiness = gGuestInitialHappiness;
    /* Assume a default initial happiness of 0 is wrong and set
     * to 128 (50%) instead. */
    if (gGuestInitialHappiness == 0)
        peep->happiness = 128;
    /* Initial value will vary by -15..16 */
    int8_t happiness_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->happiness        = Math::Clamp(0, peep->happiness + happiness_delta, PEEP_MAX_HAPPINESS);
    peep->happiness_target = peep->happiness;
    peep->nausea           = 0;
    peep->nausea_target    = 0;

    /* Scenario editor limits initial guest hunger to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->hunger = gGuestInitialHunger;
    /* Initial value will vary by -15..16 */
    int8_t hunger_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->hunger = Math::Clamp(0, peep->hunger + hunger_delta, 255);

    /* Scenario editor limits initial guest thirst to between 37..253.
     * To be on the safe side, assume the value could have been hacked
     * to any value 0..255. */
    peep->thirst = gGuestInitialThirst;
    /* Initial value will vary by -15..16 */
    int8_t thirst_delta = (scenario_rand() & 0x1F) - 15;
    /* Adjust by the delta, clamping at min=0 and max=255. */
    peep->thirst = Math::Clamp(0, peep->thirst + thirst_delta, 0xFF);

    peep->toilet = 0;
    peep->time_to_consume   = 0;
    memset(&peep->rides_been_on, 0, 32);

    peep->no_of_rides = 0;
    memset(&peep->ride_types_been_on, 0, 16);
    peep->id              = gNextGuestNumber++;
    peep->name_string_idx = STR_GUEST_X;

    money32 cash = (scenario_rand() & 0x3) * 100 - 100 + gGuestInitialCash;
    if (cash < 0)
        cash = 0;

    if (gGuestInitialCash == 0)
    {
        cash = 500;
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        cash = 0;
    }

    if (gGuestInitialCash == (money16)(uint16_t)0xFFFF)
    {
        cash = 0;
    }

    peep->cash_in_pocket           = cash;
    peep->cash_spent               = 0;
    peep->time_in_park             = -1;
    peep->pathfind_goal.x          = 0xFF;
    peep->pathfind_goal.y          = 0xFF;
    peep->pathfind_goal.z          = 0xFF;
    peep->pathfind_goal.direction  = 0xFF;
    peep->item_standard_flags      = 0;
    peep->item_extra_flags         = 0;
    peep->guest_heading_to_ride_id = 0xFF;
    peep->litter_count             = 0;
    peep->disgusting_count         = 0;
    peep->vandalism_seen                   = 0;
    peep->paid_to_enter            = 0;
    peep->paid_on_rides            = 0;
    peep->paid_on_food             = 0;
    peep->paid_on_drink            = 0;
    peep->paid_on_souvenirs        = 0;
    peep->no_of_food               = 0;
    peep->no_of_drinks             = 0;
    peep->no_of_souvenirs          = 0;
    peep->surroundings_thought_timeout = 0;
    peep->angriness                = 0;
    peep->time_lost                   = 0;

    uint8_t tshirt_colour = static_cast<uint8_t>(scenario_rand() % Util::CountOf(tshirt_colours));
    peep->tshirt_colour = tshirt_colours[tshirt_colour];

    uint8_t trousers_colour = static_cast<uint8_t>(scenario_rand() % Util::CountOf(trouser_colours));
    peep->trousers_colour = trouser_colours[trousers_colour];

    /* Minimum energy is capped at 32 and maximum at 128, so this initialises
     * a peep with approx 34%-100% energy. (65 - 32) / (128 - 32) ≈ 34% */
    uint8_t energy        = (scenario_rand() % 64) + 65;
    peep->energy        = energy;
    peep->energy_target = energy;

    if (gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES)
    {
        peep_give_real_name(peep);
    }
    peep_update_name_sort(peep);

    increment_guests_heading_for_park();

    return peep;
}

/**
 * rct2: 0x00698B0D
 * peep.sprite_index (eax)
 * thought.type (ebx)
 * argument_1 (ecx & ebx)
 * argument_2 (edx)
 */
void get_arguments_from_action(rct_peep * peep, uint32_t * argument_1, uint32_t * argument_2)
{
    Ride * ride;

    switch (peep->state)
    {
    case PEEP_STATE_FALLING:
        *argument_1 = peep->action == PEEP_ACTION_DROWNING ? STR_DROWNING : STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_1:
        *argument_1 = STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_ON_RIDE:
    case PEEP_STATE_LEAVING_RIDE:
    case PEEP_STATE_ENTERING_RIDE:
        *argument_1 = STR_ON_RIDE;
        ride        = get_ride(peep->current_ride);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE))
            *argument_1 = STR_IN_RIDE;
        *argument_1 |= ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_BUYING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_AT_RIDE | ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_WALKING:
    case PEEP_STATE_USING_BIN:
        if (peep->guest_heading_to_ride_id != 0xFF)
        {
            ride        = get_ride(peep->guest_heading_to_ride_id);
            *argument_1 = STR_HEADING_FOR | ((uint32_t)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        else
        {
            *argument_1 = (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK) ? STR_LEAVING_PARK : STR_WALKING;
            *argument_2 = 0;
        }
        break;
    case PEEP_STATE_QUEUING_FRONT:
    case PEEP_STATE_QUEUING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_QUEUING_FOR | ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_SITTING:
        *argument_1 = STR_SITTING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_WATCHING:
        if (peep->current_ride != 0xFF)
        {
            ride        = get_ride(peep->current_ride);
            *argument_1 = STR_WATCHING_RIDE | ((uint32_t)ride->name << 16);
            *argument_2 = ride->name_arguments;
            if (peep->current_seat & 0x1)
                *argument_1 = STR_WATCHING_CONSTRUCTION_OF | ((uint32_t)ride->name << 16);
            else
                *argument_1 = STR_WATCHING_RIDE | ((uint32_t)ride->name << 16);
        }
        else
        {
            *argument_1 = (peep->current_seat & 0x1) ? STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED : STR_LOOKING_AT_SCENERY;
            *argument_2 = 0;
        }
        break;
    case PEEP_STATE_PICKED:
        *argument_1 = STR_SELECT_LOCATION;
        *argument_2 = 0;
        break;
    case PEEP_STATE_PATROLLING:
    case PEEP_STATE_ENTERING_PARK:
    case PEEP_STATE_LEAVING_PARK:
        *argument_1 = STR_WALKING;
        *argument_2 = 0;
        break;
    case PEEP_STATE_MOWING:
        *argument_1 = STR_MOWING_GRASS;
        *argument_2 = 0;
        break;
    case PEEP_STATE_SWEEPING:
        *argument_1 = STR_SWEEPING_FOOTPATH;
        *argument_2 = 0;
        break;
    case PEEP_STATE_WATERING:
        *argument_1 = STR_WATERING_GARDENS;
        *argument_2 = 0;
        break;
    case PEEP_STATE_EMPTYING_BIN:
        *argument_1 = STR_EMPTYING_LITTER_BIN;
        *argument_2 = 0;
        break;
    case PEEP_STATE_ANSWERING:
        if (peep->sub_state == 0)
        {
            *argument_1 = STR_WALKING;
            *argument_2 = 0;
        }
        else if (peep->sub_state == 1)
        {
            *argument_1 = STR_ANSWERING_RADIO_CALL;
            *argument_2 = 0;
        }
        else
        {
            ride        = get_ride(peep->current_ride);
            *argument_1 = STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL | ((uint32_t)ride->name << 16);
            *argument_2 = ride->name_arguments;
        }
        break;
    case PEEP_STATE_FIXING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_FIXING_RIDE | ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_HEADING_TO_INSPECTION:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_HEADING_TO_RIDE_FOR_INSPECTION | ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    case PEEP_STATE_INSPECTING:
        ride        = get_ride(peep->current_ride);
        *argument_1 = STR_INSPECTING_RIDE | ((uint32_t)ride->name << 16);
        *argument_2 = ride->name_arguments;
        break;
    }
}

/**
 * rct2: 0x00698342
 * thought.item (eax)
 * thought.type (ebx)
 * argument_1 (esi & ebx)
 * argument_2 (esi+2)
 */
void peep_thought_set_format_args(rct_peep_thought * thought)
{
    set_format_arg(0, rct_string_id, PeepThoughts[thought->type]);

    uint8_t flags = PeepThoughtToActionMap[thought->type].flags;
    if (flags & 1)
    {
        Ride * ride = get_ride(thought->item);
        set_format_arg(2, rct_string_id, ride->name);
        set_format_arg(4, uint32_t, ride->name_arguments);
    }
    else if (flags & 2)
    {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].singular);
    }
    else if (flags & 4)
    {
        set_format_arg(2, rct_string_id, ShopItemStringIds[thought->item].indefinite);
    }
    else
    {
    }
}

/** rct2: 0x00982004 */
static constexpr const bool peep_allow_pick_up[] = {
    true,  // PEEP_STATE_FALLING
    false, // PEEP_STATE_1
    false, // PEEP_STATE_QUEUING_FRONT
    false, // PEEP_STATE_ON_RIDE
    false, // PEEP_STATE_LEAVING_RIDE
    true,  // PEEP_STATE_WALKING
    true,  // PEEP_STATE_QUEUING
    false, // PEEP_STATE_ENTERING_RIDE
    true,  // PEEP_STATE_SITTING
    true,  // PEEP_STATE_PICKED
    true,  // PEEP_STATE_PATROLLING
    true,  // PEEP_STATE_MOWING
    true,  // PEEP_STATE_SWEEPING
    false, // PEEP_STATE_ENTERING_PARK
    false, // PEEP_STATE_LEAVING_PARK
    true,  // PEEP_STATE_ANSWERING
    false, // PEEP_STATE_FIXING
    false, // PEEP_STATE_BUYING
    true,  // PEEP_STATE_WATCHING
    true,  // PEEP_STATE_EMPTYING_BIN
    true,  // PEEP_STATE_USING_BIN
    true,  // PEEP_STATE_WATERING
    true,  // PEEP_STATE_HEADING_TO_INSPECTION
    false, // PEEP_STATE_INSPECTING
};

/**
 *
 *  rct2: 0x00698827
 * returns 1 on pickup (CF not set)
 */
bool peep_can_be_picked_up(rct_peep * peep)
{
    return peep_allow_pick_up[peep->state];
}

enum
{
    PEEP_FACE_OFFSET_ANGRY = 0,
    PEEP_FACE_OFFSET_VERY_VERY_SICK,
    PEEP_FACE_OFFSET_VERY_SICK,
    PEEP_FACE_OFFSET_SICK,
    PEEP_FACE_OFFSET_VERY_TIRED,
    PEEP_FACE_OFFSET_TIRED,
    PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_VERY_UNHAPPY,
    PEEP_FACE_OFFSET_UNHAPPY,
    PEEP_FACE_OFFSET_NORMAL,
    PEEP_FACE_OFFSET_HAPPY,
    PEEP_FACE_OFFSET_VERY_HAPPY,
    PEEP_FACE_OFFSET_VERY_VERY_HAPPY,
};

static constexpr const int32_t face_sprite_small[] = {
    SPR_PEEP_SMALL_FACE_ANGRY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_SICK,
    SPR_PEEP_SMALL_FACE_VERY_SICK,
    SPR_PEEP_SMALL_FACE_SICK,
    SPR_PEEP_SMALL_FACE_VERY_TIRED,
    SPR_PEEP_SMALL_FACE_TIRED,
    SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_VERY_UNHAPPY,
    SPR_PEEP_SMALL_FACE_UNHAPPY,
    SPR_PEEP_SMALL_FACE_NORMAL,
    SPR_PEEP_SMALL_FACE_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_HAPPY,
    SPR_PEEP_SMALL_FACE_VERY_VERY_HAPPY,
};

static constexpr const int32_t face_sprite_large[] = {
    SPR_PEEP_LARGE_FACE_ANGRY_0,
    SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_VERY_SICK_0,
    SPR_PEEP_LARGE_FACE_SICK,
    SPR_PEEP_LARGE_FACE_VERY_TIRED,
    SPR_PEEP_LARGE_FACE_TIRED,
    SPR_PEEP_LARGE_FACE_VERY_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_VERY_UNHAPPY,
    SPR_PEEP_LARGE_FACE_UNHAPPY,
    SPR_PEEP_LARGE_FACE_NORMAL,
    SPR_PEEP_LARGE_FACE_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_HAPPY,
    SPR_PEEP_LARGE_FACE_VERY_VERY_HAPPY,
};

static int32_t get_face_sprite_offset(rct_peep * peep)
{

    // ANGRY
    if (peep->angriness > 0)
        return PEEP_FACE_OFFSET_ANGRY;

    // VERY_VERY_SICK
    if (peep->nausea > 200)
        return PEEP_FACE_OFFSET_VERY_VERY_SICK;

    // VERY_SICK
    if (peep->nausea > 170)
        return PEEP_FACE_OFFSET_VERY_SICK;

    // SICK
    if (peep->nausea > 140)
        return PEEP_FACE_OFFSET_SICK;

    // VERY_TIRED
    if (peep->energy < 46)
        return PEEP_FACE_OFFSET_VERY_TIRED;

    // TIRED
    if (peep->energy < 70)
        return PEEP_FACE_OFFSET_TIRED;

    int32_t offset = PEEP_FACE_OFFSET_VERY_VERY_UNHAPPY;
    // There are 7 different happiness based faces
    for (int32_t i = 37; peep->happiness >= i; i += 37)
    {
        offset++;
    }

    return offset;
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t get_peep_face_sprite_small(rct_peep * peep)
{
    return face_sprite_small[get_face_sprite_offset(peep)];
}

/**
 * Function split into large and small sprite
 *  rct2: 0x00698721
 */
int32_t get_peep_face_sprite_large(rct_peep * peep)
{
    return face_sprite_large[get_face_sprite_offset(peep)];
}

/**
 *
 *  rct2: 0x0069A5A0
 * tests if a peep's name matches a cheat code, normally returns using a register flag
 */
int32_t peep_check_easteregg_name(int32_t index, rct_peep * peep)
{
    char buffer[256];

    format_string(buffer, 256, peep->name_string_idx, &peep->id);
    return _stricmp(buffer, gPeepEasterEggNames[index]) == 0;
}

int32_t peep_get_easteregg_name_id(rct_peep * peep)
{
    char buffer[256];

    format_string(buffer, 256, peep->name_string_idx, &peep->id);

    for (uint32_t i = 0; i < Util::CountOf(gPeepEasterEggNames); i++)
        if (_stricmp(buffer, gPeepEasterEggNames[i]) == 0)
            return static_cast<int32_t>(i);

    return -1;
}

/**
 *
 *  rct2: 0x699F5A
 * al:thought_type
 * ah:thought_arguments
 * esi: peep
 */
void peep_insert_new_thought(rct_peep * peep, uint8_t thought_type, uint8_t thought_arguments)
{
    uint8_t action = PeepThoughtToActionMap[thought_type].action;
    if (action != 0xFF && peep->action >= 254)
    {
        peep->action                     = action;
        peep->action_frame               = 0;
        peep->action_sprite_image_offset = 0;
        peep->UpdateCurrentActionSpriteType();
        peep->Invalidate();
    }

    for (int32_t i = 0; i < PEEP_MAX_THOUGHTS; ++i)
    {
        rct_peep_thought * thought = &peep->thoughts[i];
        // Remove the oldest thought by setting it to NONE.
        if (thought->type == PEEP_THOUGHT_TYPE_NONE)
            break;

        if (thought->type == thought_type && thought->item == thought_arguments)
        {
            // If the thought type has not changed then we need to move
            // it to the top of the thought list. This is done by first removing the
            // existing thought and placing it at the top.
            if (i < PEEP_MAX_THOUGHTS - 2)
            {
                memmove(thought, thought + 1, sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - i - 1));
            }
            break;
        }
    }

    memmove(&peep->thoughts[1], &peep->thoughts[0], sizeof(rct_peep_thought) * (PEEP_MAX_THOUGHTS - 1));

    peep->thoughts[0].type  = thought_type;
    peep->thoughts[0].item  = thought_arguments;
    peep->thoughts[0].freshness = 0;
    peep->thoughts[0].fresh_timeout = 0;

    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_THOUGHTS;
}

void peep_set_map_tooltip(rct_peep * peep)
{
    if (peep->type == PEEP_TYPE_GUEST)
    {
        set_map_tooltip_format_arg(0, rct_string_id,
                                   (peep->peep_flags & PEEP_FLAGS_TRACKING) ? STR_TRACKED_GUEST_MAP_TIP : STR_GUEST_MAP_TIP);
        set_map_tooltip_format_arg(2, uint32_t, get_peep_face_sprite_small(peep));
        set_map_tooltip_format_arg(6, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(8, uint32_t, peep->id);

        uint32_t arg0 = 0, arg1 = 0;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(12, uint32_t, arg0);
        set_map_tooltip_format_arg(16, uint32_t, arg1);
    }
    else
    {
        set_map_tooltip_format_arg(0, rct_string_id, STR_STAFF_MAP_TIP);
        set_map_tooltip_format_arg(2, rct_string_id, peep->name_string_idx);
        set_map_tooltip_format_arg(4, uint32_t, peep->id);

        uint32_t arg0 = 0, arg1 = 0;
        get_arguments_from_action(peep, &arg0, &arg1);
        set_map_tooltip_format_arg(8, uint32_t, arg0);
        set_map_tooltip_format_arg(12, uint32_t, arg1);
    }
}

/**
 *  rct2: 0x00693BAB
 */
void rct_peep::SwitchNextActionSpriteType()
{
    // TBD: Add nextActionSpriteType as function parameter and make peep->next_action_sprite_type obsolete?
    if (next_action_sprite_type != action_sprite_type)
    {
        Invalidate();
        action_sprite_type               = next_action_sprite_type;
        const rct_sprite_bounds * spriteBounds = g_peep_animation_entries[sprite_type].sprite_bounds;
        sprite_width                     = spriteBounds[next_action_sprite_type].sprite_width;
        sprite_height_negative           = spriteBounds[next_action_sprite_type].sprite_height_negative;
        sprite_height_positive           = spriteBounds[next_action_sprite_type].sprite_height_positive;
        Invalidate();
    }
}

/**
 *
 *  rct2: 0x00693CBB
 */
static bool peep_update_queue_position(rct_peep * peep, uint8_t previous_action)
{
    peep->time_in_queue++;
    if (peep->next_in_queue == SPRITE_INDEX_NULL)
        return false;

    rct_peep * peep_next = GET_PEEP(peep->next_in_queue);

    int16_t x_diff = abs(peep_next->x - peep->x);
    int16_t y_diff = abs(peep_next->y - peep->y);
    int16_t z_diff = abs(peep_next->z - peep->z);

    if (z_diff > 10)
        return false;

    if (x_diff < y_diff)
    {
        int16_t temp_x = x_diff;
        x_diff        = y_diff;
        y_diff        = temp_x;
    }

    x_diff += y_diff / 2;
    if (x_diff > 7)
    {
        if (x_diff > 13)
        {
            if ((peep->x & 0xFFE0) != (peep_next->x & 0xFFE0) || (peep->y & 0xFFE0) != (peep_next->y & 0xFFE0))
                return false;
        }

        if (peep->sprite_direction != peep_next->sprite_direction)
            return false;

        switch (peep_next->sprite_direction / 8)
        {
        case 0:
            if (peep->x >= peep_next->x)
                return false;
            break;
        case 1:
            if (peep->y <= peep_next->y)
                return false;
            break;
        case 2:
            if (peep->x <= peep_next->x)
                return false;
            break;
        case 3:
            if (peep->y >= peep_next->y)
                return false;
            break;
        }
    }

    int16_t xy_dist, x, y;
    if (peep->action < PEEP_ACTION_NONE_1)
        peep->UpdateAction(&x, &y, &xy_dist);

    if (peep->action != PEEP_ACTION_NONE_2)
        return true;

    peep->action                  = PEEP_ACTION_NONE_1;
    peep->next_action_sprite_type = 2;
    if (previous_action != PEEP_ACTION_NONE_1)
        peep->Invalidate();
    return true;
}

/**
 *
 *  rct2: 0x00693EF2
 */
static void peep_return_to_centre_of_tile(rct_peep * peep)
{
    peep->direction ^= (1 << 1);
    peep->destination_x         = (peep->x & 0xFFE0) + 16;
    peep->destination_y         = (peep->y & 0xFFE0) + 16;
    peep->destination_tolerance = 5;
}

/**
 *
 *  rct2: 0x00693f2C
 */
static void peep_interact_with_entrance(rct_peep * peep, int16_t x, int16_t y, rct_tile_element * tile_element, uint8_t & pathing_result)
{
    uint8_t entranceType = tile_element->properties.entrance.type;
    uint8_t rideIndex    = tile_element->properties.entrance.ride_index;

    // Store some details to determine when to override the default
    // behaviour (defined below) for when staff attempt to enter a ride
    // to fix/inspect it.
    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        pathing_result |= PATHING_RIDE_EXIT;
        _peepRideEntranceExitElement = tile_element;
    }
    else if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        pathing_result |= PATHING_RIDE_ENTRANCE;
        _peepRideEntranceExitElement = tile_element;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_EXIT)
    {
        // Default guest/staff behaviour attempting to enter a
        // ride exit is to turn around.
        peep->interaction_ride_index = 0xFF;
        peep_return_to_centre_of_tile(peep);
        return;
    }

    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        if (peep->type == PEEP_TYPE_STAFF)
        {
            // Default staff behaviour attempting to enter a
            // ride entrance is to turn around.
            peep->interaction_ride_index = 0xFF;
            peep_return_to_centre_of_tile(peep);
            return;
        }

        if (peep->state == PEEP_STATE_QUEUING)
        {
            // Guest is in the ride queue.
            peep->sub_state                  = 11;
            peep->action_sprite_image_offset = _unk_F1AEF0;
            return;
        }

        // Guest is on a normal path, i.e. ride has no queue.
        if (peep->interaction_ride_index == rideIndex)
        {
            // Peep is retrying the ride entrance without leaving
            // the path tile and without trying any other ride
            // attached to this path tile. i.e. stick with the
            // peeps previous decision not to go on the ride.
            peep_return_to_centre_of_tile(peep);
            return;
        }

        peep->time_lost  = 0;
        uint8_t stationNum = (tile_element->properties.entrance.index >> 4) & 0x7;
        // Guest walks up to the ride for the first time since entering
        // the path tile or since considering another ride attached to
        // the path tile.
        if (!peep->ShouldGoOnRide(rideIndex, stationNum, false, false))
        {
            // Peep remembers that this is the last ride they
            // considered while on this path tile.
            peep->interaction_ride_index = rideIndex;
            peep_return_to_centre_of_tile(peep);
            return;
        }

        // Guest has decided to go on the ride.
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep->interaction_ride_index     = rideIndex;

        Ride * ride                          = get_ride(rideIndex);
        uint16_t previous_last                 = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue                  = previous_last;
        ride->queue_length[stationNum]++;

        peep->current_ride         = rideIndex;
        peep->current_ride_station = stationNum;
        peep->days_in_queue        = 0;
        peep->SetState(PEEP_STATE_QUEUING);
        peep->sub_state     = 11;
        peep->time_in_queue = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32_t, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32_t, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }
    }
    else
    {
        // PARK_ENTRANCE
        if (peep->type == PEEP_TYPE_STAFF)
        {
            // Staff cannot leave the park, so go back.
            peep_return_to_centre_of_tile(peep);
            return;
        }

        // If not the centre of the entrance arch
        if (tile_element->properties.entrance.index & 0xF)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }

        uint8_t entranceDirection = tile_element_get_direction(tile_element);
        if (entranceDirection != peep->direction)
        {
            if ((entranceDirection ^ (1 << 1)) != peep->direction)
            {
                peep_return_to_centre_of_tile(peep);
                return;
            }

            // Peep is leaving the park.
            if (peep->state != PEEP_STATE_WALKING)
            {
                peep_return_to_centre_of_tile(peep);
                return;
            }

            if (!(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
            {
                // If the park is open and leaving flag isn't set return to centre
                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                {
                    peep_return_to_centre_of_tile(peep);
                    return;
                }
            }

            peep->destination_x += CoordsDirectionDelta[peep->direction].x;
            peep->destination_y += CoordsDirectionDelta[peep->direction].y;
            peep->destination_tolerance = 9;
            peep->Invalidate();
            sprite_move(x, y, peep->z, (rct_sprite *)peep);
            peep->Invalidate();

            peep->SetState(PEEP_STATE_LEAVING_PARK);

            peep->var_37 = 0;
            if (peep->peep_flags & PEEP_FLAGS_TRACKING)
            {
                set_format_arg(0, rct_string_id, peep->name_string_idx);
                set_format_arg(2, uint32_t, peep->id);
                if (gConfigNotifications.guest_left_park)
                {
                    news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_LEFT_PARK, peep->sprite_index);
                }
            }
            return;
        }

        // Peep is entering the park.

        if (peep->state != PEEP_STATE_ENTERING_PARK)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }

        if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
        {
            peep->state  = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            peep_return_to_centre_of_tile(peep);
            return;
        }

        uint8_t entranceIndex = 0;
        for (entranceIndex = 0; entranceIndex < MAX_PARK_ENTRANCES; entranceIndex++)
        {
            if (gParkEntrances[entranceIndex].x == (x & 0xFFE0) && gParkEntrances[entranceIndex].y == (y & 0xFFE0))
                break;
        }

        int16_t z          = gParkEntrances[entranceIndex].z / 8;
        entranceDirection = gParkEntrances[entranceIndex].direction;

        int16_t next_x = (x & 0xFFE0) + CoordsDirectionDelta[entranceDirection].x;
        int16_t next_y = (y & 0xFFE0) + CoordsDirectionDelta[entranceDirection].y;

        // Make sure there is a path right behind the entrance, otherwise turn around
        bool               found           = false;
        rct_tile_element * nextTileElement = map_get_first_element_at(next_x / 32, next_y / 32);
        do
        {
            if (nextTileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (footpath_element_is_queue(nextTileElement))
                continue;

            if (footpath_element_is_sloped(nextTileElement))
            {
                uint8_t slopeDirection = footpath_element_get_slope_direction(nextTileElement);
                if (slopeDirection == entranceDirection)
                {
                    if (z != nextTileElement->base_height)
                    {
                        continue;
                    }
                    found = true;
                    break;
                }

                if ((slopeDirection ^ (1 << 1)) != entranceDirection)
                    continue;

                if (z - 2 != nextTileElement->base_height)
                    continue;
                found = true;
                break;
            }
            else
            {
                if (z != nextTileElement->base_height)
                {
                    continue;
                }
                found = true;
                break;
            }
        } while (!(nextTileElement++)->IsLastForTile());

        if (!found)
        {
            peep->state  = PEEP_STATE_LEAVING_PARK;
            peep->var_37 = 1;
            decrement_guests_heading_for_park();
            peep_window_state_update(peep);
            peep_return_to_centre_of_tile(peep);
            return;
        }

        money16 entranceFee = park_get_entrance_fee();
        if (entranceFee != 0)
        {
            if (peep->item_standard_flags & PEEP_ITEM_VOUCHER)
            {
                if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE)
                {
                    entranceFee /= 2;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
                else if (peep->voucher_type == VOUCHER_TYPE_PARK_ENTRY_FREE)
                {
                    entranceFee = 0;
                    peep->item_standard_flags &= ~PEEP_ITEM_VOUCHER;
                    peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
                }
            }
            if (entranceFee > peep->cash_in_pocket)
            {
                peep->state  = PEEP_STATE_LEAVING_PARK;
                peep->var_37 = 1;
                decrement_guests_heading_for_park();
                peep_window_state_update(peep);
                peep_return_to_centre_of_tile(peep);
                return;
            }

            gTotalIncomeFromAdmissions += entranceFee;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS;
            peep->SpendMoney(peep->paid_to_enter, entranceFee);
            peep->peep_flags |= PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY;
        }

        gTotalAdmissions++;
        window_invalidate_by_number(WC_PARK_INFORMATION, 0);

        peep->var_37 = 1;
        peep->destination_x += CoordsDirectionDelta[peep->direction].x;
        peep->destination_y += CoordsDirectionDelta[peep->direction].y;
        peep->destination_tolerance = 7;

        peep->Invalidate();
        sprite_move(x, y, peep->z, (rct_sprite *)peep);
        peep->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006946D8
 */
static void peep_footpath_move_forward(rct_peep * peep, int16_t x, int16_t y, rct_tile_element * tile_element, bool vandalism)
{
    peep->next_x      = (x & 0xFFE0);
    peep->next_y      = (y & 0xFFE0);
    peep->next_z      = tile_element->base_height;
    peep->SetNextFlags(tile_element->properties.path.type & 3, tile_element->properties.path.type & 4, false);

    int16_t z = peep->GetZOnSlope(x, y);

    if (peep->type == PEEP_TYPE_STAFF)
    {
        peep->Invalidate();
        sprite_move(x, y, z, (rct_sprite *)peep);
        peep->Invalidate();
        return;
    }

    uint8_t vandalThoughtTimeout = (peep->vandalism_seen & 0xC0) >> 6;
    // Advance the vandalised tiles by 1
    uint8_t vandalisedTiles = (peep->vandalism_seen * 2) & 0x3F;

    if (vandalism == true)
    {
        // Add one more to the vandalised tiles
        vandalisedTiles |= 1;
        // If there has been 2 vandalised tiles in the last 6
        if (vandalisedTiles & 0x3E && (vandalThoughtTimeout == 0))
        {

            if ((scenario_rand() & 0xFFFF) <= 10922)
            {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_VANDALISM, PEEP_THOUGHT_ITEM_NONE);
                peep->happiness_target = std::max(0, peep->happiness_target - 17);
            }
            vandalThoughtTimeout = 3;
        }
    }

    if (vandalThoughtTimeout && (scenario_rand() & 0xFFFF) <= 4369)
    {
        vandalThoughtTimeout--;
    }

    peep->vandalism_seen = (vandalThoughtTimeout << 6) | vandalisedTiles;
    uint16_t crowded      = 0;
    uint8_t  litter_count = 0;
    uint8_t  sick_count   = 0;
    uint16_t sprite_id    = sprite_get_first_in_quadrant(x, y);
    for (rct_sprite * sprite; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);
        if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
        {
            rct_peep * other_peep = (rct_peep *)sprite;
            if (other_peep->state != PEEP_STATE_WALKING)
                continue;

            if (abs(other_peep->z - peep->next_z * 8) > 16)
                continue;
            crowded++;
            continue;
        }
        else if (sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_LITTER)
        {
            rct_litter * litter = (rct_litter *)sprite;
            if (abs(litter->z - peep->next_z * 8) > 16)
                continue;

            litter_count++;
            if (litter->type != LITTER_TYPE_SICK && litter->type != LITTER_TYPE_SICK_ALT)
                continue;

            litter_count--;
            sick_count++;
        }
    }

    if (crowded >= 10 && peep->state == PEEP_STATE_WALKING && (scenario_rand() & 0xFFFF) <= 21845)
    {

        peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_CROWDED, PEEP_THOUGHT_ITEM_NONE);
        peep->happiness_target = std::max(0, peep->happiness_target - 14);
    }

    litter_count = std::min(static_cast<uint8_t>(3), litter_count);
    sick_count   = std::min(static_cast<uint8_t>(3), sick_count);

    uint8_t disgusting_time  = peep->disgusting_count & 0xC0;
    uint8_t disgusting_count = ((peep->disgusting_count & 0xF) << 2) | sick_count;
    peep->disgusting_count = disgusting_count | disgusting_time;

    if (disgusting_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the disgusting time
        peep->disgusting_count -= 0x40;
    }
    else
    {
        uint8_t total_sick = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            total_sick += (disgusting_count >> (2 * time)) & 0x3;
        }

        if (total_sick >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_PATH_DISGUSTING, PEEP_THOUGHT_ITEM_NONE);
            peep->happiness_target = std::max(0, peep->happiness_target - 17);
            // Reset disgusting time
            peep->disgusting_count |= 0xC0;
        }
    }

    uint8_t litter_time  = peep->litter_count & 0xC0;
    litter_count       = ((peep->litter_count & 0xF) << 2) | litter_count;
    peep->litter_count = litter_count | litter_time;

    if (litter_time & 0xC0 && (scenario_rand() & 0xFFFF) <= 4369)
    {
        // Reduce the litter time
        peep->litter_count -= 0x40;
    }
    else
    {
        uint8_t total_litter = 0;
        for (uint8_t time = 0; time < 3; time++)
        {
            total_litter += (litter_count >> (2 * time)) & 0x3;
        }

        if (total_litter >= 3 && (scenario_rand() & 0xFFFF) <= 10922)
        {
            peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_BAD_LITTER, PEEP_THOUGHT_ITEM_NONE);
            peep->happiness_target = std::max(0, peep->happiness_target - 17);
            // Reset litter time
            peep->litter_count |= 0xC0;
        }
    }

    peep->Invalidate();
    sprite_move(x, y, z, (rct_sprite *)peep);
    peep->Invalidate();
}

/**
 *
 *  rct2: 0x0069455E
 */
static void peep_interact_with_path(rct_peep * peep, int16_t x, int16_t y, rct_tile_element * tile_element)
{
    // 0x00F1AEE2
    bool vandalism_present = false;
    if (footpath_element_has_path_scenery(tile_element) && (tile_element->flags & TILE_ELEMENT_FLAG_BROKEN) &&
        (tile_element->properties.path.edges & 0xF) != 0xF)
    {
        vandalism_present = true;
    }

    int16_t z = tile_element->base_height * 8;
    if (!map_is_location_owned(x, y, z))
    {
        if (peep->outside_of_park == 0)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }
    }
    else
    {
        if (peep->outside_of_park == 1)
        {
            peep_return_to_centre_of_tile(peep);
            return;
        }
    }

    if (peep->type == PEEP_TYPE_GUEST && footpath_element_is_queue(tile_element))
    {

        uint8_t rideIndex = tile_element->properties.path.ride_index;

        if (peep->state == PEEP_STATE_QUEUING)
        {
            // Check if this queue is connected to the ride the
            // peep is queuing for, i.e. the player hasn't edited
            // the queue, rebuilt the ride, etc.
            if (peep->current_ride == rideIndex)
            {
                peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
                return;
            }
            // Queue got disconnected from the original ride.
            peep->interaction_ride_index = 0xFF;
            peep->RemoveFromQueue();
            peep->SetState(PEEP_STATE_1);
            peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
            return;
        }

        // Peep is not queuing.
        peep->time_lost     = 0;
        uint8_t stationNum = (tile_element->properties.path.additions & 0x70) >> 4;

        if ((tile_element->properties.path.type & (1 << 3)) // Queue has the ride sign on it
            && (footpath_element_get_direction(tile_element) ==
                ((peep->direction) ^ 2)) // Ride sign is facing the direction the peep is walking
        )
        {
            /* Peep is approaching the entrance of a ride queue.
             * Decide whether to go on the ride. */
            if (!peep->ShouldGoOnRide(rideIndex, stationNum, true, false))
            {
                // Peep has decided not to go on the ride.
                peep_return_to_centre_of_tile(peep);
                return;
            }
        }
        else
        {
            /* Peep is approaching a queue tile without a ride
             * sign facing the peep. */
            peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
            return;
        }

        // Peep has decided to go on the ride at the queue.
        peep->interaction_ride_index = rideIndex;
        Ride * ride                  = get_ride(rideIndex);

        // Add the peep to the ride queue.
        uint16_t old_last_peep                 = ride->last_peep_in_queue[stationNum];
        ride->last_peep_in_queue[stationNum] = peep->sprite_index;
        peep->next_in_queue                  = old_last_peep;
        ride->queue_length[stationNum]++;

        peep_decrement_num_riders(peep);
        peep->current_ride         = rideIndex;
        peep->current_ride_station = stationNum;
        peep->state                = PEEP_STATE_QUEUING;
        peep->days_in_queue        = 0;
        peep_window_state_update(peep);

        peep->sub_state             = 10;
        peep->destination_tolerance = 2;
        peep->time_in_queue         = 0;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32_t, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32_t, ride->name_arguments);
            if (gConfigNotifications.guest_queuing_for_ride)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, STR_PEEP_TRACKING_PEEP_JOINED_QUEUE_FOR_X, peep->sprite_index);
            }
        }

        peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
    }
    else
    {
        peep->interaction_ride_index = 0xFF;
        if (peep->state == PEEP_STATE_QUEUING)
        {
            peep->RemoveFromQueue();
            peep->SetState(PEEP_STATE_1);
        }
        peep_footpath_move_forward(peep, x, y, tile_element, vandalism_present);
    }
}

/**
 *
 *  rct2: 0x00693F70
 */
static bool peep_interact_with_shop(rct_peep * peep, int16_t x, int16_t y, rct_tile_element * tile_element)
{
    uint8_t  rideIndex = track_element_get_ride_index(tile_element);
    Ride * ride      = get_ride(rideIndex);

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return false;

    if (peep->type == PEEP_TYPE_STAFF)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    peep->time_lost = 0;

    if (ride->status != RIDE_STATUS_OPEN)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (peep->interaction_ride_index == rideIndex)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
    {
        peep_return_to_centre_of_tile(peep);
        return true;
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY))
    {
        peep->time_lost = 0;
        if (!peep->ShouldGoOnRide(rideIndex, 0, false, false))
        {
            peep_return_to_centre_of_tile(peep);
            return true;
        }

        money16 cost = ride->price;
        if (cost != 0 && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            ride->total_profit += cost;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
            // TODO: Refactor? SpendMoney previously accepted nullptr to not track money, passing a temporary variable as a
            // workaround
            money16 money = 0;
            peep->SpendMoney(money, cost);
        }
        peep->destination_x         = (x & 0xFFE0) + 16;
        peep->destination_y         = (y & 0xFFE0) + 16;
        peep->destination_tolerance = 3;

        peep->current_ride = rideIndex;
        peep->SetState(PEEP_STATE_ENTERING_RIDE);
        peep->sub_state    = PEEP_SHOP_APPROACH;

        peep->time_on_ride = 0;
        ride->cur_num_customers++;
        if (peep->peep_flags & PEEP_FLAGS_TRACKING)
        {
            set_format_arg(0, rct_string_id, peep->name_string_idx);
            set_format_arg(2, uint32_t, peep->id);
            set_format_arg(6, rct_string_id, ride->name);
            set_format_arg(8, uint32_t, ride->name_arguments);
            rct_string_id string_id = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IN_RIDE) ? STR_PEEP_TRACKING_PEEP_IS_IN_X
                                                                                             : STR_PEEP_TRACKING_PEEP_IS_ON_X;
            if (gConfigNotifications.guest_used_facility)
            {
                news_item_add_to_queue(NEWS_ITEM_PEEP_ON_RIDE, string_id, peep->sprite_index);
            }
        }
    }
    else
    {
        if (peep->guest_heading_to_ride_id == rideIndex)
            peep->guest_heading_to_ride_id = 0xFF;
        peep->action_sprite_image_offset = _unk_F1AEF0;
        peep->SetState(PEEP_STATE_BUYING);
        peep->current_ride = rideIndex;
        peep->sub_state    = 0;
    }

    return true;
}

bool is_valid_path_z_and_direction(rct_tile_element * tileElement, int32_t currentZ, int32_t currentDirection)
{
    if (footpath_element_is_sloped(tileElement))
    {
        int32_t slopeDirection = footpath_element_get_slope_direction(tileElement);
        if (slopeDirection == currentDirection)
        {
            if (currentZ != tileElement->base_height)
                return false;
        }
        else
        {
            slopeDirection ^= 2;
            if (slopeDirection != currentDirection)
                return false;
            if (currentZ != tileElement->base_height + 2)
                return false;
        }
    }
    else
    {
        if (currentZ != tileElement->base_height)
            return false;
    }
    return true;
}

void rct_peep::PerformNextAction(uint8_t & pathing_result)
{
    rct_tile_element * tmpTile;
    PerformNextAction(pathing_result, tmpTile);
}

/**
 *
 *  rct2: 0x00693C9E
 */
void rct_peep::PerformNextAction(uint8_t & pathing_result, rct_tile_element * & tile_result)
{
    pathing_result = 0;
    uint8_t previousAction = action;

    if (action == PEEP_ACTION_NONE_1)
        action = PEEP_ACTION_NONE_2;

    if (state == PEEP_STATE_QUEUING)
    {
        if (peep_update_queue_position(this, previousAction))
            return;
    }

    int16_t actionX, actionY, xy_dist;
    if (!UpdateAction(&actionX, &actionY, &xy_dist))
    {
        pathing_result |= PATHING_DESTINATION_REACHED;
        uint8_t result = 0;
        if (type == PEEP_TYPE_GUEST)
        {
            result = guest_path_finding(this);
        }
        else
        {
            result = staff_path_finding(this);
        }

        if (result != 0)
            return;

        if (!UpdateAction(&actionX, &actionY, &xy_dist))
            return;
    }

    if ((actionX & 0xFFE0) == next_x && (actionY & 0xFFE0) == next_y)
    {
        int16_t height = GetZOnSlope(actionX, actionY);
        Invalidate();
        MoveTo(actionX, actionY, height);
        Invalidate();
        return;
    }

    if (actionX < 32 || actionY < 32 || actionX >= gMapSizeUnits || actionY >= gMapSizeUnits)
    {
        if (outside_of_park == 1)
        {
            pathing_result |= PATHING_OUTSIDE_PARK;
        }
        peep_return_to_centre_of_tile(this);
        return;
    }

    rct_tile_element * tileElement = map_get_first_element_at(actionX / 32, actionY / 32);
    int16_t            base_z     = std::max(0, (z / 8) - 2);
    int16_t            top_z      = (z / 8) + 1;

    do
    {
        if (base_z > tileElement->base_height)
            continue;
        if (top_z < tileElement->base_height)
            continue;
        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            peep_interact_with_path(this, actionX, actionY, tileElement);
            tile_result = tileElement;
            return;
        }
        else if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (peep_interact_with_shop(this, actionX, actionY, tileElement))
            {
                tile_result = tileElement;
                return;
            }
        }
        else if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            peep_interact_with_entrance(this, actionX, actionY, tileElement, pathing_result);
            tile_result = tileElement;
            return;
        }
    } while (!(tileElement++)->IsLastForTile());

    if (type == PEEP_TYPE_STAFF || (GetNextIsSurface()))
    {
        int16_t height = abs(tile_element_height(actionX, actionY) - z);

        if (height <= 3 || (type == PEEP_TYPE_STAFF && height <= 32))
        {
            interaction_ride_index = 0xFF;
            if (state == PEEP_STATE_QUEUING)
            {
                RemoveFromQueue();
                SetState(PEEP_STATE_1);
            }

            if (!map_is_location_in_park({actionX & 0xFFE0, actionY & 0xFFE0}))
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            tileElement = map_get_surface_element_at({ actionX, actionY });
            if (tileElement == nullptr)
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            int16_t water_height = surface_get_water_height(tileElement);
            if (water_height)
            {
                peep_return_to_centre_of_tile(this);
                return;
            }

            if (type == PEEP_TYPE_STAFF && !GetNextIsSurface())
            {
                // Prevent staff from leaving the path on their own unless they're allowed to mow.
                if (!((this->staff_orders & STAFF_ORDERS_MOWING) && this->staff_mowing_timeout >= 12))
                {
                    peep_return_to_centre_of_tile(this);
                    return;
                }
            }

            // The peep is on a surface and not on a path
            next_x      = actionX & 0xFFE0;
            next_y      = actionY & 0xFFE0;
            next_z      = tileElement->base_height;
            SetNextFlags(0, false, true);

            height = GetZOnSlope(actionX, actionY);
            Invalidate();
            MoveTo(actionX, actionY, height);
            Invalidate();
            return;
        }
    }

    peep_return_to_centre_of_tile(this);
}

/**
 *
 *  rct2: 0x0069A98C
 */
void peep_reset_pathfind_goal(rct_peep * peep)
{

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug)
    {
        log_info("Resetting pathfind_goal for %s", gPathFindDebugPeepName);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    peep->pathfind_goal.x         = 0xFF;
    peep->pathfind_goal.y         = 0xFF;
    peep->pathfind_goal.z         = 0xFF;
    peep->pathfind_goal.direction = 0xFF;
}

static bool peep_has_valid_xy(rct_peep * peep)
{
    if (peep->x != LOCATION_NULL)
    {
        if (peep->x < (256 * 32) && peep->y < (256 * 32))
        {
            return true;
        }
    }

    return false;
}

using easter_egg_function = void (*)(rct_peep * peep, rct_peep * otherPeep);

static void peep_apply_easter_egg_to_nearby_guests(rct_peep * peep, easter_egg_function easter_egg)
{
    if (!peep_has_valid_xy(peep))
        return;

    uint16_t spriteIndex = sprite_get_first_in_quadrant(peep->x, peep->y);
    if (spriteIndex == SPRITE_INDEX_NULL)
        return;

    rct_peep * otherPeep = GET_PEEP(spriteIndex);
    for (; spriteIndex != SPRITE_INDEX_NULL; spriteIndex = otherPeep->next_in_quadrant)
    {
        otherPeep = GET_PEEP(spriteIndex);

        if (otherPeep->sprite_identifier != SPRITE_IDENTIFIER_PEEP)
            continue;

        if (otherPeep->type != PEEP_TYPE_GUEST)
            continue;

        int32_t zDiff = abs(otherPeep->z - peep->z);
        if (zDiff > 32)
            continue;

        easter_egg(peep, otherPeep);
    }
}

static void peep_give_passing_peeps_purple_clothes([[maybe_unused]] rct_peep* peep, rct_peep* otherPeep)
{
    otherPeep->tshirt_colour   = COLOUR_BRIGHT_PURPLE;
    otherPeep->trousers_colour = COLOUR_BRIGHT_PURPLE;
    invalidate_sprite_2((rct_sprite *)otherPeep);
}

static void peep_give_passing_peeps_pizza(rct_peep * peep, rct_peep * otherPeep)
{
    if ((otherPeep->item_standard_flags & PEEP_ITEM_PIZZA))
        return;

    otherPeep->item_standard_flags |= PEEP_ITEM_PIZZA;

    int32_t peepDirection              = (peep->sprite_direction >> 3) ^ 2;
    int32_t otherPeepOppositeDirection = otherPeep->sprite_direction >> 3;
    if (peepDirection == otherPeepOppositeDirection)
    {
        if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2)
        {
            peep->Invalidate();
            otherPeep->action                     = PEEP_ACTION_WAVE_2;
            otherPeep->action_frame               = 0;
            otherPeep->action_sprite_image_offset = 0;
            otherPeep->UpdateCurrentActionSpriteType();
            invalidate_sprite_2((rct_sprite *)otherPeep);
        }
    }
    invalidate_sprite_2((rct_sprite *)otherPeep);
}

static void peep_make_passing_peeps_sick(rct_peep * peep, rct_peep * otherPeep)
{
    if (peep == otherPeep)
        return;
    if (otherPeep->state != PEEP_STATE_WALKING)
        return;

    if (otherPeep->action == PEEP_ACTION_NONE_1 || otherPeep->action == PEEP_ACTION_NONE_2)
    {
        otherPeep->action                     = PEEP_ACTION_THROW_UP;
        otherPeep->action_frame               = 0;
        otherPeep->action_sprite_image_offset = 0;
        otherPeep->UpdateCurrentActionSpriteType();
        invalidate_sprite_2((rct_sprite *)otherPeep);
    }
}

static void peep_give_passing_peeps_ice_cream(rct_peep * peep, rct_peep * otherPeep)
{
    if (peep == otherPeep)
        return;
    if (otherPeep->item_standard_flags & PEEP_ITEM_ICE_CREAM)
        return;

    otherPeep->item_standard_flags |= PEEP_ITEM_ICE_CREAM;
    otherPeep->UpdateSpriteType();
}

/**
 *
 *  rct2: 0x0068FD3A
 */
static void peep_easter_egg_peep_interactions(rct_peep * peep)
{
    if (peep->peep_flags & PEEP_FLAGS_PURPLE)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_purple_clothes);
    }

    if (peep->peep_flags & PEEP_FLAGS_PIZZA)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_pizza);
    }

    if (peep->peep_flags & PEEP_FLAGS_CONTAGIOUS)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_make_passing_peeps_sick);
    }

    if (peep->peep_flags & PEEP_FLAGS_JOY)
    {
        if (scenario_rand() <= 1456)
        {
            if (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2)
            {
                peep->action                     = PEEP_ACTION_JOY;
                peep->action_frame               = 0;
                peep->action_sprite_image_offset = 0;
                peep->UpdateCurrentActionSpriteType();
                peep->Invalidate();
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_ICE_CREAM)
    {
        peep_apply_easter_egg_to_nearby_guests(peep, &peep_give_passing_peeps_ice_cream);
    }
}

/**
 * Gets the height including the bit depending on how far up the slope the peep
 * is.
 *  rct2: 0x00694921
 */
int32_t rct_peep::GetZOnSlope(int32_t tile_x, int32_t tile_y)
{
    if (tile_x == LOCATION_NULL)
        return 0;

    if (GetNextIsSurface())
    {
        return tile_element_height(tile_x, tile_y) & 0xFFFF;
    }

    int32_t height = next_z * 8;
    uint8_t slope = GetNextDirection();
    slope |= GetNextIsSloped() ? (1 << 2) : 0;
    return height + map_height_from_slope(tile_x, tile_y, slope);
}

/**
 *
 *  rct2: 0x0069C483
 */
static void peep_give_real_name(rct_peep * peep)
{
    // Generate a name_string_idx from the peep id using bit twiddling
    uint16_t ax = (uint16_t)(peep->id + 0xF0B);
    uint16_t dx = 0;
    static constexpr uint16_t twiddlingBitOrder[] = { 4, 9, 3, 7, 5, 8, 2, 1, 6, 0, 12, 11, 13, 10 };
    for (size_t i = 0; i < Util::CountOf(twiddlingBitOrder); i++)
    {
        dx |= (ax & (1 << twiddlingBitOrder[i]) ? 1 : 0) << i;
    }
    ax = dx & 0xF;
    dx *= 4;
    ax *= 4096;
    dx += ax;
    if (dx < ax)
    {
        dx += 0x1000;
    }
    dx /= 4;
    dx += REAL_NAME_START;
    peep->name_string_idx = dx;
}

static int32_t peep_compare(const void * sprite_index_a, const void * sprite_index_b)
{
    rct_peep const * peep_a = GET_PEEP(*(uint16_t *)sprite_index_a);
    rct_peep const * peep_b = GET_PEEP(*(uint16_t *)sprite_index_b);

    // Compare types
    if (peep_a->type != peep_b->type)
    {
        return peep_a->type - peep_b->type;
    }

    // Simple ID comparison for when both peeps use a number or a generated name
    const bool both_numbers = (peep_a->name_string_idx >= 767 && peep_a->name_string_idx <= 771 &&
                               peep_b->name_string_idx >= 767 && peep_b->name_string_idx <= 771);
    if (both_numbers)
    {
        return peep_a->id - peep_b->id;
    }
    const bool both_have_generated_names = (peep_a->name_string_idx >= REAL_NAME_START && peep_a->name_string_idx <= REAL_NAME_END &&
                                            peep_b->name_string_idx >= REAL_NAME_START && peep_b->name_string_idx <= REAL_NAME_END);
    if (both_have_generated_names)
    {
        rct_string_id peep_a_format = peep_a->name_string_idx + REAL_NAME_START;
        rct_string_id peep_b_format = peep_b->name_string_idx + REAL_NAME_START;

        uint16_t peep_a_name = (peep_a_format % Util::CountOf(real_names));
        uint16_t peep_b_name = (peep_b_format % Util::CountOf(real_names));

        if (peep_a_name == peep_b_name)
        {
            uint16_t peep_a_initial = ((peep_a_format >> 10) % Util::CountOf(real_name_initials));
            uint16_t peep_b_initial = ((peep_b_format >> 10) % Util::CountOf(real_name_initials));
            return peep_a_initial - peep_b_initial;
        }
        else
        {
            return peep_a_name - peep_b_name;
        }
    }

    // At least one of them has a custom name assigned
    // Compare their names as strings
    utf8   name_a[256];
    utf8   name_b[256];
    uint32_t peepIndex = peep_a->id;
    format_string(name_a, 256, peep_a->name_string_idx, &peepIndex);
    peepIndex = peep_b->id;
    format_string(name_b, 256, peep_b->name_string_idx, &peepIndex);
    return strlogicalcmp(name_a, name_b);
}

/**
 *
 *  rct2: 0x00699115
 */
void peep_update_name_sort(rct_peep * peep)
{
    // Remove peep from sprite list
    uint16_t nextSpriteIndex = peep->next;
    uint16_t prevSpriteIndex = peep->previous;
    if (prevSpriteIndex != SPRITE_INDEX_NULL)
    {
        rct_peep * prevPeep = GET_PEEP(prevSpriteIndex);
        prevPeep->next      = nextSpriteIndex;
    }
    else
    {
        gSpriteListHead[SPRITE_LIST_PEEP] = nextSpriteIndex;
    }

    if (nextSpriteIndex != SPRITE_INDEX_NULL)
    {
        rct_peep * nextPeep = GET_PEEP(nextSpriteIndex);
        nextPeep->previous  = prevSpriteIndex;
    }

    rct_peep * otherPeep;
    uint16_t     spriteIndex;
    FOR_ALL_PEEPS(spriteIndex, otherPeep)
    {
        // Check if peep should go before this one
        if (peep_compare(&peep->sprite_index, &otherPeep->sprite_index) >= 0)
        {
            continue;
        }

        // Place peep before this one
        peep->previous      = otherPeep->previous;
        otherPeep->previous = peep->sprite_index;
        if (peep->previous != SPRITE_INDEX_NULL)
        {
            rct_peep * prevPeep = GET_PEEP(peep->previous);
            peep->next          = prevPeep->next;
            prevPeep->next      = peep->sprite_index;
        }
        else
        {
            peep->next                        = gSpriteListHead[SPRITE_LIST_PEEP];
            gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
        }
        goto finish_peep_sort;
    }

    // Place peep at the end
    FOR_ALL_PEEPS(spriteIndex, otherPeep)
    {
        if (otherPeep->next == SPRITE_INDEX_NULL)
        {
            otherPeep->next = peep->sprite_index;
            peep->previous  = otherPeep->sprite_index;
            peep->next      = SPRITE_INDEX_NULL;
            goto finish_peep_sort;
        }
    }

    gSpriteListHead[SPRITE_LIST_PEEP] = peep->sprite_index;
    peep->next                        = SPRITE_INDEX_NULL;
    peep->previous                    = SPRITE_INDEX_NULL;

finish_peep_sort:
    // This is required at the moment because this function reorders peeps in the sprite list
    sprite_position_tween_reset();
}

void peep_sort()
{
    // Count number of peeps
    uint16_t     sprite_index, num_peeps = 0;
    rct_peep * peep;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        num_peeps++;
    }

    // No need to sort
    if (num_peeps < 2)
        return;

    // Create a copy of the peep list and sort it using peep_compare
    uint16_t * peep_list = (uint16_t *)malloc(num_peeps * sizeof(uint16_t));
    int32_t   i         = 0;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        peep_list[i++] = peep->sprite_index;
    }
    qsort(peep_list, num_peeps, sizeof(uint16_t), peep_compare);

    // Set the correct peep->next and peep->previous using the sorted list
    for (i = 0; i < num_peeps; i++)
    {
        peep           = GET_PEEP(peep_list[i]);
        peep->previous = (i > 0) ? peep_list[i - 1] : SPRITE_INDEX_NULL;
        peep->next     = (i + 1 < num_peeps) ? peep_list[i + 1] : SPRITE_INDEX_NULL;
    }
    // Make sure the first peep is set
    gSpriteListHead[SPRITE_LIST_PEEP] = peep_list[0];

    free(peep_list);

    i = 0;
    FOR_ALL_PEEPS(sprite_index, peep)
    {
        i++;
    }
    assert(i == num_peeps);
}

/**
 *
 *  rct2: 0x0069926C
 */
void peep_update_names(bool realNames)
{
    if (realNames)
    {
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep * peep;
        uint16_t     spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->name_string_idx == STR_GUEST_X)
            {
                peep_give_real_name(peep);
            }
        }
    }
    else
    {
        gParkFlags &= ~PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        rct_peep * peep;
        uint16_t     spriteIndex;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (peep->name_string_idx >= REAL_NAME_START && peep->name_string_idx <= REAL_NAME_END)
            {
                peep->name_string_idx = STR_GUEST_X;
            }
        }
    }

    peep_sort();
    gfx_invalidate_screen();
}

bool rct_peep::HeadingForRideOrParkExit() const
{
    return (peep_flags & PEEP_FLAGS_LEAVING_PARK) || (guest_heading_to_ride_id != 0xFF);
}

void peep_handle_easteregg_name(rct_peep * peep)
{
    peep->peep_flags &= ~PEEP_FLAGS_WAVING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_BRAYSHAW, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_WAVING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PHOTO;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CHRIS_SAWYER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PHOTO;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PAINTING;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_SIMON_FOSTER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PAINTING;
    }

    peep->peep_flags &= ~PEEP_FLAGS_WOW;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOHN_WARDLEY, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_WOW;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MELANIE_WARN, peep))
    {
        peep->happiness        = 250;
        peep->happiness_target = 250;
        peep->energy           = 127;
        peep->energy_target    = 127;
        peep->nausea           = 0;
        peep->nausea_target    = 0;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LITTER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_LISA_STIRLING, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LITTER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_LOST;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DONALD_MACRAE, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LOST;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HUNGER;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATHERINE_MCGOWAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HUNGER;
    }

    peep->peep_flags &= ~PEEP_FLAGS_BATHROOM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FRANCES_MCGOWAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_BATHROOM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CROWDED;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CORINA_MASSOURA, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_CROWDED;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HAPPINESS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_CAROL_YOUNG, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HAPPINESS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_NAUSEA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_MIA_SHERIDAN, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_NAUSEA;
    }

    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_RODGER, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_LEAVING_PARK;
        peep->peep_flags &= ~PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PURPLE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EMMA_GARRELL, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PURPLE;
    }

    peep->peep_flags &= ~PEEP_FLAGS_PIZZA;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_JOANNE_BARTON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_PIZZA;
    }

    peep->peep_flags &= ~PEEP_FLAGS_CONTAGIOUS;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_FELICITY_ANDERSON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_CONTAGIOUS;
    }

    peep->peep_flags &= ~PEEP_FLAGS_JOY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_KATIE_SMITH, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_JOY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ANGRY;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_EILIDH_BELL, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_ANGRY;
    }

    peep->peep_flags &= ~PEEP_FLAGS_ICE_CREAM;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_NANCY_STILLWAGON, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_ICE_CREAM;
    }

    peep->peep_flags &= ~PEEP_FLAGS_HERE_WE_ARE;
    if (peep_check_easteregg_name(EASTEREGG_PEEP_NAME_DAVID_ELLIS, peep))
    {
        peep->peep_flags |= PEEP_FLAGS_HERE_WE_ARE;
    }
}

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
void pathfind_logging_enable([[maybe_unused]] rct_peep* peep)
{
#if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    /* Determine if the pathfinding debugging is wanted for this peep. */
    format_string(gPathFindDebugPeepName, sizeof(gPathFindDebugPeepName), peep->name_string_idx, &(peep->id));

    /* For guests, use the existing PEEP_FLAGS_TRACKING flag to
     * determine for which guest(s) the pathfinding debugging will
     * be output for. */
    if (peep->type == PEEP_TYPE_GUEST)
    {
        gPathFindDebug = peep->peep_flags & PEEP_FLAGS_TRACKING;
    }
    /* For staff, there is no tracking button (any other similar
     * suitable existing mechanism?), so fall back to a crude
     * string comparison with a compile time hardcoded name. */
    else
    {
        gPathFindDebug = strcmp(gPathFindDebugPeepName, "Mechanic Debug") == 0;
    }
#endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}

void pathfind_logging_disable()
{
#if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    gPathFindDebug = false;
#endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

void increment_guests_in_park()
{
    if (gNumGuestsInPark < UINT16_MAX)
    {
        gNumGuestsInPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests in park above max value (65535).");
    }
}

void increment_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark < UINT16_MAX)
    {
        gNumGuestsHeadingForPark++;
    }
    else
    {
        openrct2_assert(false, "Attempt to increment guests heading for park above max value (65535).");
    }
}

void decrement_guests_in_park()
{
    if (gNumGuestsInPark > 0)
    {
        gNumGuestsInPark--;
    }
    else
    {
        log_error("Attempt to decrement guests in park below zero.");
    }
}

void decrement_guests_heading_for_park()
{
    if (gNumGuestsHeadingForPark > 0)
    {
        gNumGuestsHeadingForPark--;
    }
    else
    {
        log_error("Attempt to decrement guests heading for park below zero.");
    }
}

static void peep_release_balloon(rct_peep * peep, int16_t spawn_height)
{
    if (peep->item_standard_flags & PEEP_ITEM_BALLOON)
    {
        peep->item_standard_flags &= ~PEEP_ITEM_BALLOON;

        if (peep->sprite_type == PEEP_SPRITE_TYPE_BALLOON && peep->x != LOCATION_NULL)
        {
            create_balloon(peep->x, peep->y, spawn_height, peep->balloon_colour, false);
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_INVENTORY;
            peep->UpdateSpriteType();
        }
    }
}
